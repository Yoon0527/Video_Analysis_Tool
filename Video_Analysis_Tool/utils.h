#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <fstream>
#include <sstream>
#include <algorithm>

std::vector<int> nms_boxes(const std::vector<cv::Rect>& boxes, const std::vector<float>& box_confidences, float nms_threshold, int max_detection_number);
std::vector<cv::Rect> bbox_transform_inv(const std::vector<cv::Rect>& boxes, const std::vector<cv::Vec4f>& deltas);
std::vector<int> pre_coord(const std::vector<std::vector<float>>& pre_abnormal_coord, const std::vector<std::vector<float>>& boxes);
cv::Rect txt_boxes(const std::vector<cv::Rect>& boxes, const std::vector<std::vector<int>>& pre_abnormal_coord_list);
std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<int>>, std::vector<cv::Rect>> pre_coord_explore(std::vector<std::vector<int>>& pre_abnormal_coord_list, std::vector<std::vector<int>>& abnormal_number_count_list, std::vector<cv::Rect>& boxes, int abnormal_count);
std::tuple<std::vector<std::vector<int>>, std::vector<std::vector<int>>> pre_coord_zero_append(std::vector<std::vector<int>>& pre_abnormal_coord_list, std::vector<std::vector<int>>& abnormal_number_count_list);
cv::Mat preprocess_image(const cv::Mat& image, int image_size, float& scale);
cv::Mat postprocess_image(const cv::Mat& image, int raw_height, int raw_width, float scale);
std::vector<cv::Rect> postprocess_boxes(std::vector<cv::Rect>& boxes, float scale, int height, int width);
void draw_box(cv::Mat& image, const std::vector<cv::Rect>& result_output, int frame_number, const std::string& patient_number, const std::string& screening_start_time);
float expit(float x);