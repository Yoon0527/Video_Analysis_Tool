// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <opencv2/opencv.hpp>

std::vector<int> nms_boxes(const std::vector<cv::Rect>& boxes, const std::vector<float>& box_confidences, float nms_threshold, int max_detection_number);
std::vector<cv::Rect> bbox_transform_inv(const std::vector<cv::Rect>& boxes, const std::vector<cv::Vec4f>& deltas);
std::vector<cv::Rect> pre_coord(const std::vector<cv::Rect>& pre_abnormal_coord, const std::vector<cv::Rect>& boxes, int box_number_save);
cv::Mat preprocess_image(cv::Mat image, int image_size, float& scale);
cv::Mat postprocess_image(cv::Mat image, int raw_height, int raw_width, float scale);
std::vector<cv::Rect> postprocess_boxes(const std::vector<cv::Rect>& boxes, float scale, int height, int width);
void draw_box(cv::Mat& image, const std::vector<cv::Rect>& result_output, int frame_number, const std::string& patient_number, const std::string& screening_start_time);

#endif // UTILS_H
