add_executable(demo1 main.cpp PlayerController.h PickupController.h EnemyController.h TurretController.h Physics.h)
target_link_libraries(demo1 engine)
