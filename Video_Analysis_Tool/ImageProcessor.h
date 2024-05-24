#pragma once
#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>
#include "inference.h"

class ImageProcessor : public QObject {
    Q_OBJECT

public:
    ImageProcessor() {}
    ~ImageProcessor() {}

private:
    Inference* ai_model;

public slots:
    void process_frame(const cv::Mat& frame, const cv::Rect& roi, const QSize& size, Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode);
    void process_frame_ai(const cv::Mat& frame, const cv::Rect& roi, const QSize& size, Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode, int sensitivity, int frame_number, int patient_number);

signals:
    void frame_processed(const QImage& result);
};