#include "Video_Analysis_Tool.h"
#include "cropFrame.h"
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <thread>
#include <vector>
#include <QThread>

Video_Analysis_Tool::Video_Analysis_Tool(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    init_ui();

    connect(ui.btn_load, SIGNAL(clicked()), this, SLOT(load_media()));
    connect(ui.btn_play_pause, SIGNAL(clicked()), this, SLOT(stop_media()));
    connect(ui.btn_next, SIGNAL(clicked()), this, SLOT(next_media()));
    connect(ui.btn_prev, SIGNAL(clicked()), this, SLOT(prev_media()));
    connect(ui.slider_length, SIGNAL(sliderMoved(int)), this, SLOT(slider_move(int)));
    connect(ui.btn_skip, SIGNAL(clicked()), this, SLOT(move_forward()));
    connect(ui.btn_skip_before, SIGNAL(clicked()), this, SLOT(move_backward()));
    connect(ui.btn_analysis, SIGNAL(clicked()), this, SLOT(ai_analysis()));
    connect(ui.btn_crop, SIGNAL(clicked()), this, SLOT(crop_frame()));
    connect(ui.btn_close, SIGNAL(clicked()), this, SLOT(exit()));

    //Inference ai_model;

    imageProcessor = new ImageProcessor();
    imageProcessorThread = new QThread(this);

    imageProcessor->moveToThread(imageProcessorThread);
    connect(imageProcessorThread, &QThread::finished, imageProcessor, &QObject::deleteLater);
    connect(this, &Video_Analysis_Tool::process_frame, imageProcessor, &ImageProcessor::process_frame);
    connect(this, &Video_Analysis_Tool::process_frame_ai, imageProcessor, &ImageProcessor::process_frame_ai);
    connect(imageProcessor, &ImageProcessor::frame_processed, this, &Video_Analysis_Tool::on_frame_processed);

    imageProcessorThread->start();

    showFullScreen();
}

Video_Analysis_Tool::~Video_Analysis_Tool()
{
    clean_up();
}

void Video_Analysis_Tool::clean_up() {
    imageProcessorThread->quit();
    imageProcessorThread->wait();
}

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
        media_cnt = 0;
        file_list_len = file_list.size();
    }
}

void Video_Analysis_Tool::set_video(QString file_path) {
    cap.open(file_path.toStdString());
    if (cap.isOpened()) {
        play_status = true;

        v_fps = cap.get(cv::CAP_PROP_FPS);
        v_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        v_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
        total_frame_len = cap.get(cv::CAP_PROP_FRAME_COUNT);

        ui.slider_length->setMaximum(total_frame_len);
        roi = QRect(0, 0, v_width, v_height);
        cvROI = cv::Rect(roi.x(), roi.y(), roi.width(), roi.height());
        connect(&timer, &QTimer::timeout, this, &Video_Analysis_Tool::show_media);
        timer.start(1000 / v_fps);
    }
    else {
        qWarning("Error: Could not open video file.");
    }
}


//void Video_Analysis_Tool::show_media() {
//    cv::Mat frame;
//    cap >> frame;
//    //roi = QRect(798, 104, 1008, 875);
//    if (frame.empty()) {
//        timer.stop();
//        return;
//    }
//    else {
//        if (!roi.isNull()) {
//            // Convert QRect to cv::Rect
//            //cv::Rect cvROI(roi.x(), roi.y(), roi.width(), roi.height());
//            //cv::Rect cvROI(798, 104, 1008, 875);
//            // Ensure the ROI is within the bounds of the frame
//            //cvROI &= cv::Rect(0, 0, frame.cols, frame.rows);
//
//            cv::Mat roi_frame = frame(cvROI).clone();
//
//            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
//            ui.slider_length->setValue(current_frame);
//
//            // Convert BGR to RGB
//            cv::cvtColor(roi_frame, roi_frame, cv::COLOR_BGR2RGB);
//
//            // Convert frame to QImage
//            QImage img((const unsigned char*)(roi_frame.data), roi_frame.cols, roi_frame.rows, QImage::Format_RGB888);
//
//            // Scale image to fit QLabel
//            img = img.scaled(ui.lbl_frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//            qDebug() << Qt::KeepAspectRatio;
//            qDebug() << Qt::SmoothTransformation;
//            // Display the QImage in QLabel
//            ui.lbl_frame->setPixmap(QPixmap::fromImage(img));
//        }
//        else {
//            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
//            ui.slider_length->setValue(current_frame);
//
//            // Convert BGR to RGB
//            cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
//
//            // Convert frame to QImage
//            QImage img((const unsigned char*)(frame.data), frame.cols, frame.rows, QImage::Format_RGB888);
//
//            // Scale image to fit QLabel
//            img = img.scaled(ui.lbl_frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//
//            // Display the QImage in QLabel
//            ui.lbl_frame->setPixmap(QPixmap::fromImage(img));
//        }
//    }
//    play_status = true;
//    ui.btn_play_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
//    ui.slider_length->setEnabled(true);
//}

