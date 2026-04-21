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

# 拷贝YOLO模型文件到输出目录
CONFIG(debug, debug|release) {
    YOLO_OUTPUT_DIR = $$DESTDIR/libs/YOLO3
} else {
    YOLO_OUTPUT_DIR = $$DESTDIR/libs/YOLO3
}

# 创建目录
QMAKE_POST_LINK += $$QMAKE_MKDIR_CMD $$YOLO_OUTPUT_DIR

# 拷贝文件
QMAKE_POST_LINK += $$QMAKE_COPY_DIR libs/YOLO3 $$YOLO_OUTPUT_DIR

# 拷贝到根目录（用于兼容性）
QMAKE_POST_LINK += $$QMAKE_COPY libs/YOLO3/yolov3.weights $$DESTDIR/
yolov3.weights
QMAKE_POST_LINK += $$QMAKE_COPY libs/YOLO3/yolo3.cfg $$DESTDIR/yolo3.cfg
QMAKE_POST_LINK += $$QMAKE_COPY libs/YOLO3/coco.names $$DESTDIR/coco.names

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/camera.cpp \
    src/detectionresultwindow.cpp \
    src/objectdetector.cpp

HEADERS += \
    include/mainwindow.h \
    include/camera.h \
    include/detectionresultwindow.h \
    include/objectdetector.h

FORMS += \
    Form/mainwindow.ui

INCLUDEPATH += include \
    $$(OPENCV_SDK_DIR)/include

LIBS += -L$$(OPENCV_SDK_DIR)/x64/mingw/lib \
    -lopencv_core4120 \
    -lopencv_imgproc4120 \
    -lopencv_imgcodecs4120 \
    -lopencv_videoio4120 \
    -lopencv_dnn4120
