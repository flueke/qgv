# Activate Export macro for building the library itself
# if QGVCORE_LIB is not set or equal 0 (null), import macro is used
add_definitions(-DQGVCORE_LIB -D_PACKAGE_ast -D_dll_import -D_BLD_cdt -D_DLL_BLD)

add_library(qgvcore SHARED
    private/QGVCore.cpp
    private/QGVGraphPrivate.cpp
    private/QGVEdgePrivate.cpp
    private/QGVGvcPrivate.cpp
    private/QGVNodePrivate.cpp
    QGVEdge.cpp
    QGVNode.cpp
    QGVScene.cpp
    QGVSubGraph.cpp
    )

target_link_libraries(qgvcore
        PRIVATE Qt5::Widgets
        PRIVATE Qt5::Gui
        PUBLIC ${GRAPHVIZ_CDT_LIBRARY}
        PUBLIC ${GRAPHVIZ_CGRAPH_LIBRARY}
        PUBLIC ${GRAPHVIZ_GVC_LIBRARY})

target_include_directories(qgvcore
    PRIVATE ${GRAPHVIZ_INCLUDE_DIRS}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)

SET_TARGET_PROPERTIES(qgvcore PROPERTIES
	VERSION ${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${${PROJECT_NAME}_PATCH_LEVEL}
	SOVERSION ${TARGET_VERSION_MAJOR})

install(TARGETS qgvcore
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
)
