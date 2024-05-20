#include "ai_run.h"
#include <fstream>
#include <iostream>

// Constructor to initialize the ai_run object
ai_run::ai_run()
    : key("CAIMI Alphaon V1.0 Model"),
    abnormal_count(3),
    max_detection_number(3),
    init_detection(0),
    continue_detection(0),
    frame_location_number(0),
    env(ORT_LOGGING_LEVEL_WARNING, "ai_run"),
    session_options() {

    // Read and decrypt the detection model
    std::vector<unsigned char> detection_data = read_binary_file("./model/detection.bin");
    AESCipher aes_detection(key);
    std::vector<unsigned char> detection_model = aes_detection.decrypt(detection_data);

    ort_session_detection = new Ort::Session(env, detection_model.data(), detection_model.size(), session_options);

    // Read and decrypt the classification model
    std::vector<unsigned char> classification_data = read_binary_file("./model/classification.bin");
    AESCipher aes_classification(key);
    std::vector<unsigned char> classification_model = aes_classification.decrypt(classification_data);

    ort_session_classification = new Ort::Session(env, classification_model.data(), classification_model.size(), session_options);

    // Initialize class list and other parameters
    class_list = { "fundus", "etc", "instruments", "NBI", "close", "nonendo", "bleeding", "esophagus", "squamocolumnar_junction", "middle_upper_body", "lower_body", "antrum", "duodenal_bulb", "duodenal_descending", "angulus", "middle_upper_body_retro" };
    location_spot_check_list = { "", "esophagus", "middle_upper_body", "lower_body", "antrum", "angulus", "middle_upper_body_retro", "fundus", "squamocolumnar_junction", "duodenal_bulb", "duodenal_descending" };
    score_threshold_list = { 0, 0.45, 0.42, 0.39, 0.35, 0.32, 0.28, 0.24, 0.21, 0.17, 0.13 };

    // Load anchors input
    anchors_input = {}; // Add code to load anchors_input from file

    abnormal_number_count_list = std::vector<std::vector<int>>(3, std::vector<int>(10, 0));
    pre_abnormal_coord_list = std::vector<std::vector<std::vector<int>>>(3, std::vector<std::vector<int>>(10, std::vector<int>(4, 0)));
    location_spot_count = std::vector<std::string>(20, "");
}

// Destructor to clean up dynamically allocated memory
ai_run::~ai_run() {
    delete ort_session_detection;
    delete ort_session_classification;
}

// Function to read a binary file into a vector
std::vector<unsigned char> ai_run::read_binary_file(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    return std::vector<unsigned char>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}