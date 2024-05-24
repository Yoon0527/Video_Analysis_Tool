#include "inference.h"
#include "utils.h"
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

std::vector<cv::Vec4f> Inference::convertToVec4f(const std::vector<float>& detection_boxes) {
    std::vector<cv::Vec4f> vec4f_boxes;
    vec4f_boxes.reserve(detection_boxes.size() / 4);
    for (size_t i = 0; i < detection_boxes.size(); i += 4) {
        vec4f_boxes.emplace_back(detection_boxes[i], detection_boxes[i + 1], detection_boxes[i + 2], detection_boxes[i + 3]);
    }
    return vec4f_boxes;
}

std::vector<std::vector<int>> Inference::inference_frame(cv::Mat frame, int sensitivity, int frame_number, int patient_number) {
    // Sensitivity에 따라 score_threshold 조정
    double score_threshold = sensitivity / 10.0;

    // Frame 전처리
    float scale;
    cv::Mat preprocess_frame = preprocess_image(frame, 256, scale);

    // ONNX Runtime 입력 준비
    std::vector<const char*> input_node_names_detection = { ortSessionDetection->GetInputName(0, Ort::AllocatorWithDefaultOptions()) };
    std::vector<const char*> input_node_names_classification = { ortSessionClassification->GetInputName(0, Ort::AllocatorWithDefaultOptions()) };

    std::vector<int64_t> input_shape = { 1, preprocess_frame.channels(), preprocess_frame.rows, preprocess_frame.cols };
    Ort::Value input_tensor_detection = Ort::Value::CreateTensor<float>(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault), preprocess_frame.ptr<float>(), preprocess_frame.total(), input_shape.data(), input_shape.size());
    Ort::Value input_tensor_classification = Ort::Value::CreateTensor<float>(Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault), preprocess_frame.ptr<float>(), preprocess_frame.total(), input_shape.data(), input_shape.size());

    // Get output names for detection and classification sessions
    char* output_name_detection = ortSessionDetection->GetOutputName(0, Ort::AllocatorWithDefaultOptions());
    char* output_name_classification = ortSessionClassification->GetOutputName(0, Ort::AllocatorWithDefaultOptions());

    // 추론 실행
    auto output_tensors_detection = ortSessionDetection->Run(Ort::RunOptions{ nullptr }, input_node_names_detection.data(), &input_tensor_detection, 1, &output_name_detection, 1);
    auto output_tensors_classification = ortSessionClassification->Run(Ort::RunOptions{ nullptr }, input_node_names_classification.data(), &input_tensor_classification, 1, &output_name_classification, 1);

    // Detection 결과 후처리
    std::vector<float> detection_boxes(output_tensors_detection[0].GetTensorData<float>(), output_tensors_detection[0].GetTensorData<float>() + output_tensors_detection[0].GetTensorTypeAndShapeInfo().GetElementCount());
    std::vector<float> detection_scores(output_tensors_detection[1].GetTensorData<float>(), output_tensors_detection[1].GetTensorData<float>() + output_tensors_detection[1].GetTensorTypeAndShapeInfo().GetElementCount());

    // Convert detection_boxes to std::vector<cv::Vec4f>
    std::vector<cv::Vec4f> detection_boxes_vec4f = convertToVec4f(detection_boxes);

    // Bounding box 변환
    std::vector<cv::Rect> boxes = bbox_transform_inv(anchors_input, detection_boxes_vec4f);

    // NMS 적용
    std::vector<int> nms_indices = nms_boxes(boxes, detection_scores, 0.5, 200);
    std::vector<cv::Rect> final_boxes;
    for (int idx : nms_indices) {
        if (detection_scores[idx] > score_threshold) {
            final_boxes.push_back(boxes[idx]);
        }
    }

    // 원본 이미지 크기에 맞게 후처리
    std::vector<cv::Rect> processed_boxes = postprocess_boxes(final_boxes, scale, frame.rows, frame.cols);

    // 결과 저장을 위한 초기화
    std::vector<std::vector<int>> result_output;

    // 결과 처리 및 저장
    for (const auto& box : processed_boxes) {
        std::vector<int> result{ frame_number, box.x, box.y, box.width, box.height, 0 }; // 마지막 0은 예시로 frame_location_number 입니다.
        result_output.push_back(result);
    }

    // Free the output name memory
    Ort::AllocatorWithDefaultOptions().Free(output_name_detection);
    Ort::AllocatorWithDefaultOptions().Free(output_name_classification);

    return result_output;

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