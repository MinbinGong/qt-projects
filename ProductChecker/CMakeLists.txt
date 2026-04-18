cmake_minimum_required(VERSION 3.16)

project(ProductChecker VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt相关配置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/form)

# 查找Qt包
set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/mingw_64")
find_package(Qt6 COMPONENTS Widgets REQUIRED)

# 查找OpenCV包
set(OpenCV_DIR "C:/opencv-build/install/x64/mingw/lib")
find_package(OpenCV REQUIRED)

# 设置包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# 源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/camera.cpp
)

# 头文件
set(HEADERS
    include/mainwindow.h
    include/camera.h
)

# UI文件
set(UIS
    ${CMAKE_SOURCE_DIR}/form/mainwindow.ui
)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UIS})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    ${OpenCV_LIBS}
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/release
)
