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

option(CTPC_DOCS "Build ctpc docs" ON)
if(CTPC_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        file(READ "${PROJECT_SOURCE_DIR}/README.md" README)
        string(REPLACE "\n" "\n/// " README "${README}")
        configure_file(
            "${PROJECT_SOURCE_DIR}/docs/mainpage.dox.in"
            "${PROJECT_BINARY_DIR}/mainpage.dox"
            @ONLY
        )
        set(DOXYGEN_GENERATE_MAN NO)
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_TREEVIEW YES)
        set(DOXYGEN_EXCLUDE_SYMBOLS detail)
        set(DOXYGEN_EXTRACT_STATIC YES)
        set(DOXYGEN_HIDE_SCOPE_NAMES YES)
        doxygen_add_docs(docs
            "${PROJECT_SOURCE_DIR}/include"
            "${PROJECT_SOURCE_DIR}/docs/*.dox"
            "${PROJECT_BINARY_DIR}/mainpage.dox"
        )
    endif()
endif()
