# enable all QT-Stuff we need
# see http://cmake.org/cmake/help/cmake2.6docs.html#module:FindQt4 for a complete list

SET(qgv_TS
)

SET(qgv_UI
)

# all files which may be mocced
SET(qgv_CPP
    )

add_executable(qgv_example
    MainWindow.ui
    ress.qrc
    main.cpp
    MainWindow.cpp
    QGraphicsViewEc.cpp
    )

target_link_libraries(qgv_example
    PRIVATE qgvcore
    PRIVATE Qt5::Widgets
    PRIVATE Qt5::Gui
    )

target_include_directories(qgv_example
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# The COMPONENT option only defines the internal belonging of the INSTALL'ed thing
INSTALL(
  TARGETS qgv_example
  RUNTIME DESTINATION bin)
