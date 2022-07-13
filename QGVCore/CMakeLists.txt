# all files which may be mocced
SET(qgvlib_CPP
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

SET(TARGET_NAME "QGVCore")

# Activate Export macro for building the library itself
# if QGVCORE_LIB is not set or equal 0 (null), import macro is used
ADD_DEFINITIONS(-DQGVCORE_LIB -D_PACKAGE_ast -D_dll_import -D_BLD_cdt -D_DLL_BLD)

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
        PRIVATE ${GRAPHVIZ_CDT_LIBRARY}
        PRIVATE ${GRAPHVIZ_CGRAPH_LIBRARY}
        PRIVATE ${GRAPHVIZ_GVC_LIBRARY})

target_include_directories(qgvcore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/private
    PRIVATE ${GRAPHVIZ_INCLUDE_DIRS})

SET_TARGET_PROPERTIES(qgvcore PROPERTIES
	OUTPUT_NAME "QGVCore"
	VERSION ${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${${PROJECT_NAME}_PATCH_LEVEL}
	SOVERSION ${TARGET_VERSION_MAJOR})
IF(WIN32)
	# this extra install command is needed by the nsis installer
	INSTALL(TARGETS qgvcore
		RUNTIME DESTINATION ${LIB_INSTALL_DIR} 
		COMPONENT library 
	)
ELSE(WIN32)
	INSTALL(TARGETS qgvcore
		ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
		LIBRARY DESTINATION ${LIB_INSTALL_DIR}
		COMPONENT library
	)
ENDIF(WIN32)

# The COMPONENT option only defines the internal belonging of the INSTALL'ed thing
# INSTALL(
  # FILES ${qgvlib_TRANS}
  # DESTINATION ${qm_DIR}  
  # COMPONENT library
# )

#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}.pc.in
#                ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc @ONLY)
#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_NAME}-uninstalled.pc.in
#                ${CMAKE_BINARY_DIR}/${TARGET_NAME}-uninstalled.pc @ONLY)
#
#INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.pc
#                DESTINATION lib/pkgconfig)
#
#IF(WIN32)
#	# QuaZip
#	# FIND_FILE(PACK_zlib zlib.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
#	# INSTALL(FILES ${QUAZIP_LIBRARIES} ${PACK_zlib} DESTINATION ${bin_DIR} COMPONENT library)
#
#	# Qt itself
#	FIND_FILE(PACK_qtcore QtCore4.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
#	FIND_FILE(PACK_qtgui QtGui4.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
#	
#	FIND_FILE(PACK_mingwm10 mingwm10.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
#	FIND_FILE(PACK_libgcc_s_dw libgcc_s_dw2-1.dll PATHS ${QT_BINARY_DIR} NO_DEFAULT_PATH)
#	
#	INSTALL(
#		FILES 
#			${PACK_qtcore}
#			${PACK_qtgui}
#			# ${PACK_qtnetwork}
#			# ${PACK_qtsql}
#			# ${PACK_libmysql}
#			# ${PACK_qtxml}
#			# ${PACK_qtxml_patterns}
#			${PACK_mingwm10}
#			${PACK_libgcc_s_dw}
#      DESTINATION ${bin_DIR}
#		COMPONENT qtlibs
#	)
#  
#  INSTALL(
#    FILES
#      ${GRAPHVIZ_GVC_LIBRARY}
#      ${GRAPHVIZ_CGRAPH_LIBRARY}
#      ${GRAPHVIZ_CDT_LIBRARY}
#      DESTINATION ${bin_DIR}
#    COMPONENT graphvizlibs
#  )
#ENDIF(WIN32)

#IF(BUILD_DEV)
#	FILE(GLOB header "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
#	INSTALL(
#		FILES ${header}
#		DESTINATION ${include_DIR}
#		COMPONENT headerfiles
#	)
#ENDIF(BUILD_DEV)
