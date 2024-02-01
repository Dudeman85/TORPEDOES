add_executable(TORPEDOES main.cpp  PlayerController.h)
target_link_libraries(TORPEDOES engine)
add_compile_definitions(TORPEDOES PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")
