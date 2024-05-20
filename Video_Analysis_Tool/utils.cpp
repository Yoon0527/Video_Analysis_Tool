
// Include necessary libraries
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <filesystem>

using namespace cv;
using namespace std;
namespace fs = std::filesystem;

// Define sigmoid function
double sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

// Non-maximum suppression function
std::vector<int> nms_boxes(const std::vector<cv::Rect>& boxes, const std::vector<float>& box_confidences, float nms_threshold, int max_detection_number) {
    std::vector<int> keep;
    std::vector<float> areas(boxes.size());
    for (int i = 0; i < boxes.size(); ++i) {
        areas[i] = boxes[i].width * boxes[i].height;
    }

    std::vector<int> ordered(boxes.size());
    std::iota(ordered.begin(), ordered.end(), 0);
    std::sort(ordered.begin(), ordered.end(), [&box_confidences](int i, int j) { return box_confidences[i] > box_confidences[j]; });

    while (!ordered.empty()) {
        int i = ordered[0];
        keep.push_back(i);
        cv::Rect box_i = boxes[i];

        ordered.erase(ordered.begin());
        for (int j : ordered) {
            cv::Rect box_j = boxes[j];
            int xx1 = std::max(box_i.x, box_j.x);
            int yy1 = std::max(box_i.y, box_j.y);
            int xx2 = std::min(box_i.x + box_i.width, box_j.x + box_j.width);
            int yy2 = std::min(box_i.y + box_i.height, box_j.y + box_j.height);

            float width1 = std::max(0.0f, static_cast<float>(xx2 - xx1 + 1));
            float height1 = std::max(0.0f, static_cast<float>(yy2 - yy1 + 1));
            float intersection = width1 * height1;
            float union_area = areas[i] + areas[j] - intersection;
            float iou = intersection / union_area;

            if (iou > nms_threshold) {
                ordered.erase(std::remove(ordered.begin(), ordered.end(), j), ordered.end());
            }
        }

        if (keep.size() >= max_detection_number) {
            break;
        }
    }

    return keep;
}

// Bounding box transform inverse function
std::vector<cv::Rect> bbox_transform_inv(const std::vector<cv::Rect>& boxes, const std::vector<float>& deltas) {
    std::vector<cv::Rect> result;
    for (int i = 0; i < boxes.size(); ++i) {
        float cxa = (boxes[i].x + boxes[i].x + boxes[i].width) / 2.0f;
        float cya = (boxes[i].y + boxes[i].y + boxes[i].height) / 2.0f;
        float wa = boxes[i].width;
        float ha = boxes[i].height;
        float ty = deltas[4 * i];
        float tx = deltas[4 * i + 1];
        float th = deltas[4 * i + 2];
        float tw = deltas[4 * i + 3];

        float w = sigmoid(tw) * wa;
        float h = sigmoid(th) * ha;
        float cy = ty * ha + cya;
        float cx = tx * wa + cxa;
        float ymin = cy - h / 2.0f;
        float xmin = cx - w / 2.0f;
        float ymax = cy + h / 2.0f;
        float xmax = cx + w / 2.0f;

        result.emplace_back(cv::Rect(xmin, ymin, xmax - xmin, ymax - ymin));
    }
    return result;
}

// Pre-coordinate function
std::vector<int> pre_coord(const std::vector<std::vector<int>>& pre_abnormal_coord, const std::vector<cv::Rect>& boxes) {
    std::vector<int> box_number_save;
    for (int box_number = 0; box_number < boxes.size(); ++box_number) {
        float box_center_x = (boxes[box_number].x + boxes[box_number].width / 2.0f);
        float box_center_y = (boxes[box_number].y + boxes[box_number].height / 2.0f);
        for (const auto& pre_abnormal_coord_value : pre_abnormal_coord) {
            if (box_center_x >= pre_abnormal_coord_value[0] && box_center_x <= pre_abnormal_coord_value[2] &&
                box_center_y >= pre_abnormal_coord_value[1] && box_center_y <= pre_abnormal_coord_value[3]) {
                box_number_save.push_back(box_number);
                break;
            }
        }
    }
    std::sort(box_number_save.begin(), box_number_save.end());
    box_number_save.erase(std::unique(box_number_save.begin(), box_number_save.end()), box_number_save.end());
    return box_number_save;
}

