cmake_minimum_required(VERSION 3.22)

project(ctpc
    VERSION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(third-party/ctre EXCLUDE_FROM_ALL)

add_library(ctpc INTERFACE)
add_library(ctpc::ctpc ALIAS ctpc)
target_link_libraries(ctpc INTERFACE ctre::ctre)
target_include_directories(ctpc INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(ctpc INTERFACE cxx_std_20)

option(CTPC_EXAMPLES "Build ctpc examples" OFF)
if(CTPC_EXAMPLES)
    add_subdirectory(examples)
endif()
