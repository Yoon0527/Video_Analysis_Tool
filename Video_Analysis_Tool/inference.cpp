#include "inference.h"
#include <iostream>

void inference_frame() {
    try {
        std::string key = "CAIMI Alphaon V1.0 Model";
        ModelLoader modelLoader(key);

        // Load and decrypt detection model
        Ort::Session ortSessionDetection = modelLoader.loadModel("./model/detection.bin");

        // Load and decrypt classification model
        Ort::Session ortSessionClassification = modelLoader.loadModel("./model/classification.bin");

        std::cout << "�� �ε� �� ��ȣȭ ����" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "����: " << e.what() << std::endl;
    }
}