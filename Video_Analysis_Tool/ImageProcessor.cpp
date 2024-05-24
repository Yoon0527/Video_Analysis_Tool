#include "ImageProcessor.h"

ImageProcessor::ImageProcessor() : ai_model(new Inference()) {}

ImageProcessor::~ImageProcessor() {
    delete ai_model; // 메모리 누수 방지를 위해 ai_model 삭제
}

void ImageProcessor::process_frame(const cv::Mat& frame, const cv::Rect& roi, const QSize& size, Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode) {
    cv::Mat roi_frame = frame(roi).clone();
    cv::cvtColor(roi_frame, roi_frame, cv::COLOR_BGR2RGB);
    QImage img((const unsigned char*)(roi_frame.data), roi_frame.cols, roi_frame.rows, QImage::Format_RGB888);
    QImage result = img.scaled(size, aspectRatioMode, transformationMode);
    emit frame_processed(result);
}

void ImageProcessor::process_frame_ai(const cv::Mat& frame, const cv::Rect& roi, const QSize& size, Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode, int sensitivity, int frame_number, int patient_number) {
    // Extract the region of interest (ROI) from the frame
    cv::Mat roi_frame = frame(roi).clone();

    // Convert BGR to RGB
    cv::cvtColor(roi_frame, roi_frame, cv::COLOR_BGR2RGB);

    // Perform inference on the ROI using the AI model
    std::vector<std::vector<int>> result_output = ai_model->inference_frame(roi_frame, sensitivity, frame_number, patient_number);

    // Process the result output and draw bounding boxes on the ROI
    for (const auto& result : result_output) {
        int x = result[1];
        int y = result[2];
        int width = result[3];
        int height = result[4];
        cv::rectangle(roi_frame, cv::Rect(x, y, width, height), cv::Scalar(0, 255, 0), 2);
    }

    // Convert the processed ROI frame to QImage
    QImage img((const unsigned char*)(roi_frame.data), roi_frame.cols, roi_frame.rows, QImage::Format_RGB888);

    // Scale the QImage
    QImage result = img.scaled(size, aspectRatioMode, transformationMode);

    // Emit the processed frame
    emit frame_processed(result);
}