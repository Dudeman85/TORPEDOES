add_executable(sandbox1 main.cpp PlayerController.h PickupController.h TurretController.h ScoreController.h)
target_link_libraries(sandbox1 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")