// Text boxes function
std::vector<cv::Rect> txt_boxes(const std::vector<cv::Rect>& boxes, const std::vector<std::vector<int>>& pre_abnormal_coord_list) {
    std::vector<cv::Rect> result;
    std::vector<std::vector<int>> filtered_pre_abnormal_coord_list;
    for (const auto& coord : pre_abnormal_coord_list) {
        if (coord != std::vector<int>{0, 0, 0, 0}) {
            filtered_pre_abnormal_coord_list.push_back(coord);
        }
    }
    if (filtered_pre_abnormal_coord_list.size() > 5) {
        filtered_pre_abnormal_coord_list.erase(filtered_pre_abnormal_coord_list.begin(), filtered_pre_abnormal_coord_list.end() - 5);
    }

    for (const auto& box : boxes) {
        int xmin = box.x;
        int ymin = box.y;
        int xmax = box.x + box.width;
        int ymax = box.y + box.height;
        if (!filtered_pre_abnormal_coord_list.empty()) {
            std::vector<int> sum_coords(4, 0);
            for (const auto& coord : filtered_pre_abnormal_coord_list) {
                sum_coords[0] += coord[0];
                sum_coords[1] += coord[1];
                sum_coords[2] += coord[2];
                sum_coords[3] += coord[3];
            }
            int num_coords = filtered_pre_abnormal_coord_list.size();
            xmin = (sum_coords[0] + xmin) / (num_coords + 1);
            ymin = (sum_coords[1] + ymin) / (num_coords + 1);
            xmax = (sum_coords[2] + xmax) / (num_coords + 1);
            ymax = (sum_coords[3] + ymax) / (num_coords + 1);
        }
        result.emplace_back(xmin, ymin, xmax - xmin, ymax - ymin);
    }
    return result;
}

// Pre-coordinate exploration function
std::tuple<std::vector<std::vector<std::vector<int>>>, std::vector<std::vector<int>>, std::vector<std::vector<cv::Rect>>> pre_coord_explore(const std::vector<std::vector<std::vector<int>>>& pre_abnormal_coord_list, const std::vector<std::vector<int>>& abnormal_number_count_list, const std::vector<cv::Rect>& boxes, int abnormal_count) {
    std::vector<std::vector<std::vector<int>>> new_pre_abnormal_coord_list = pre_abnormal_coord_list;
    std::vector<std::vector<int>> new_abnormal_number_count_list = abnormal_number_count_list;
    std::vector<std::vector<cv::Rect>> box_txt;

    for (int abnormal_save_number = 0; abnormal_save_number < 3; ++abnormal_save_number) {
        std::vector<int> box_number_save;
        std::vector<std::vector<int>> pre_abnormal_coord = new_pre_abnormal_coord_list[abnormal_save_number];
        std::vector<int> abnormal_number_count = new_abnormal_number_count_list[abnormal_save_number];

        if (pre_abnormal_coord != std::vector<std::vector<int>>(10, std::vector<int>{0, 0, 0, 0})) {
            box_number_save = pre_coord(pre_abnormal_coord, boxes);
            if (!box_number_save.empty()) {
                std::vector<cv::Rect> boxes_tmp(boxes.begin(), boxes.end());
                std::vector<cv::Rect> new_boxes;
                for (int i = 0; i < boxes.size(); ++i) {
                    if (std::find(box_number_save.begin(), box_number_save.end(), i) != box_number_save.end()) {
                        new_boxes.push_back(boxes[i]);
                    }
                }
                abnormal_number_count.erase(abnormal_number_count.begin());
                abnormal_number_count.push_back(1);
                for (int i = 0; i < new_boxes.size(); ++i) {
                    pre_abnormal_coord.erase(pre_abnormal_coord.begin());
                    pre_abnormal_coord.push_back({ new_boxes[i].x, new_boxes[i].y, new_boxes[i].x + new_boxes[i].width, new_boxes[i].y + new_boxes[i].height });
                }
                if (std::accumulate(abnormal_number_count.begin(), abnormal_number_count.end(), 0) >= abnormal_count) {
                    box_txt.push_back(txt_boxes(new_boxes, pre_abnormal_coord));
                }
                new_pre_abnormal_coord_list[abnormal_save_number] = pre_abnormal_coord;
                new_abnormal_number_count_list[abnormal_save_number] = abnormal_number_count;
            }
            else {
                pre_abnormal_coord.erase(pre_abnormal_coord.begin());
                pre_abnormal_coord.push_back({ 0, 0, 0, 0 });
                abnormal_number_count.erase(abnormal_number_count.begin());
                abnormal_number_count.push_back(0);
                new_pre_abnormal_coord_list[abnormal_save_number] = pre_abnormal_coord;
                new_abnormal_number_count_list[abnormal_save_number] = abnormal_number_count;
            }
        }
    }

    for (int abnormal_save_number = 0; abnormal_save_number < 3; ++abnormal_save_number) {
        std::vector<std::vector<int>> pre_abnormal_coord = new_pre_abnormal_coord_list[abnormal_save_number];
        std::vector<int> abnormal_number_count = new_abnormal_number_count_list[abnormal_save_number];
        if (!boxes.empty() && pre_abnormal_coord == std::vector<std::vector<int>>(10, std::vector<int>{0, 0, 0, 0})) {
            abnormal_number_count.erase(abnormal_number_count.begin());
            abnormal_number_count.push_back(1);
            pre_abnormal_coord.erase(pre_abnormal_coord.begin());
            pre_abnormal_coord.push_back({ boxes[0].x, boxes[0].y, boxes[0].x + boxes[0].width, boxes[0].y + boxes[0].height });
            new_pre_abnormal_coord_list[abnormal_save_number] = pre_abnormal_coord;
            new_abnormal_number_count_list[abnormal_save_number] = abnormal_number_count;
            break;
        }
    }

    return std::make_tuple(new_pre_abnormal_coord_list, new_abnormal_number_count_list, box_txt);
}

