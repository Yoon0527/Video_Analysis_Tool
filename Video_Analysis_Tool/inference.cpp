#include "inference.h"

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

        //anchors_input = std::make_unique<cv::Mat>(loadAnchors("./anchor/ anchors_input_256_default.npy"));
        anchors_input = loadAnchors("./anchor/ anchors_input_256_default.npy");

        abnormal_number_count_list = std::vector<std::vector<int>>(3, std::vector<int>(10, 0));
        pre_abnormal_coord_list = std::vector<std::vector<std::vector<int>>>(3, std::vector<std::vector<int>>(10, std::vector<int>(4, 0)));
        location_spot_count = std::vector<std::string>(20, "");
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

cv::Mat Inference::loadAnchors(const std::string& file_path) {
    cv::Mat anchors_input;
    std::ifstream file(file_path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    file.read(reinterpret_cast<char*>(anchors_input.data), anchors_input.total() * anchors_input.elemSize());
    file.close();

    return anchors_input;
}