//void Video_Analysis_Tool::show_media() {
//    ImageProcessor* imageProcessor = new ImageProcessor();
//    QThread* imageProcessorThread = new QThread();
//    imageProcessor->moveToThread(imageProcessorThread);
//    imageProcessorThread->start();
//
//    cv::Mat frame;
//    cap >> frame;
//    //roi = QRect(798, 104, 1008, 875);
//    if (frame.empty()) {
//        timer.stop();
//        return;
//    }
//    else {
//        if (!roi.isNull()) {
//            QImage result;
//            bool success = QMetaObject::invokeMethod(imageProcessor, "get_frame", Qt::DirectConnection, Q_RETURN_ARG(QImage, result),Q_ARG(cv::Mat, frame), Q_ARG(cv::Rect,cvROI), Q_ARG(QSize, ui.lbl_frame->size()), Q_ARG(Qt::AspectRatioMode, Qt::KeepAspectRatio), Q_ARG(Qt::TransformationMode, Qt::SmoothTransformation));
//            
//            // Display the QImage in QLabel
//            ui.lbl_frame->setPixmap(QPixmap::fromImage(return_img));
//            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
//            ui.slider_length->setValue(current_frame);
//        }
//        else {
//            
//            // Display the QImage in QLabel
//            ui.lbl_frame->setPixmap(QPixmap::fromImage(return_img));
//            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
//            ui.slider_length->setValue(current_frame);
//        }
//    }
//    play_status = true;
//    ui.btn_play_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
//    ui.slider_length->setEnabled(true);
//}

void Video_Analysis_Tool::show_media() {
    int sensitivity = 6;
    int patient_num = 1234;

    cv::Mat frame;
    cap >> frame;

    if (frame.empty()) {
        timer.stop();
        return;
    }
    else {
        if (!ai_status) {
            emit process_frame(frame, cvROI, ui.lbl_frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
            ui.slider_length->setValue(current_frame);
        }
        else if (ai_status) {
            emit process_frame_ai(frame, cvROI, ui.lbl_frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation, sensitivity, current_frame, patient_num);
            current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
            ui.slider_length->setValue(current_frame);
        }
        
    }

    play_status = true;
    ui.btn_play_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui.slider_length->setEnabled(true);
}

void Video_Analysis_Tool::on_frame_processed(const QImage& result) {
    ui.lbl_frame->setPixmap(QPixmap::fromImage(result));
}

void Video_Analysis_Tool::stop_media() {
    if (!play_status) {
        timer.start(1000 / v_fps); // 일시정지 해제
        ui.btn_play_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
    else {
        timer.stop(); // 일시정지
        ui.btn_play_pause->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
    play_status = !play_status;
}

void Video_Analysis_Tool::next_media() {
    media_cnt += 1;
    if (media_cnt < file_list_len) {
        QString* first_path(&file_list[0]);
        set_video(*(first_path + media_cnt));
        //ui.sl_imgBrightness->setValue(0);
    }
    else {
        media_cnt = file_list_len;
        QMessageBox::warning(nullptr, "Warning", "This is the last video.");
    }
}

void Video_Analysis_Tool::prev_media() {
    media_cnt -= 1;
    if (media_cnt >= 0) {
        QString* first_path(&file_list[0]);
        set_video(*(first_path + media_cnt));
        //ui.sl_imgBrightness->setValue(0);
    }
    else {
        media_cnt = 0;
        QMessageBox::warning(this, "Warning", "This is the first video.");
    }
}

void Video_Analysis_Tool::slider_move(int position) {
    cap.set(cv::CAP_PROP_POS_FRAMES, position);
    //if (!play_status) {
    //    show_media();
    //}
}

void Video_Analysis_Tool::move_forward() {
    int current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
    int target_frame = current_frame + (10 * v_fps); // 10초 앞으로
    if (target_frame >= total_frame_len) {
        target_frame = total_frame_len - 1;
    }
    cap.set(cv::CAP_PROP_POS_FRAMES, target_frame);
    //if (is_paused) {
    //    show_media(); // 일시정지 상태일 때 프레임을 갱신
    //}
}

void Video_Analysis_Tool::move_backward() {
    int current_frame = cap.get(cv::CAP_PROP_POS_FRAMES);
    int target_frame = current_frame - (10 * v_fps); // 10초 뒤로
    if (target_frame < 0) {
        target_frame = 0;
    }
    cap.set(cv::CAP_PROP_POS_FRAMES, target_frame);
    //if (is_paused) {
    //    show_media(); // 일시정지 상태일 때 프레임을 갱신
    //}
}

void Video_Analysis_Tool::ai_analysis() {
    ai_status = !ai_status;

    if (ai_status) {
        ui.btn_analysis->setText("Ai Analysis Off");
    }
    else if (!ai_status) {
        ui.btn_analysis->setText("Ai Analysis On");
    }
}

//void Video_Analysis_Tool::crop_frame() {
//    cv::Mat frame;
//    roi = QRect(0, 0, frame.cols, frame.rows);
//    if (cap.read(frame)) {
//        CropFrame roiSelector(frame, roi, this);
//        if (roiSelector.exec() == QDialog::Accepted) {
//            roi = roiSelector.getSelectedROI();
//            qDebug() << roi;
//        }
//    }
//}

void Video_Analysis_Tool::crop_frame() {
    cv::Mat frame;
    if (cap.read(frame)) {
        if (play_status) {
            stop_media();
        }
        QRect initialROI(0, 0, frame.cols, frame.rows);
        CropFrame roiSelector(frame, initialROI, this);
        if (roiSelector.exec() == QDialog::Accepted) {
            roi = roiSelector.getSelectedROI();
            cvROI = cv::Rect(roi.x(), roi.y(), roi.width(), roi.height());
            if (!play_status) {
                stop_media();
            }
            qDebug() << roi;
        }
    }
}

void Video_Analysis_Tool::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (cap.isOpened()) {
        // Get the current position in the video
        int currentPos = cap.get(cv::CAP_PROP_POS_FRAMES);
        cap.set(cv::CAP_PROP_POS_FRAMES, currentPos);

        cv::Mat frame;
        cap >> frame;

        // Check if the frame is not empty
        if (!frame.empty()) {
            emit process_frame(frame, cvROI, ui.lbl_frame->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
}

void Video_Analysis_Tool::exit() {
    QApplication::quit();  // 프로그램 종료
}