macro(ctpc_test NAME)
    add_executable(${NAME}_test ${NAME}.cpp)
    target_link_libraries(${NAME}_test PRIVATE ctpc::ctpc Catch2::Catch2WithMain)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${NAME}_test PRIVATE -Wno-nonnull)
    endif()
    catch_discover_tests(${NAME}_test)
endmacro()

ctpc_test(utf)
ctpc_test(verbatim)
