add_executable(sandbox3 main.cpp)
target_link_libraries(sandbox3 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")
