// utils.cpp
#include "utils.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<int> nms_boxes(const std::vector<cv::Rect>& boxes, const std::vector<float>& box_confidences, float nms_threshold, int max_detection_number) {
    std::vector<int> x_coord, y_coord, width, height;
    std::vector<float> areas;
    for (const auto& box : boxes) {
        x_coord.push_back(box.x);
        y_coord.push_back(box.y);
        width.push_back(box.width);
        height.push_back(box.height);
        areas.push_back(static_cast<float>(box.width * box.height));
    }

    std::vector<int> ordered(box_confidences.size());
    for (size_t i = 0; i < ordered.size(); ++i) {
        ordered[i] = static_cast<int>(i);
    }
    std::sort(ordered.begin(), ordered.end(), [&box_confidences](int a, int b) {
        return box_confidences[a] > box_confidences[b];
        });

    std::vector<int> keep;
    while (!ordered.empty()) {
        int i = ordered[0];
        keep.push_back(i);
        std::vector<float> xx1, yy1, xx2, yy2;
        for (size_t j = 1; j < ordered.size(); ++j) {
            xx1.push_back(std::max(x_coord[i], x_coord[ordered[j]]));
            yy1.push_back(std::max(y_coord[i], y_coord[ordered[j]]));
            xx2.push_back(std::min(x_coord[i] + width[i], x_coord[ordered[j]] + width[ordered[j]]));
            yy2.push_back(std::min(y_coord[i] + height[i], y_coord[ordered[j]] + height[ordered[j]]));
        }
        std::vector<float> width1, height1;
        for (size_t j = 0; j < xx1.size(); ++j) {
            width1.push_back(std::max(0.0f, xx2[j] - xx1[j] + 1));
            height1.push_back(std::max(0.0f, yy2[j] - yy1[j] + 1));
        }
        std::vector<float> intersection;
        for (size_t j = 0; j < width1.size(); ++j) {
            intersection.push_back(width1[j] * height1[j]);
        }
        std::vector<float> union_;
        for (size_t j = 0; j < intersection.size(); ++j) {
            union_.push_back(areas[i] + areas[ordered[j + 1]] - intersection[j]);
        }
        std::vector<float> iou;
        for (size_t j = 0; j < union_.size(); ++j) {
            iou.push_back(intersection[j] / union_[j]);
        }
        std::vector<int> indexes;
        for (size_t j = 0; j < iou.size(); ++j) {
            if (iou[j] <= nms_threshold) {
                indexes.push_back(j);
            }
        }
        std::vector<int> new_ordered;
        for (int index : indexes) {
            new_ordered.push_back(ordered[index + 1]);
        }
        ordered = new_ordered;
    }
    if (keep.size() > static_cast<size_t>(max_detection_number)) {
        keep.resize(max_detection_number);
    }
    return keep;
}

std::vector<cv::Rect> bbox_transform_inv(const std::vector<cv::Rect>& boxes, const std::vector<cv::Vec4f>& deltas) {
    std::vector<cv::Rect> result;
    for (size_t i = 0; i < boxes.size(); ++i) {
        float cxa = (boxes[i].x + boxes[i].x + boxes[i].width) / 2.0f;
        float cya = (boxes[i].y + boxes[i].y + boxes[i].height) / 2.0f;
        float wa = boxes[i].width;
        float ha = boxes[i].height;

        float tx = deltas[i][1];
        float ty = deltas[i][0];
        float tw = deltas[i][3];
        float th = deltas[i][2];

        float w = std::exp(tw) * wa;
        float h = std::exp(th) * ha;
        float cx = tx * wa + cxa;
        float cy = ty * ha + cya;

        result.emplace_back(cv::Rect(cx - w / 2, cy - h / 2, w, h));
    }
    return result;
}

