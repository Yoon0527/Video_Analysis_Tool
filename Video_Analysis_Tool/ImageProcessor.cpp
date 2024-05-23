#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(QObject* parent):QObject(parent) {

}
ImageProcessor::~ImageProcessor() {

}

QImage ImageProcessor::get_frame(cv::Mat frame, cv::Rect cvROI, QSize lbl_size, Qt::AspectRatioMode aspect, Qt::TransformationMode smooth) {
	cv::Mat roi_frame = frame(cvROI).clone();
    cv::cvtColor(roi_frame, roi_frame, cv::COLOR_BGR2RGB);

    // QImage 생성자에 복사본을 만듭니다.
    QImage img(roi_frame.data, roi_frame.cols, roi_frame.rows, roi_frame.step, QImage::Format_RGB888);

    // QImage가 데이터의 소유권을 갖도록 합니다.
    QImage img_copy = img.copy();

    // 필요한 경우 크기를 조정합니다.
    img_copy = img_copy.scaled(lbl_size, aspect, smooth);

    return img_copy;

}