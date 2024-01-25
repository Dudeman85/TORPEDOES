add_executable(sandbox1 main.cpp client.cpp server.cpp PlayerController.h PickupController.h TurretController.h ScoreController.h NetCode.h NetSerialize.h)
target_include_directories(sandbox1 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(sandbox1 engine)