std::vector<cv::Rect> pre_coord(const std::vector<cv::Rect>& pre_abnormal_coord, const std::vector<cv::Rect>& boxes, int box_number_save) {
    std::vector<cv::Rect> result;
    for (const auto& box : boxes) {
        for (const auto& coord : pre_abnormal_coord) {
            float box_center_x = (box.x + box.width) / 2.0f;
            float box_center_y = (box.y + box.height) / 2.0f;
            float coord_center_x = (coord.x + coord.width) / 2.0f;
            float coord_center_y = (coord.y + coord.height) / 2.0f;

            if (box_center_x > coord.x && box_center_x < coord.x + coord.width && box_center_y > coord.y && box_center_y < coord.y + coord.height) {
                result.push_back(box);
                if (result.size() >= static_cast<size_t>(box_number_save)) {
                    return result;
                }
            }
        }
    }
    return result;
}

cv::Mat preprocess_image(cv::Mat image, int image_size, float& scale) {
    int raw_height = image.rows;
    int raw_width = image.cols;
    float aspect_ratio = static_cast<float>(raw_height) / raw_width;

    int resized_height, resized_width;
    if (raw_height > raw_width) {
        resized_height = image_size;
        resized_width = static_cast<int>(image_size / aspect_ratio);
    }
    else {
        resized_width = image_size;
        resized_height = static_cast<int>(image_size * aspect_ratio);
    }
    scale = static_cast<float>(resized_height) / raw_height;

    cv::Mat resized_image;
    cv::resize(image, resized_image, cv::Size(resized_width, resized_height));
    resized_image.convertTo(resized_image, CV_32F, 1.0 / 255.0);

    cv::Mat mean = (cv::Mat_<float>(1, 3) << 0.485, 0.456, 0.406);
    cv::Mat std = (cv::Mat_<float>(1, 3) << 0.229, 0.224, 0.225);
    resized_image = (resized_image - mean) / std;

    int pad_h = image_size - resized_height;
    int pad_w = image_size - resized_width;
    cv::copyMakeBorder(resized_image, resized_image, 0, pad_h, 0, pad_w, cv::BORDER_CONSTANT, 0);

    return resized_image;
}

cv::Mat postprocess_image(cv::Mat image, int raw_height, int raw_width, float scale) {
    int re_image_size = image.rows;

    int resized_height, resized_width;
    if (raw_height > raw_width) {
        resized_height = re_image_size;
        resized_width = static_cast<int>(raw_width * scale);
    }
    else {
        resized_width = re_image_size;
        resized_height = static_cast<int>(raw_height * scale);
    }

    int crop_h = re_image_size - resized_height;
    int crop_w = re_image_size - resized_width;

    if (crop_h == 0) {
        image = image(cv::Rect(0, 0, image.cols - crop_w, image.rows));
    }
    if (crop_w == 0) {
        image = image(cv::Rect(0, 0, image.cols, image.rows - crop_h));
    }

    cv::resize(image, image, cv::Size(raw_width, raw_height));
    return image;
}

std::vector<cv::Rect> postprocess_boxes(const std::vector<cv::Rect>& boxes, float scale, int height, int width) {
    std::vector<cv::Rect> result;
    for (const auto& box : boxes) {
        int x1 = std::max(0, std::min(width - 1, static_cast<int>(box.x / scale)));
        int y1 = std::max(0, std::min(height - 1, static_cast<int>(box.y / scale)));
        int x2 = std::max(0, std::min(width - 1, static_cast<int>((box.x + box.width) / scale)));
        int y2 = std::max(0, std::min(height - 1, static_cast<int>((box.y + box.height) / scale)));
        result.emplace_back(cv::Rect(x1, y1, x2 - x1, y2 - y1));
    }
    return result;
}

void draw_box(cv::Mat& image, const std::vector<cv::Rect>& result_output, int frame_number, const std::string& patient_number, const std::string& screening_start_time) {
    for (const auto& rect : result_output) {
        cv::rectangle(image, rect, cv::Scalar(0, 255, 0), 2);
    }

    std::string path = "./snapshot_result/" + patient_number + "_" + screening_start_time;
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }

    std::string filename = path + "/" + patient_number + "_" + screening_start_time + "_" + std::to_string(frame_number) + ".jpg";
    cv::imwrite(filename, image);
}
