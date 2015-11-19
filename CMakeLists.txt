
cmake_minimum_required(VERSION 2.8)

PROJECT(ARFIVisualization)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

add_executable(ARFIVisualization MACOSX_BUNDLE ARFIVisualization)

if(VTK_LIBRARIES)
  target_link_libraries(ARFIVisualization ${VTK_LIBRARIES})
else()
  target_link_libraries(ARFIVisualization vtkHybrid vtkWidgets)
endif()
