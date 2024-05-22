#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Video_Analysis_Tool.h"
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include "inference.h"

class Video_Analysis_Tool : public QMainWindow
{
    Q_OBJECT

public:
    Video_Analysis_Tool(QWidget *parent = nullptr);
    ~Video_Analysis_Tool();

private:
    Ui::Video_Analysis_ToolClass ui;
    QStringList file_list;
    QString current_path;
    QTimer timer;
    QRect roi;
    
    cv::VideoCapture cap;
    int media_cnt;
    int file_list_len;

    float v_fps;
    int v_width;
    int v_height;
    int total_frame_len;
    int current_frame;
    bool play_status = false;
    bool ai_status = false;
    bool roiSelected = false;
    void init_ui();
    void set_video(QString file_path);

private slots:
    void load_media();
    void show_media();
    void stop_media();
    void next_media();
    void prev_media();
    void slider_move(int position);
    void move_forward();
    void move_backward();
    void ai_analysis();
    void crop_frame();
    void resizeEvent(QResizeEvent* event);
};
