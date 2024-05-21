#pragma once
#include "ModelLoader.h"
#include <onnxruntime_cxx_api.h>


class Inference {
public:
	Inference();
	~Inference();
private:
	std::unique_ptr<Ort::Session> ortSessionDetection;
	std::unique_ptr<Ort::Session> ortSessionClassification;
	
	void inference_frame();
};
