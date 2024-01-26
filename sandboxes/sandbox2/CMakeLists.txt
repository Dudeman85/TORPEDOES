add_executable(sandbox2 OpenGL_TestScene.cpp SFML_Tiled_TestScene.cpp GLCheck.hpp PlayerController.h main.cpp TextRendering_TestScene.cpp ENetTest_Client.cpp ENetTest_Server.cpp)
target_link_libraries(sandbox2 engine)
add_compile_definitions(PROJECT_NAME="${CMAKE_CURRENT_SOURCE_DIR}")