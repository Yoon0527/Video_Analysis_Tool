#pragma once
#include "ModelLoader.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>

class Inference {
public:
	Inference();
	~Inference();

	void inference_frame(cv::Mat frame, int sensitivity, int frame_number, int patient_number);

private:
	std::unique_ptr<Ort::Session> ortSessionDetection;
	std::unique_ptr<Ort::Session> ortSessionClassification;
};
