cmake_minimum_required(VERSION 3.0.0)
project(Raytracing VERSION 0.2.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(NAME RayTracing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src thirdparty)

file(GLOB SOURCES "src/*.cpp")

add_executable(RayTracing main.cpp ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
