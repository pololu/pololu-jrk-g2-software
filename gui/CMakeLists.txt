use_cxx11()

# Find includes in corresponding build directories
set (CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories (
  "${CMAKE_CURRENT_SOURCE_DIR}/qt"
)

# Run Qt's moc automatically.
set (CMAKE_AUTOMOC ON)

find_package (Qt5Widgets)

configure_file (gui_info.rc.in gui_info.rc)

if (POLOLU_BUILD)
  if (APPLE)
    # Embed app.icns in the executable.
    qt5_add_resources(ICON_QRC qt/app_icns.qrc)
  else ()
    # Embed app.ico in the executable.
    qt5_add_resources(ICON_QRC qt/app_ico.qrc)
  endif ()
endif ()

add_executable (gui
  qt/altwindow.cpp
  qt/graphwindow.cpp
  qt/main.cpp
  qt/mainwindow.cpp
  qt/qcustomplot.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/gui_info.rc
  ${ICON_QRC}
  ${ICON_FILE}
)

set_target_properties (gui PROPERTIES
  OUTPUT_NAME ${GUI_NAME}
)

if (WIN32)
  set_target_properties (gui PROPERTIES
    LINK_FLAGS "-mwindows"
  )
endif ()

target_link_libraries (gui Qt5::Widgets) # TODO: add 'lib' and 'bootloader'
qt5_use_modules(gui PrintSupport)
qt5_use_modules(gui SerialPort)   # TODO: remove since we aren't using it

install(TARGETS gui DESTINATION bin)
