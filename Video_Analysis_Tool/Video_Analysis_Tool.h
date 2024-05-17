#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Video_Analysis_Tool.h"
#include <QTimer>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

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
    cv::VideoCapture cap;

    float v_fps;
    int v_width;
    int v_height;
    bool play_status = false;
    
    void init_ui();
    void set_video(QString file_path);

private slots:
    void load_media();
    void show_media();
    void stop_media();
};
