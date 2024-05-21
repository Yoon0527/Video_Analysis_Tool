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

        std::cout << "�� �ε� �� ��ȣȭ ����" << std::endl;
        //inference_frame(modelLoader.loadModel("./model/detection.bin"), modelLoader.loadModel("./model/classification.bin"));
    }
    catch (const std::exception& e) {
        std::cerr << "����: " << e.what() << std::endl;
    }
}

void Inference::inference_frame() {
    
}