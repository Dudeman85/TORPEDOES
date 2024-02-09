//Author Sauli Hanell month.day.year 2.1.2024 
#include <engine/Application.h>
#include <functional>
#include "BulletSystem.h"
#include "MenuSystem.h"
#include "engine/Input.h"

bool DoActionOnesWhenInputIsDown(int GLFW_KEY, bool& key, GLFWwindow* window, std::function<void(int)> action)
{

    if (key && glfwGetKey(window, GLFW_KEY) == GLFW_PRESS)
    {

        key = false;
        action(GLFW_KEY);

    }
    else if (glfwGetKey(window, GLFW_KEY) == GLFW_RELEASE) key = true;


    return key;
}

void OnSpacePressed(int a)
{

}
static void OnPausePressed(int a)  //pause menu
{
    printf("HERE IN ON PRESSED P\n");

    


}
//void OnEnterPressed(bool& key, auto& window)
//{
//    DoActionOnesWhenInputIsDown(GLFW_KEY_ENTER, spacePressed, window, OnSpacePressed);
//
//}



void shoot(auto& player, auto& bulletTexture, auto PlayerMonster)
{

    ecs::Entity pause = ecs::NewEntity();
    ecs::AddComponent(pause, new Transform{ .position = player.position + Vector3(0,0,1), .rotation = player.rotation, .scale = Vector3(10) });
    ecs::AddComponent(pause, new SpriteRenderer{ .texture = &bulletTexture });

    vector<Vector2> verts{ Vector2(1,1),Vector2(1,-1),Vector2(-1,-1),Vector2(-1,1) };
    ecs::AddComponent(pause, new PolygonCollider{ .vertices = verts });
    ecs::AddComponent(pause, new Rigidbody{ .velocity = TransformSystem::UpVector(PlayerMonster) * 990 / 10, .restitution = 1 });

}
//create points that tell distance to world origin
void CreatePoints(auto)
{
    ecs::Entity points = ecs::NewEntity();
    ecs::AddComponent(points, new Transform{ .position = Vector3(0,0,0),  .scale = Vector3(10) });
    //ecs::AddComponent(points, new SpriteRenderer{ .texture = &bulletTexture });
}
int main()
{

    //NO_OPENAL = true;

    //Create the window and OpenGL context before creating EngineLib
    //Parameters define window size(x,y) and name
    GLFWwindow* window = engine::CreateGLWindow(1800, 1600, "Window");

    bool isPaused = false;
    bool isSpacePressed = false, isUpArrowPressed = false, isDownArrowPressed = false;

    // DoActionOnesWhenInputIsDown(GLFW_KEY_SPACE, spacePressed, window, OnSpacePressed);



     //Initialize the engine, this must be called after creating a window
    engine::EngineInit();

    //Create the camera
    engine::Camera cam = engine::Camera(800, 600);

    //Move the camera back a bit
    cam.SetPosition(Vector3(0, 0, 10));

    float speed = 1;
    bool isArrowKeysMovePlayer = false, upArrowDown = true, downArrowDown = true, leftArrowDown, rightArrowDown;
    //Set the bacground color
    engine::SpriteRenderSystem::SetBackgroundColor(0, 150, 255);

    //Load some sprites
    engine::Texture Selected_Texture("Finihs_Line.png");
    engine::Texture monsterTexture("UI_Booster_Icon.png");
    engine::Texture UI_BG_Test_Texture("UI_BG_Test.png");
    engine::Texture bulletTexture("Torpedo_Test.png");
    engine::Texture wallTexture("UI_Empty_Box.png");

    //menu
    engine::Texture UI_BackToMenu("UI_BackToMenu.png");
    engine::Texture UI_BackToMenu_N("UI_BackToMenu_N.png");

    engine::Texture UI_StartGame("UI_StartGame.png");
    engine::Texture UI_StartGame_N("UI_StartGame_N.png");

    engine::Texture UI_Options("UI_Options.png");
    engine::Texture UI_Options_N("UI_Options_N.png");

    engine::Texture UI_Resume("UI_Resume.png");
    engine::Texture UI_Resume_N("UI_Resume_N.png");


    //bullet
    vector<ecs::Entity>bullets;
    float waitTime = 1, startTime = 0;


    //Create a new entity
    ecs::Entity ButtonStart = ecs::NewEntity();
    ecs::Entity ButtonOptions = ecs::NewEntity();
    ecs::Entity ButtonResume = ecs::NewEntity();

    ecs::Entity PlayerMonster = ecs::NewEntity();

    ecs::Entity UI_BG_Test = ecs::NewEntity();

    ecs::Entity strawberry = ecs::NewEntity();
    ecs::Entity wall = ecs::NewEntity();

    ecs::AddTag(PlayerMonster, "player");

    ecs::AddTag(UI_BG_Test, "BG");

    ecs::AddTag(strawberry, "entity");


    std::shared_ptr<PauseSystem> pauseSystem = ecs::GetSystem<PauseSystem>();
    pauseSystem->Init();
    ecs::Entity pause = ecs::NewEntity();

    bool readyToShoot = true;

    //Add the transform and SpriteRenderer components required for rendering a sprite
    ecs::AddComponent(strawberry, Transform{ .position = Vector3(0, 40, 0), .rotation = Vector3(0, 0, 45), .scale = Vector3(10) });
    ecs::AddComponent(strawberry, SpriteRenderer{ .texture = &Selected_Texture });

    ecs::AddComponent(PlayerMonster, Transform{ .position = Vector3(40, 40, 1), .rotation = Vector3(0, 0, 0), .scale = Vector3(10) });
    ecs::AddComponent(PlayerMonster, SpriteRenderer{ .texture = &monsterTexture });

    ecs::AddComponent(UI_BG_Test, Transform{ .position = Vector3(0,0,0), .rotation = Vector3(0, 0, 0), .scale = Vector3(500) });
    ecs::AddComponent(UI_BG_Test, SpriteRenderer{ .texture = &UI_BG_Test_Texture });

    //wall
    ecs::AddComponent(wall, Transform{ .position = Vector3(-10, 40, 1), .rotation = Vector3(0, 0, 0), .scale = Vector3(10,20) });
    vector<Vector2> wallVerts{ Vector2(2,2),Vector2(2,-2),Vector2(-2,-2),Vector2(-2,2) };

    ecs::AddComponent(wall, Rigidbody{ .mass = INFINITY, .gravityScale = 0, .restitution = 0, .kinematic = true });

    ecs::AddComponent(wall, PolygonCollider{ .vertices = wallVerts,  .visualise = true });
    ecs::AddComponent(wall, SpriteRenderer{ .texture = &wallTexture });

    input::initialize(window);
    input::InputEvent* upPressed = new input::InputEvent("up");
    input::InputEvent* downPressed = new input::InputEvent("down");
    input::InputEvent* enterPressed = new input::InputEvent("enter");
    input::InputEvent* pButtonPressed = new input::InputEvent("P");
    input::bindInput(GLFW_KEY_UP,{"up"});
    input::bindInput(GLFW_KEY_DOWN, { "down" });
    input::bindInput(GLFW_KEY_ENTER, { "enter" });
    input::bindInput(GLFW_KEY_P, { "P" });
    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
        input::update();

        Transform& playerTransform = ecs::GetComponent<Transform>(PlayerMonster);
        //Close window when Esc is pressed
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //If the W key is pressed
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            if (isArrowKeysMovePlayer) TransformSystem::Translate(PlayerMonster, Vector3(0, 1, 0) * speed);

            cam.Translate(Vector3(0, 1, 0) * speed);

        }
        //If the A key is pressed
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            if (isArrowKeysMovePlayer)  TransformSystem::Translate(PlayerMonster, Vector3(-1, 0, 0) * speed);
            cam.Translate(Vector3(-1, 0, 0) * speed);
        }
        //If the S key is pressed
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (isArrowKeysMovePlayer) TransformSystem::Translate(PlayerMonster, Vector3(0, -1, 0) * speed);
            cam.Translate(Vector3(0, -1, 0) * speed);
        }
        //If the D key is pressed
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            if (isArrowKeysMovePlayer) TransformSystem::Translate(PlayerMonster, Vector3(1, 0, 0) * speed);
            cam.Translate(Vector3(1, 0, 0) * speed);
        }
        //If the E key is pressed
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        {
            TransformSystem::Rotate(PlayerMonster, Vector3(0, 0, -90) * deltaTime);
        }
        //If the Q key is pressed
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        {
            TransformSystem::Rotate(PlayerMonster, Vector3(0, 0, 90) * deltaTime);
        }
        //if the R key is pressed change arrowkeys function
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) isArrowKeysMovePlayer = !isArrowKeysMovePlayer;



        DoActionOnesWhenInputIsDown(GLFW_KEY_P, isPaused, window, OnPausePressed);

        if (pButtonPressed->isNewPress())
        {
            ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();
        }
        if (isPaused && upPressed->isNewPress())
        {
            ecs::GetSystem<PauseSystem>()->MoveUpper();
        }
        if (isPaused && downPressed->isNewPress())
        {
            ecs::GetSystem<PauseSystem>()->MoveLower();
        }
        if (isPaused && enterPressed->isNewPress())
        {
            ecs::GetSystem<PauseSystem>()->Selected();
        }
        ////keyDown LOGIC
     /*   {


            if (upArrowDown && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                printf("UpArrow downn\n");
                upArrowDown = false;
            }
            else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) upArrowDown = true;

            if (downArrowDown && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            {
                printf("DownArrow down\n");
                downArrowDown = false;
            }
            else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)downArrowDown = true;


        }*/       

        //Zoom in 
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            TransformSystem::SetScale(PlayerMonster, (playerTransform.scale + 1));

        }
        //Zoom out
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        {
            TransformSystem::SetScale(PlayerMonster, (playerTransform.scale - 1));

        }

        //shoot
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        {
            shoot(playerTransform, bulletTexture, PlayerMonster);
            //	if (readyToShoot) 
            //	{
            //		readyToShoot = false;
            //		startTime =0;


            //	ecs::Entity bullet = ecs::NewEntity();
            //	ecs::AddComponent(bullet, new Transform{ .position = player.position+ Vector3(0,0,1), .rotation = player.rotation, .scale = Vector3(10)});
            //	ecs::AddComponent(bullet, new SpriteRenderer{ .texture = &bulletTexture});

            //	

            //	vector<Vector2> verts{ Vector2(1,1),Vector2(1,-1),Vector2(-1,-1),Vector2(-1,1) };
            //	ecs::AddComponent(bullet, new PolygonCollider{ .vertices = verts });
            //	auto bulletPhysics = ecs::AddComponent(bullet, new Rigidbody{ .velocity = TransformSystem::UpVector(PlayerMonster) * 990, .restitution = 1});

            ///*	bullets.push_back(bullet);*/
            //	}

        }
        if (!readyToShoot)
        {
            startTime += deltaTime;

            if (startTime >= waitTime) readyToShoot = true;
        }

        ecs::GetSystem<BulletSystem>()->Update();

        //Update all engine systems, this usually should go last in the game loop
        //For greater control of system execution, you can update each one manually
        engine::Update(&cam);

        //OpenGL stuff, goes very last
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    input::uninitialize();

    glfwTerminate();
}
