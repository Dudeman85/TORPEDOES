add_executable(sandbox6 main.cpp BulletSystem.h)
target_link_libraries(sandbox6 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")
