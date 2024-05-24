#pragma once
#include "ModelLoader.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>

class Inference {
public:
	Inference();
	~Inference();

	std::vector<std::vector<int>> inference_frame(cv::Mat frame, int sensitivity, int frame_number, int patient_number);

private:
	std::unique_ptr<Ort::Session> ortSessionDetection;
	std::unique_ptr<Ort::Session> ortSessionClassification;
	//std::unique_ptr<cv::Mat> anchors_input;
	cv::Mat anchors_input;
	std::vector<std::string> class_list = {
	"fundus", "etc", "instruments", "NBI", "close", "nonendo", "bleeding", "esophagus",
	"squamocolumnar_junction", "middle_upper_body", "lower_body", "antrum",
	"duodenal_bulb", "duodenal_descending", "angulus", "middle_upper_body_retro"
	};

	std::vector<std::string> location_spot_check_list = {
	"", "esophagus", "middle_upper_body", "lower_body", "antrum", "angulus",
	"middle_upper_body_retro", "fundus", "squamocolumnar_junction",
	"duodenal_bulb", "duodenal_descending"
	};

	std::vector<double> score_threshold_list = { 0, 0.45, 0.42, 0.39, 0.35, 0.32, 0.28, 0.24, 0.21, 0.17, 0.13 };
	int abnormal_count = 3; //5
	int max_detection_number = 3;

	std::vector<std::vector<int>> abnormal_number_count_list;
	std::vector<std::vector<std::vector<int>>> pre_abnormal_coord_list;
	std::vector<std::string> location_spot_count;

	// 기타 변수들
	int init_detection = 0;
	int continue_detection = 0;
	int frame_location_number = 0;

	std::vector<cv::Vec4f> convertToVec4f(const std::vector<float>& detection_boxes);

private:
	cv::Mat loadAnchors(const std::string& file_path);
};
