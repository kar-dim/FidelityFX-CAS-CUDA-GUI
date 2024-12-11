#include "mainwindow.h"
#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QScreen>
#include <QPixmap>
#include <QImageReader>
#include <CASLibWrapper.h>
#include "widget_utils.hpp"

#define CLAMP(x) qBound(0.0f, x/100.0f, 1.0f)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    imageView(new QLabel),
    sharpenStrength(new QSlider(Qt::Horizontal)),
    contrastAdaption(new QSlider(Qt::Horizontal)),
    sharpenStrengthLabel(new QLabel("Sharpen Strength")),
    contrastAdaptionLabel(new QLabel("Contrast Adaption")),
    casObj(CAS_initialize(5,5)),
    // 80% of the screen size
    targetImageSize(QGuiApplication::primaryScreen()->availableGeometry().size() * 0.8)
{
    //setup sliders
    setupSlider(sharpenStrength, sharpenStrengthLabel, 0);
    setupSlider(contrastAdaption, contrastAdaptionLabel, 100);
    //setup file menu
    setupMenu();
    //setup main image view
    setupImageView();
    //setup main widget
    setupMainWidget();
}

MainWindow::~MainWindow()
{
    CAS_destroy(casObj);
}

void MainWindow::setupSlider(QSlider *slider, QLabel *label, const int value)
{
    //Setup slider properties (QSlider and correspinding QLabel)
    slider->setRange(0, 100);
    slider->setValue(value);
    label->setFixedWidth(130);
    WidgetUtils::setVisibility(false, slider, label);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
}

void MainWindow::setupMenu()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("File");
    openImageAction = fileMenu->addAction("Open Image");
    saveImageAction = fileMenu->addAction("Save Image");
    saveImageAction->setEnabled(false);
    connect(openImageAction, &QAction::triggered, this, &MainWindow::openImage);
}

void MainWindow::setupImageView()
{
    imageView->setAlignment(Qt::AlignCenter);
    imageView->setVisible(false);
}

// Main Widget initialize
void MainWindow::setupMainWidget()
{
    // Main Layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    addSliderLayout(mainLayout, sharpenStrength, sharpenStrengthLabel);
    addSliderLayout(mainLayout, contrastAdaption, contrastAdaptionLabel);
    mainLayout->addWidget(imageView);
    // Central Widget
    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);
}

//add a Slider Horizontal layout (Slider and Label) into a Vertical layout
void MainWindow::addSliderLayout(QVBoxLayout *mainLayout, QSlider *slider, QLabel *label)
{
    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(label);
    sliderLayout->addWidget(slider);
    mainLayout->addLayout(sliderLayout);
}

void MainWindow::openImage()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg)");
    if (fileName.isEmpty())
        return;
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    image = reader.read();

    if (!image.isNull())
    {
        if (image.format() != QImage::Format_RGBA8888)
            image = image.convertToFormat(QImage::Format_RGBA8888);

        //reinitialize CAS memory
        CAS_reinitialize(casObj, image.height(), image.width());

        // Only scale down if the image is larger than the target size
        QPixmap pixmap = QPixmap::fromImage(image);
        WidgetUtils::scalePixmap(pixmap, targetImageSize);
        imageView->setPixmap(pixmap);
        WidgetUtils::setVisibility(true, imageView, sharpenStrength, contrastAdaption, sharpenStrengthLabel, contrastAdaptionLabel);
        saveImageAction->setEnabled(true);

        // Resize the window to fit the image and sliders
        adjustSize();

        //reset sliders
        sharpenStrength->setValue(0);
        contrastAdaption->setValue(100);
    }
}


void MainWindow::sliderValueChanged()
{
    //don't calculate it parameters are (very close to) 0
    if (sharpenStrength->value() <= 0.001 || contrastAdaption->value() <= 0.001)
        return;
    //apply CAS sharpening
    const uchar* casData = CAS_sharpenImage(casObj, image.constBits(), 1, CLAMP(sharpenStrength->value()), CLAMP(contrastAdaption->value()));
    //show the new image
    QPixmap pixmap = QPixmap::fromImage(QImage(casData, image.width(), image.height(), QImage::Format_RGB888));
    WidgetUtils::scalePixmap(pixmap, targetImageSize);
    imageView->setPixmap(pixmap);
}
