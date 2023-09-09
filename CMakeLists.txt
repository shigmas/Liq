# run with something like:
# % mkdir build ; cd build
# % CMAKE_PREFIX_PATH=/opt/qt-6/ cmake ..
# % cmake --build . # (or just make)

cmake_minimum_required(VERSION 3.5)

project(LiqApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Qml Quick Widgets SerialPort)

qt_standard_project_setup()


qt_add_executable(LiqApp
        src/main.cpp
        src/Controller.cpp
        src/PointsItem.cpp
)

# Resources
# URI can't match the app
qt_add_qml_module(LiqApp
        URI Liq
        QML_FILES
                qml/liq.qml
)


target_include_directories(LiqApp PRIVATE rplidar_sdk/sdk/sdk/src rplidar_sdk/sdk/sdk/include)

target_link_directories(LiqApp PRIVATE rplidar_sdk/sdk/output/Linux/Release)
target_link_libraries(LiqApp PRIVATE Qt6::Gui Qt6::Quick Qt6::Widgets Qt6::SerialPort rplidar_sdk)
