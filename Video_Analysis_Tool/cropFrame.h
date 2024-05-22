#pragma once
#include <QDialog>
#include <opencv2/opencv.hpp>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>

class CropFrame : public QDialog
{
    Q_OBJECT

public:
    explicit CropFrame(cv::Mat frame, QRect initialROI, QWidget* parent = nullptr);
    QRect getSelectedROI() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private slots:
    void onOkClicked();
    void onCancelClicked();

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);


private:
private:
    cv::Mat frame;
    QImage qimg;
    QPoint startPoint;
    QPoint endPoint;
    QRect selectedROI;
    QSize originalFrameSize;
    bool selecting;
    bool dragging;
    bool resizing;

    enum Edge { None, Left, Right, Top, Bottom } edge;
    const int edgeMargin = 10;

    Edge getEdgeUnderPoint(const QPoint& point);

    QPushButton* okButton;
    QPushButton* cancelButton;



};