#pragma once
#include <string>
#include <vector>
#include <onnxruntime_cxx_api.h>
#include "decoding.h"

class ai_run {
public:
    ai_run();
    ~ai_run(); // Destructor to clean up dynamically allocated memory

private:
    std::string key;
    std::vector<unsigned char> read_binary_file(const std::string& filepath);
    Ort::Session* ort_session_detection; // Pointer to Ort::Session
    Ort::Session* ort_session_classification; // Pointer to Ort::Session
    Ort::Env env;
    Ort::SessionOptions session_options;
    std::vector<std::string> class_list;
    std::vector<std::string> location_spot_check_list;
    std::vector<double> score_threshold_list;
    std::vector<float> anchors_input;
    int abnormal_count;
    int max_detection_number;
    std::vector<std::vector<int>> abnormal_number_count_list;
    std::vector<std::vector<std::vector<int>>> pre_abnormal_coord_list;
    std::vector<std::string> location_spot_count;
    int init_detection;
    int continue_detection;
    int frame_location_number;

};