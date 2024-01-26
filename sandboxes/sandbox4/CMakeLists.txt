add_executable(sandbox4 main.cpp)
target_link_libraries(sandbox4 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")
