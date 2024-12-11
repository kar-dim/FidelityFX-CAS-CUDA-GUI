#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <CASLibWrapper.h>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openImage();
    void saveImage();
    void sliderValueChanged();

private:
    void setupMenu();
    void setupUI();
    void setupSlider(QSlider *slider, QLabel *label, const int value);
    void updateSliderValue(QSlider *slider, QLabel *valueLabel);
    void setupImageView();
    void setupMainWidget();
    void addSliderLayout(QVBoxLayout *mainLayout, QSlider *slider, QLabel *label);

    QImage image;
    QSlider *sharpenStrength, *contrastAdaption;
    QLabel *imageView, *sharpenStrengthLabel, *contrastAdaptionLabel;
    void* casObj;
    QAction *openImageAction, *saveImageAction;
    const QSize targetImageSize;
};

#endif // MAINWINDOW_H
