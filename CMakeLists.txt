cmake_minimum_required(VERSION 3.10)
project(MyVTKProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(target_name vtkReader)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
include_directories(include)

set(Sources 
  src/main.cpp
  src/vtkReader.cpp
  src/polyData.cpp
)

add_executable(${target_name} ${Sources})
target_link_libraries(${target_name} PRIVATE ${VTK_LIBRARIES})
target_compile_definitions(${target_name} PRIVATE DEBUG)

