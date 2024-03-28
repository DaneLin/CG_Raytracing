cmake_minimum_required(VERSION 3.0.0)
project(Raytracing VERSION 0.2.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(NAME RayTracing)

# 查找OpenMP
find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src thirdparty/glm thirdparty/loader)

file(GLOB SOURCES "src/*.cpp")



add_executable(RayTracing main.cpp ${SOURCES})

# 如果找到OpenMP，则链接OpenMP库
if(OpenMP_CXX_FOUND)
    target_link_libraries(RayTracing PUBLIC OpenMP::OpenMP_CXX)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
