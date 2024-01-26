add_executable(sandbox7 main.cpp)
target_link_libraries(sandbox7 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")