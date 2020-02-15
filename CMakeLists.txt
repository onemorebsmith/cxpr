# CMakeList.txt : CMake project for K3D, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.14)

project(cxpr VERSION "0.1" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)


option(CXPR_BUILD_TESTS "Build and run cxpr tests" ON)

file(GLOB_RECURSE HEADERS "*.h")

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${HEADERS})
target_include_directories(${PROJECT_NAME} INTERFACE $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)


if(CXPR_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


#ExportHeaderLib(${PROJECT_NAME} .)