// Pre-coordinate zero append function
std::tuple<std::vector<std::vector<std::vector<int>>>, std::vector<std::vector<int>>> pre_coord_zero_append(const std::vector<std::vector<std::vector<int>>>& pre_abnormal_coord_list, const std::vector<std::vector<int>>& abnormal_number_count_list) {
    std::vector<std::vector<std::vector<int>>> new_pre_abnormal_coord_list = pre_abnormal_coord_list;
    std::vector<std::vector<int>> new_abnormal_number_count_list = abnormal_number_count_list;

    for (int abnormal_save_number = 0; abnormal_save_number < 3; ++abnormal_save_number) {
        std::vector<std::vector<int>> pre_abnormal_coord = new_pre_abnormal_coord_list[abnormal_save_number];
        std::vector<int> abnormal_number_count = new_abnormal_number_count_list[abnormal_save_number];
        pre_abnormal_coord.erase(pre_abnormal_coord.begin());
        pre_abnormal_coord.push_back({ 0, 0, 0, 0 });
        abnormal_number_count.erase(abnormal_number_count.begin());
        abnormal_number_count.push_back(0);
        new_pre_abnormal_coord_list[abnormal_save_number] = pre_abnormal_coord;
        new_abnormal_number_count_list[abnormal_save_number] = abnormal_number_count;
    }

    return std::make_tuple(new_pre_abnormal_coord_list, new_abnormal_number_count_list);
}


pair<Mat, float> preprocess_image(Mat image, int image_size) {
    int image_height = image.rows;
    int image_width = image.cols;
    float scale;
    int resized_height, resized_width;

    if (image_height > image_width) {
        scale = static_cast<float>(image_size) / image_height;
        resized_height = image_size;
        resized_width = static_cast<int>(image_width * scale);
    }
    else {
        scale = static_cast<float>(image_size) / image_width;
        resized_height = static_cast<int>(image_height * scale);
        resized_width = image_size;
    }

    resize(image, image, Size(resized_width, resized_height));
    image.convertTo(image, CV_32F, 1.0 / 255.0);

    vector<float> mean = { 0.485, 0.456, 0.406 };
    vector<float> std = { 0.229, 0.224, 0.225 };

    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                image.at<Vec3f>(i, j)[c] = (image.at<Vec3f>(i, j)[c] - mean[c]) / std[c];
            }
        }
    }

    int pad_h = image_size - resized_height;
    int pad_w = image_size - resized_width;
    copyMakeBorder(image, image, 0, pad_h, 0, pad_w, BORDER_CONSTANT, Scalar(0, 0, 0));

    return make_pair(image, scale);
}

Mat postprocess_image(Mat image, int raw_height, int raw_width, float scale) {
    int re_image_size = image.rows;
    int resized_height, resized_width;

    if (raw_height > raw_width) {
        resized_height = re_image_size;
        resized_width = static_cast<int>(raw_width * scale);
    }
    else {
        resized_height = static_cast<int>(raw_height * scale);
        resized_width = re_image_size;
    }

    int crop_h = re_image_size - resized_height;
    int crop_w = re_image_size - resized_width;

    if (crop_h == 0) {
        image = image(Rect(0, 0, image.cols - crop_w, image.rows));
    }
    if (crop_w == 0) {
        image = image(Rect(0, 0, image.cols, image.rows - crop_h));
    }

    resize(image, image, Size(raw_width, raw_height));

    return image;
}

vector<vector<float>> postprocess_boxes(vector<vector<float>> boxes, float scale, int height, int width) {
    for (auto& box : boxes) {
        for (auto& coord : box) {
            coord /= scale;
        }
        box[0] = min(max(box[0], 0.0f), static_cast<float>(width - 1));
        box[1] = min(max(box[1], 0.0f), static_cast<float>(height - 1));
        box[2] = min(max(box[2], 0.0f), static_cast<float>(width - 1));
        box[3] = min(max(box[3], 0.0f), static_cast<float>(height - 1));
    }
    return boxes;
}

void draw_box(Mat image, vector<vector<float>> result_output, int frame_number, string patient_number, string screening_start_time) {
    for (const auto& r_o : result_output) {
        int xmin = static_cast<int>(r_o[0]);
        int ymin = static_cast<int>(r_o[1]);
        int xmax = static_cast<int>(r_o[2]);
        int ymax = static_cast<int>(r_o[3]);
        rectangle(image, Point(xmin, ymin), Point(xmax, ymax), Scalar(0, 255, 0), 2);
    }

    string dir_path = "./snapshot_result/" + patient_number + "_" + screening_start_time;
    if (!fs::exists(dir_path)) {
        fs::create_directories(dir_path);
    }

    //if(!std::filesystem::exists)

    string file_path = dir_path + "/" + patient_number + "_" + screening_start_time + "_" + to_string(frame_number) + ".jpg";
    imwrite(file_path, image);
}


