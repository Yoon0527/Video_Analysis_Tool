#include "cropFrame.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>

CropFrame::CropFrame(cv::Mat frame, QRect initialROI, QWidget* parent)
    : QDialog(parent), frame(frame), selectedROI(initialROI), selecting(false), dragging(false), resizing(false)
{
    // Check if the frame is valid
    if (frame.empty()) {
        qDebug() << "Empty frame!";
        return;
    }
    originalFrameSize = QSize(frame.cols, frame.rows);
    // Calculate the maximum width and height for the dialog based on the frame size
    const int maxWidth = 1200; // Set maximum width
    const int maxHeight = maxWidth * frame.rows / frame.cols; // Calculate corresponding height to maintain aspect ratio

    // Convert the frame to QImage
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    QImage qimg(reinterpret_cast<uchar*>(rgbFrame.data), rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);

    // Check if QImage creation is successful
    if (qimg.isNull()) {
        qDebug() << "Failed to convert frame to QImage!";
        return;
    }

    // Set the size of the dialog based on the calculated maximum width and height
    setFixedSize(maxWidth, maxHeight);

    // Set initial ROI to cover the entire frame
    //selectedROI = QRect(0, 0, frame.cols-150, frame.rows-150);
    selectedROI = QRect(0, 0, maxWidth, maxHeight);
    // QPoint selectionOrigin = QPoint(0, 0);
     // Create OK and Cancel buttons
    okButton = new QPushButton("OK", this);
    cancelButton = new QPushButton("Cancel", this);

    // Connect buttons to slots
    connect(okButton, &QPushButton::clicked, this, &CropFrame::onOkClicked);
    connect(cancelButton, &QPushButton::clicked, this, &CropFrame::onCancelClicked);

    // Layout for buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

QRect CropFrame::getSelectedROI() const
{
    // Adjust ROI to the original frame size
    QRect adjustedROI;
    adjustedROI.setX(selectedROI.x() * originalFrameSize.width() / width());
    adjustedROI.setY(selectedROI.y() * originalFrameSize.height() / height());
    adjustedROI.setWidth(selectedROI.width() * originalFrameSize.width() / width());
    adjustedROI.setHeight(selectedROI.height() * originalFrameSize.height() / height());
    return adjustedROI;
}

//void CropFrame::paintEvent(QPaintEvent* event)
//{
//    QDialog::paintEvent(event);
//    QPainter painter(this);
//
//    // Calculate the maximum width and height for the frame while preserving aspect ratio
//    const double frameAspectRatio = static_cast<double>(frame.cols) / frame.rows;
//    const int maxWidth = width(); // Set maximum width to the width of the dialog
//    const int maxHeight = maxWidth / frameAspectRatio;
//
//    // Resize the frame while preserving aspect ratio
//    cv::Mat resizedFrame;
//    cv::resize(frame, resizedFrame, cv::Size(maxWidth, maxHeight), 0, 0, cv::INTER_AREA);
//
//    // Convert the resized frame to QImage
//    cv::Mat rgbFrame;
//    cv::cvtColor(resizedFrame, rgbFrame, cv::COLOR_BGR2RGB);
//    QImage qimg(reinterpret_cast<uchar*>(rgbFrame.data), rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);
//
//    // Check if QImage creation is successful
//    if (qimg.isNull()) {
//        qDebug() << "Failed to convert frame to QImage!";
//        return;
//    }
//
//    // Draw the QImage
//    painter.drawImage(0, 0, qimg);
//
//    // Calculate the scaled ROI
//    QRect scaledROI;
//    scaledROI.setX(selectedROI.x() * maxWidth / frame.cols);
//    scaledROI.setY(selectedROI.y() * maxHeight / frame.rows);
//    scaledROI.setWidth(selectedROI.width() * maxWidth / frame.cols);
//    scaledROI.setHeight(selectedROI.height() * maxHeight / frame.rows);
//
//    painter.setPen(QPen(Qt::red, 2));
//    painter.drawRect(scaledROI);
//}

void CropFrame::onOkClicked()
{
    accept(); // This will close the dialog with QDialog::Accepted
}

void CropFrame::onCancelClicked()
{
    reject(); // This will close the dialog with QDialog::Rejected
}

void CropFrame::paintEvent(QPaintEvent* event)
{
    QDialog::paintEvent(event);
    QPainter painter(this);

    const double frameAspectRatio = static_cast<double>(frame.cols) / frame.rows;
    const int maxWidth = width(); // Set maximum width to the width of the dialog
    const int maxHeight = maxWidth / frameAspectRatio;
    cv::Mat resizedFrame;
    cv::resize(frame, resizedFrame, cv::Size(maxWidth, maxHeight), 0, 0, cv::INTER_AREA);

    cv::Mat rgbFrame;
    cv::cvtColor(resizedFrame, rgbFrame, cv::COLOR_BGR2RGB);
    QImage qimg(reinterpret_cast<uchar*>(rgbFrame.data), rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);

    painter.drawImage(0, 0, qimg);

    painter.setPen(QPen(Qt::red, 2));
    painter.drawRect(selectedROI);
    qDebug() << selectedROI;
}

void CropFrame::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        Edge edge = getEdgeUnderPoint(event->position().toPoint());
        if (edge != None) {
            resizing = true;
            this->edge = edge;
        }
        else if (selectedROI.contains(event->position().toPoint())) {
            dragging = true;
            startPoint = event->position().toPoint();
        }
        else {
            selecting = true;
            startPoint = event->position().toPoint();
            endPoint = startPoint;
        }
    }
}

void CropFrame::mouseMoveEvent(QMouseEvent* event)
{
    if (selecting) {
        endPoint = event->position().toPoint();
        selectedROI = QRect(startPoint, endPoint).normalized();
        update();
    }
    else if (dragging) {
        QPoint delta = event->position().toPoint() - startPoint;
        selectedROI.translate(delta);
        startPoint = event->position().toPoint();
        update();
    }
    else if (resizing) {
        switch (edge) {
        case Left:
            selectedROI.setLeft(event->position().toPoint().x());
            break;
        case Right:
            selectedROI.setRight(event->position().toPoint().x());
            break;
        case Top:
            selectedROI.setTop(event->position().toPoint().y());
            break;
        case Bottom:
            selectedROI.setBottom(event->position().toPoint().y());
            break;
        default:
            break;
        }
        update();
    }
}

void CropFrame::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        selecting = false;
        dragging = false;
        resizing = false;
    }
}

CropFrame::Edge CropFrame::getEdgeUnderPoint(const QPoint& point)
{
    if (abs(point.x() - selectedROI.left()) < edgeMargin)
        return Left;
    if (abs(point.x() - selectedROI.right()) < edgeMargin)
        return Right;
    if (abs(point.y() - selectedROI.top()) < edgeMargin)
        return Top;
    if (abs(point.y() - selectedROI.bottom()) < edgeMargin)
        return Bottom;
    return None;
}