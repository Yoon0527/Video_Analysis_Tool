#include "ImageProcessor.h"

void ImageProcessor::process_frame(const cv::Mat& frame, const cv::Rect& roi, const QSize& size, Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode) {
    cv::Mat roi_frame = frame(roi).clone();
    cv::cvtColor(roi_frame, roi_frame, cv::COLOR_BGR2RGB);
    QImage img((const unsigned char*)(roi_frame.data), roi_frame.cols, roi_frame.rows, QImage::Format_RGB888);
    QImage result = img.scaled(size, aspectRatioMode, transformationMode);
    emit frame_processed(result);
}