cmake_minimum_required(VERSION 3.22)

project(ctpc
    VERSION 0.1.0
    LANGUAGES CXX
)

option(CTPC_DEVEL "Enable default options for development" OFF)

add_subdirectory(third-party/ctre EXCLUDE_FROM_ALL)

add_library(ctpc INTERFACE)
add_library(ctpc::ctpc ALIAS ctpc)
target_link_libraries(ctpc INTERFACE ctre::ctre)
target_include_directories(ctpc INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(ctpc INTERFACE cxx_std_20)

option(CTPC_EXAMPLES "Build ctpc examples" ${CTPC_DEVEL})
if(CTPC_EXAMPLES)
    add_subdirectory(examples)
endif()

option(CTPC_TESTS "Build ctpc tests" ${CTPC_DEVEL})
if(CTPC_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.2.0
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extra")
    include(CTest)
    include(Catch)
    add_subdirectory(tests)
endif()

option(CTPC_DOCS "Build ctpc docs" ${CTPC_DEVEL})
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
        set(DOXYGEN_DISABLE_INDEX NO)
        set(DOXYGEN_FULL_SIDEBAR NO)
        set(DOXYGEN_EXCLUDE_SYMBOLS detail)
        set(DOXYGEN_EXTRACT_STATIC YES)
        set(DOXYGEN_STRIP_FROM_PATH include/)
        set(DOXYGEN_HIDE_SCOPE_NAMES YES)
        set(DOXYGEN_HTML_EXTRA_STYLESHEET
            "${PROJECT_SOURCE_DIR}/third-party/doxygen-awesome-css/doxygen-awesome.css"
            "${PROJECT_SOURCE_DIR}/third-party/doxygen-awesome-css/doxygen-awesome-sidebar-only.css"
        )
        doxygen_add_docs(docs
            "${PROJECT_SOURCE_DIR}/include"
            "${PROJECT_SOURCE_DIR}/docs/*.dox"
            "${PROJECT_BINARY_DIR}/mainpage.dox"
        )
    endif()
endif()
