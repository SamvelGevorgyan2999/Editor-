find_package(Qt6 REQUIRED COMPONENTS Test)

# Helper to declare a Qt Test executable linked against the core library.
function(ate_add_test name source)
    qt_add_executable(${name} ${source})
    target_link_libraries(${name} PRIVATE ate_core Qt6::Test)
    add_test(NAME ${name} COMMAND ${name})
endfunction()

ate_add_test(test_documentstats test_documentstats.cpp)
ate_add_test(test_filemanager   test_filemanager.cpp)


