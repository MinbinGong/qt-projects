QT += widgets

TEMPLATE = app
TARGET = ProductChecker

# 指定输出目录
CONFIG(debug, debug|release) {
    DESTDIR = build/debug
    OBJECTS_DIR = build/debug/obj
    MOC_DIR = build/debug/moc
    RCC_DIR = build/debug/rcc
    UI_DIR = build/debug/ui
} else {
    DESTDIR = build/release
    OBJECTS_DIR = build/release/obj
    MOC_DIR = build/release/moc
    RCC_DIR = build/release/rcc
    UI_DIR = build/release/ui
}

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/camera.cpp

HEADERS += \
    include/mainwindow.h \
    include/camera.h

FORMS += \
    Form/mainwindow.ui

INCLUDEPATH += include \
    $$(OPENCV_SDK_DIR)/include

LIBS += -L$$(OPENCV_SDK_DIR)/x64/mingw/lib \
    -lopencv_core4120 \
    -lopencv_imgproc4120 \
    -lopencv_imgcodecs4120 \
    -lopencv_videoio4120
