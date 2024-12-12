#include "mainwindow.h"
#include <QMainWindow>
#include <QSlider>
#include <QLabel>
#include <QImage>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QScreen>
#include <QPixmap>
#include <QImageReader>
#include <CASLibWrapper.h>
#include "widget_utils.hpp"

#define CLAMP(x) qBound(0.0f, x/100.0f, 1.0f)

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    sharpenStrength(new QSlider(Qt::Horizontal)),
    contrastAdaption(new QSlider(Qt::Horizontal)),
    imageView(new QLabel),
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

//destroy DLL's memory
MainWindow::~MainWindow()
{
    CAS_destroy(casObj);
}

//setup CAS parameter sliders
void MainWindow::setupSlider(QSlider *slider, QLabel *label, const int value)
{
    //Setup slider properties (QSlider and correspinding QLabel)
    slider->setRange(0, 100);
    slider->setValue(value);
    label->setFixedWidth(130);
    WidgetUtils::setVisibility(false, slider, label);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::sliderValueChanged);
}

//setup "File" menu
void MainWindow::setupMenu()
{
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("File");
    openImageAction = fileMenu->addAction("Open Image");
    saveImageAction = fileMenu->addAction("Save Image");
    saveImageAction->setEnabled(false);
    connect(openImageAction, &QAction::triggered, this, &MainWindow::openImage);
    connect(saveImageAction, &QAction::triggered, this, &MainWindow::saveImage);
}

//setup Main image view
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

//updates the Image label to show the passed-in QImage
void MainWindow::updateImageView(const QImage& image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    WidgetUtils::scalePixmap(pixmap, targetImageSize);
    imageView->setPixmap(pixmap);
}

//Open an image and display it to the user. Reinitialize CAS with the new dimensions and
void MainWindow::openImage()
{
    const QString fileName = QFileDialog::getOpenFileName(this, "Open Image", "", "Images (*.png *.jpg)");
    if (fileName.isEmpty())
        return;
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    originalImage = reader.read();

    if (!originalImage.isNull())
    {
        //convert to RGBA interleaved format
        originalImage = originalImage.convertToFormat(QImage::Format_RGBA8888);
        //reinitialize CAS memory
        CAS_reinitialize(casObj, originalImage.height(), originalImage.width());

        // Only scale down if the image is larger than the target size
        updateImageView(originalImage);
        WidgetUtils::setVisibility(true, imageView, sharpenStrength, contrastAdaption, sharpenStrengthLabel, contrastAdaptionLabel);
        saveImageAction->setEnabled(true);

        // Resize the window to fit the image and sliders
        adjustSize();

        //reset sliders
        sharpenStrength->setValue(0);
        contrastAdaption->setValue(100);
    }
}

//Attempt to save the sharpened image
void MainWindow::saveImage()
{
    const QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QString(), "Images (*.png *.jpg *.bmp)");
    if (fileName.isEmpty())
        return;

    if (!sharpenedImage.save(fileName))
        QMessageBox::warning(this, "Save Image", "Failed to save the image.");
    else
        QMessageBox::information(this, "Save Image", "Image saved successfully.");
}

//event handler when a Slider is changed, triggers the CAS sharpening to occur and the display to show the new image
void MainWindow::sliderValueChanged()
{
    //don't calculate it parameters are (very close to) 0
    if (sharpenStrength->value() <= 0.001 || contrastAdaption->value() <= 0.001)
        return;

    //apply CAS CUDA from DLL and update UI
    const uchar* casData = CAS_sharpenImage(casObj, originalImage.constBits(), 1, CLAMP(sharpenStrength->value()), CLAMP(contrastAdaption->value()));
    sharpenedImage = QImage(casData, originalImage.width(), originalImage.height(), originalImage.width() * 3, QImage::Format_RGB888);
    updateImageView(sharpenedImage);

}
