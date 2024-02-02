add_executable(sandbox5 main.cpp PlayerController.h)
target_link_libraries(sandbox5 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")
