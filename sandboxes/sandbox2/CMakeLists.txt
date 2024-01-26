add_executable(sandbox2 GLCheck.hpp PlayerController.h main.cpp)
target_link_libraries(sandbox2 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")