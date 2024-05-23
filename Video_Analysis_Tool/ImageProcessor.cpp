#include "ImageProcessor.h"

ImageProcessor::ImageProcessor(QObject* parent):QObject(parent) {

}
ImageProcessor::~ImageProcessor() {

}

QImage ImageProcessor::get_frame(cv::Mat frame, cv::Rect cvROI, QSize lbl_size, Qt::AspectRatioMode aspect, Qt::TransformationMode smooth) {
	cv::Mat roi_frame = frame(cvROI).clone();
	cv::cvtColor(roi_frame, roi_frame, cv::COLOR_BGR2RGB);
	QImage img((const unsigned char*)(roi_frame.data), roi_frame.cols, roi_frame.rows, QImage::Format_RGB888);
	img = img.scaled(lbl_size, aspect, smooth);
	return img;

}