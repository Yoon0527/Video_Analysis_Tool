#include <QObject>
#include <QImage>
#include <opencv2/opencv.hpp>

class ImageProcessor :public QObject
{
	Q_OBJECT

public:
	explicit ImageProcessor(QObject* parent = nullptr);
	virtual ~ImageProcessor();

	Q_INVOKABLE QImage get_frame(cv::Mat frame, cv::Rect cvROI, QSize lbl_size, Qt::AspectRatioMode aspect, Qt::TransformationMode smooth);
};