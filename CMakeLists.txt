######################
#
#    Copyright (C) 2014 EngSaS - Engineering Solutions and Services Langenbach. All rights reserved.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
######################

cmake_minimum_required(VERSION 3.12)
project(qgv)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET_VERSION_MAJOR 0 )
    set(TARGET_VERSION_MINOR 1 )
    set(${PROJECT_NAME}_PATCH_LEVEL   0 )
set(${PROJECT_NAME}_SOVERSION ${TARGET_VERSION_MAJOR})
set(${PROJECT_NAME}_VERSION
    "${TARGET_VERSION_MAJOR}.${TARGET_VERSION_MINOR}.${${PROJECT_NAME}_PATCH_LEVEL}" )

## Create binaries in the root of the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Extending the Module path with the local module path of desuto
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets Gui)
find_package(Graphviz REQUIRED)

######################
#
# include source tree
#
######################
set(libqgv_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/QGVCore)
set(libqgv_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/QGVCore)
add_subdirectory(QGVCore)
add_subdirectory(Sample)
