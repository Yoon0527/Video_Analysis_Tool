#include "Video_Analysis_Tool.h"
#include <QFileDialog>
#include <opencv2/opencv.hpp>

Video_Analysis_Tool::Video_Analysis_Tool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    init_ui();

    connect(ui.btn_load, SIGNAL(clicked()), this, SLOT(load_media()));
}

Video_Analysis_Tool::~Video_Analysis_Tool()
{}

void Video_Analysis_Tool::init_ui() {
    ui.btn_play_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui.btn_next->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui.btn_prev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui.btn_skip->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    ui.btn_skip_before->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
}

void Video_Analysis_Tool::load_media() {
    QFileDialog dlg;
    file_list = dlg.getOpenFileNames(this, "Load Media", "", "Media Files (*.avi *.mp4 *.mkv)");

    if (!file_list.isEmpty()) {
        current_path = file_list[0];
        set_video(current_path);
    }
}

void Video_Analysis_Tool::set_video(QString file_path) {
    cap.open(file_path.toStdString());
    if (cap.isOpened()) {
        play_status = true;

        v_fps = cap.get(cv::CAP_PROP_FPS);
        v_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        v_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

        connect(&timer, &QTimer::timeout, this, &Video_Analysis_Tool::show_media);
        timer.start(1000 / v_fps);
    }
    else {
        qWarning("Error: Could not open video file.");
    }
}

void Video_Analysis_Tool::show_media() {
    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) {
        timer.stop();
        return;
    }

    // Convert BGR to RGB
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    // Convert frame to QImage
    QImage img((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);

    // Scale image to fit QLabel
    img = img.scaled(ui.lbl_frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Display the QImage in QLabel
    ui.lbl_frame->setPixmap(QPixmap::fromImage(img));
}