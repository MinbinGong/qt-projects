QT += widgets

TEMPLATE = app
TARGET = ProductChecker

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
    src/camera.cpp \
    src/detectionresultwindow.cpp \
    src/objectdetector.cpp

HEADERS += \
    include/mainwindow.h \
    include/camera.h \
    include/detectionresultwindow.h \
    include/objectdetector.h

FORMS += \
    form/mainwindow.ui

INCLUDEPATH += include \
    $$(OPENCV_SDK_DIR)/include

LIBS += -L$$(OPENCV_SDK_DIR)/x64/mingw/lib \
    -lopencv_core4120 \
    -lopencv_imgproc4120 \
    -lopencv_imgcodecs4120 \
    -lopencv_videoio4120 \
    -lopencv_dnn4120

YOLO_FILES = \
    $$PWD/libs/YOLO3/yolov3.weights \
    $$PWD/libs/YOLO3/yolo3.cfg \
    $$PWD/libs/YOLO3/coco.names

# 拷贝YOLO模型文件
win32 {
    # 创建目标目录
    QMAKE_POST_LINK = mkdir "$$DESTDIR/libs" 2>nul && mkdir "$$DESTDIR/libs/YOLO3" 2>nul
    # 拷贝YOLO模型文件
    QMAKE_POST_LINK += && xcopy /s /q /y /i "$$PWD/libs/YOLO3" "$$DESTDIR/libs/YOLO3"
    QMAKE_POST_LINK += && copy /y "$$PWD/libs/YOLO3/yolov3.weights" "$$DESTDIR/"
    QMAKE_POST_LINK += && copy /y "$$PWD/libs/YOLO3/yolo3.cfg" "$$DESTDIR/"
    QMAKE_POST_LINK += && copy /y "$$PWD/libs/YOLO3/coco.names" "$$DESTDIR/"
    # 部署Qt依赖
    WINDEPLOYQT = $$[QT_INSTALL_BINS]/windeployqt.exe
    QMAKE_POST_LINK += && "$$WINDEPLOYQT" --no-compiler-runtime --no-translations "$$DESTDIR$$TARGET.exe"
}
