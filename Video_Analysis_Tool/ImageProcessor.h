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
    Inference* Inference;

public slots:
    void process_frame(const cv::Mat& frame, const cv::Rect& roi, const QSize& size, Qt::AspectRatioMode aspectRatioMode, Qt::TransformationMode transformationMode);

signals:
    void frame_processed(const QImage& result);
};