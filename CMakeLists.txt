# Reference: http://qt-project.org/doc/qt-5.9/qtdoc/cmake-manual.html

cmake_minimum_required(VERSION 3.0)

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include Qt basic functions
include(QtCommon)

# Basic information about project

project(example VERSION 1.0)

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()



set(SOURCE_FILES
    src/main.cpp
	src/graphwindow.cpp
	src/mainwindow.cpp
	src/qcustomplot.cpp	
	src/altwindow.cpp
)






find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)

# source_group("UI Files" FILES ${UI_FILES})

add_executable(${PROJECT_NAME} ${OS_BUNDLE} # Expands to WIN32 or MACOS_BUNDLE depending on OS
    ${SOURCE_FILES} ${META_FILES_TO_INCLUDE} ${RESOURCE_FILES}
)

qt5_use_modules(${PROJECT_NAME} Widgets)
qt5_use_modules(${PROJECT_NAME} PrintSupport)
qt5_use_modules(${PROJECT_NAME} SerialPort)
qt5_use_modules(${PROJECT_NAME} Concurrent)




