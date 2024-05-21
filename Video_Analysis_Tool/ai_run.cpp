#include "utils.h"
#include "decoding.h"
#include "ai_run.h"
#include <filesystem>
#include <fstream>

ai_run::ai_run() {
	
	//detection_model
}

ai_run::~ai_run() {

}

std::vector<unsigned char> ai_run::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("파일을 열 수 없습니다: " + filePath);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data(), size))) {
        throw std::runtime_error("파일을 읽는 데 실패했습니다: " + filePath);
    }
    return buffer;

}

Ort::Session ai_run::load_model(const std::string& filePath, Ort::Session& session) {
    AESCipher aes(key);
    std::vector<unsigned char> encryptedModel = readFile(filePath);
    std::string decryptedModel = aes.decrypt(encryptedModel);

    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ModelLoader");
    Ort::SessionOptions sessionOptions;
    return Ort::Session(env, decryptedModel.data(), decryptedModel.size(), sessionOptions);

    //return session;
}


std::vector<std::vector<int>> ai_run::AI_run(cv::Mat frame, std::vector<int> crop_coord, int score_threshold, int frame_number, int patient_number, std::string screening_start_time) {

}