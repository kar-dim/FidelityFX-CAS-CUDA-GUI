QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    widget_utils.cpp

HEADERS += \
    mainwindow.h \
    widget_utils.hpp

LIBS += -L$$PWD/libs -lFidelityFX-CAS-CUDA
INCLUDEPATH += $$PWD/libs/include

DISTFILES += \
    assets/theme_images/checkbox.png \
    assets/theme_images/down_arrow.png \
    assets/theme_images/handle.png \
    assets/fonts/TitilliumWeb-Regular.ttf \
    stylesheets/main.qss

RESOURCES += \
    Resources.qrc

RC_ICONS = assets/gpuopen.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
