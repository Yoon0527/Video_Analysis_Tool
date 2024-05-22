#include "inference.h"
#include <iostream>


Inference::Inference() {
    try {
        std::string key = "CAIMI Alphaon V1.0 Model";
        ModelLoader modelLoader(key);

        // Load and decrypt detection model
        ortSessionDetection = std::make_unique<Ort::Session>(modelLoader.loadModel("./model/detection.bin"));

        // Load and decrypt classification model
        ortSessionClassification = std::make_unique<Ort::Session>(modelLoader.loadModel("./model/classification.bin"));

        std::cout << "모델 로드 및 복호화 성공" << std::endl;
        //inference_frame(modelLoader.loadModel("./model/detection.bin"), modelLoader.loadModel("./model/classification.bin"));
    }
    catch (const std::exception& e) {
        std::cerr << "오류: " << e.what() << std::endl;
    }
}

Inference::~Inference() {

}

void Inference::inference_frame(cv::Mat frame, int sensitivity, int frame_number, int patient_number) {
    double score_threshold = 0.6;
    std::vector<int> result_output;

}