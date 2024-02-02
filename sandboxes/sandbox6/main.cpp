//Author Sauli Hanell 2.1.2024 
#include <engine/Application.h>
#include <functional>
#include "BulletSystem.h"
#include "MenuSystem.h"


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
 static void OnAPressed(int a)  //pause menu
{
     printf("HERE IN ON PRESSED P");
     ecs::GetSystem<PauseSystem>()->ToggleShowUIMenu();


}
//void OnEnterPressed(bool& key, auto& window)
//{
//    DoActionOnesWhenInputIsDown(GLFW_KEY_ENTER, spacePressed, window, OnSpacePressed);
//
//}


void KeyActions(int key)
{

    switch (key)
    {

    case 32:
        std::cout << "Space key pressed" << std::endl;
        break;
    case 39:
        std::cout << "Apostrophe key pressed" << std::endl;
        break;
    case 44:
        std::cout << "Comma key pressed" << std::endl;
        break;
    case 45:
        std::cout << "Minus key pressed" << std::endl;
        break;
    case 46:
        std::cout << "Period key pressed" << std::endl;
        break;
    case 47:
        std::cout << "Slash key pressed" << std::endl;
        break;
    case 48:
        std::cout << "Digit 0 key pressed" << std::endl;
        break;
    case 49:
        std::cout << "Digit 1 key pressed" << std::endl;
        break;
    case 50:
        std::cout << "Digit 2 key pressed" << std::endl;
        break;
    case 51:
        std::cout << "Digit 3 key pressed" << std::endl;
        break;
    case 52:
        std::cout << "Digit 4 key pressed" << std::endl;
        break;
    case 53:
        std::cout << "Digit 5 key pressed" << std::endl;
        break;
    case 54:
        std::cout << "Digit 6 key pressed" << std::endl;
        break;
    case 55:
        std::cout << "Digit 7 key pressed" << std::endl;
        break;
    case 56:
        std::cout << "Digit 8 key pressed" << std::endl;
        break;
    case 57:
        std::cout << "Digit 9 key pressed" << std::endl;
        break;
    case 59:
        std::cout << "Semicolon key pressed" << std::endl;
        break;
    case 61:
        std::cout << "Equal key pressed" << std::endl;
        break;



        //characters
    case 65:
        std::cout << "A key pressed" << std::endl;
        break;
    case 66:
        std::cout << "B key pressed" << std::endl;
        break;
    case 67:
        std::cout << "C key pressed" << std::endl;
        break;
    case 68:
        std::cout << "D key pressed" << std::endl;
        break;
    case 69:
        std::cout << "E key pressed" << std::endl;
        break;
    case 70:
        std::cout << "F key pressed" << std::endl;
        break;
    case 71:
        std::cout << "G key pressed" << std::endl;
        break;
    case 72:
        std::cout << "H key pressed" << std::endl;
        break;
    case 73:
        std::cout << "I key pressed" << std::endl;
        break;
    case 74:
        std::cout << "J key pressed" << std::endl;
        break;
    case 75:
        std::cout << "K key pressed" << std::endl;
        break;
    case 76:
        std::cout << "L key pressed" << std::endl;
        break;
    case 77:
        std::cout << "M key pressed" << std::endl;
        break;
    case 78:
        std::cout << "N key pressed" << std::endl;
        break;
    case 79:
        std::cout << "O key pressed" << std::endl;
        break;
    case 80:
        std::cout << "P key pressed" << std::endl;
        break;
    case 81:
        std::cout << "Q key pressed" << std::endl;
        break;
    case 82:
        std::cout << "R key pressed" << std::endl;
        break;
    case 83:
        std::cout << "S key pressed" << std::endl;
        break;
    case 84:
        std::cout << "T key pressed" << std::endl;
        break;
    case 85:
        std::cout << "U key pressed" << std::endl;
        break;
    case 86:
        std::cout << "V key pressed" << std::endl;
        break;
    case 87:
        std::cout << "W key pressed" << std::endl;
        break;
    case 88:
        std::cout << "X key pressed" << std::endl;
        break;
    case 89:
        std::cout << "Y key pressed" << std::endl;
        break;
    case 90:
        std::cout << "Z key pressed" << std::endl;

        break;
    case 91:
        std::cout << "Left bracket key pressed" << std::endl;
        break;
    case 92:
        std::cout << "Backslash key pressed" << std::endl;
        break;
    case 93:
        std::cout << "Right bracket key pressed" << std::endl;
        break;
    case 96:
        std::cout << "Grave accent key pressed" << std::endl;
        break;
    case 161:
        std::cout << "World 1 key pressed" << std::endl;
        break;
    case 162:
        std::cout << "World 2 key pressed" << std::endl;
        break;
    case 256:
        std::cout << "Escape key pressed" << std::endl;
        break;
    case 257:
        std::cout << "Enter key pressed" << std::endl;
        break;
    case 258:
        std::cout << "Tab key pressed" << std::endl;
        break;
    case 259:
        std::cout << "Backspace key pressed" << std::endl;
        break;
    case 260:
        std::cout << "Insert key pressed" << std::endl;
        break;
    case 261:
        std::cout << "Delete key pressed" << std::endl;
        break;
    case 262:
        std::cout << "Right arrow key pressed" << std::endl;
        break;
    case 263:
        std::cout << "Left arrow key pressed" << std::endl;
        break;
    case 264:
        std::cout << "Down arrow key pressed" << std::endl;
        break;
    case 265:
        std::cout << "Up arrow key pressed" << std::endl;
        break;
    case 266:
        std::cout << "Page up key pressed" << std::endl;
        break;
    case 267:
        std::cout << "Page down key pressed" << std::endl;
        break;
    case 268:
        std::cout << "Home key pressed" << std::endl;
        break;
    case 269:
        std::cout << "End key pressed" << std::endl;
        break;
    case 280:
        std::cout << "Caps Lock key pressed" << std::endl;
        break;
    case 281:
        std::cout << "Scroll Lock key pressed" << std::endl;
        break;
    case 282:
        std::cout << "Num Lock key pressed" << std::endl;
        break;
    case 283:
        std::cout << "Print Screen key pressed" << std::endl;
        break;
    case 284:
        std::cout << "Pause key pressed" << std::endl;
        break;

        //Fn

    case 290:
        std::cout << "F1 key pressed" << std::endl;
        break;
    case 291:
        std::cout << "F2 key pressed" << std::endl;
        break;
    case 292:
        std::cout << "F3 key pressed" << std::endl;
        break;
    case 293:
        std::cout << "F4 key pressed" << std::endl;
        break;
    case 294:
        std::cout << "F5 key pressed" << std::endl;
        break;
    case 295:
        std::cout << "F6 key pressed" << std::endl;
        break;
    case 296:
        std::cout << "F7 key pressed" << std::endl;
        break;
    case 297:
        std::cout << "F8 key pressed" << std::endl;
        break;
    case 298:
        std::cout << "F9 key pressed" << std::endl;
        break;
    case 299:
        std::cout << "F10 key pressed" << std::endl;
        break;
    case 300:
        std::cout << "F11 key pressed" << std::endl;
        break;
    case 301:
        std::cout << "F12 key pressed" << std::endl;
        break;
    case 302:
        std::cout << "F13 key pressed" << std::endl;
        break;
    case 303:
        std::cout << "F14 key pressed" << std::endl;
        break;
    case 304:
        std::cout << "F15 key pressed" << std::endl;
        break;
    case 305:
        std::cout << "F16 key pressed" << std::endl;
        break;
    case 306:
        std::cout << "F17 key pressed" << std::endl;
        break;
    case 307:
        std::cout << "F18 key pressed" << std::endl;
        break;
    case 308:
        std::cout << "F19 key pressed" << std::endl;
        break;
    case 309:
        std::cout << "F20 key pressed" << std::endl;
        break;
    case 310:
        std::cout << "F21 key pressed" << std::endl;
        break;
    case 311:
        std::cout << "F22 key pressed" << std::endl;
        break;
    case 312:
        std::cout << "F23 key pressed" << std::endl;
        break;
    case 313:
        std::cout << "F24 key pressed" << std::endl;
        break;
    case 314:
        std::cout << "F25 key pressed" << std::endl;
        break;


        //keypad            
    case 320:
        std::cout << "Keypad 0 key pressed" << std::endl;
        break;
    case 321:
        std::cout << "Keypad 1 key pressed" << std::endl;
        break;
    case 322:
        std::cout << "Keypad 2 key pressed" << std::endl;
        break;
    case 323:
        std::cout << "Keypad 3 key pressed" << std::endl;
        break;
    case 324:
        std::cout << "Keypad 4 key pressed" << std::endl;
        break;
    case 325:
        std::cout << "Keypad 5 key pressed" << std::endl;
        break;
    case 326:
        std::cout << "Keypad 6 key pressed" << std::endl;
        break;
    case 327:
        std::cout << "Keypad 7 key pressed" << std::endl;
        break;
    case 328:
        std::cout << "Keypad 8 key pressed" << std::endl;
        break;
    case 329:
        std::cout << "Keypad 9 key pressed" << std::endl;
        break;
    case 330:
        std::cout << "Keypad Decimal key pressed" << std::endl;
        break;
    case 331:
        std::cout << "Keypad Divide key pressed" << std::endl;
        break;
    case 332:
        std::cout << "Keypad Multiply key pressed" << std::endl;
        break;
    case 333:
        std::cout << "Keypad Subtract key pressed" << std::endl;
        break;
    case 334:
        std::cout << "Keypad Add key pressed" << std::endl;
        break;
    case 335:
        std::cout << "Keypad Enter key pressed" << std::endl;
        break;
    case 336:
        std::cout << "Keypad Equal key pressed" << std::endl;
        break;

    case 340:
        std::cout << "Left Shift key pressed" << std::endl;
        break;
    case 341:
        std::cout << "Left Control key pressed" << std::endl;
        break;
    case 342:
        std::cout << "Left Alt key pressed" << std::endl;
        break;
    case 343:
        std::cout << "Left Super key pressed" << std::endl;
        break;
    case 344:
        std::cout << "Right Shift key pressed" << std::endl;
        break;
    case 345:
        std::cout << "Right Control key pressed" << std::endl;
        break;
    case 346:
        std::cout << "Right Alt key pressed" << std::endl;
        break;
    case 347:
        std::cout << "Right Super key pressed" << std::endl;
        break;
    case 348:
        std::cout << "Menu key pressed" << std::endl;
        break;
    default:
        std::cout << "Key not recognized" << std::endl;
        break;

    }
}

void shoot(auto& player, auto& bulletTexture, auto PlayerMonster)
{

    ecs::Entity pauseText = ecs::NewEntity();
    ecs::AddComponent(pauseText, new Transform{ .position = player.position + Vector3(0,0,1), .rotation = player.rotation, .scale = Vector3(10) });
    ecs::AddComponent(pauseText, new SpriteRenderer{ .texture = &bulletTexture });

    vector<Vector2> verts{ Vector2(1,1),Vector2(1,-1),Vector2(-1,-1),Vector2(-1,1) };
    ecs::AddComponent(pauseText, new PolygonCollider{ .vertices = verts });
    ecs::AddComponent(pauseText, new Rigidbody{ .velocity = TransformSystem::UpVector(PlayerMonster) * 990 / 10, .restitution = 1 });
    ecs::AddComponent(pauseText, new Pause{.lifeTime = 2});
}
int main()
{

    //NO_OPENAL = true;

    //Create the window and OpenGL context before creating EngineLib
    //Parameters define window size(x,y) and name
    GLFWwindow* window = engine::CreateGLWindow(1800, 1600, "Window");

    bool OnPPressed  = false;
    bool spacePressed = false, upArrowPressed = false,downArrowPressed = false;

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
    engine::Texture strawberry_Texture("strawberry.png");
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


    bool readyToShoot = true;

    //Add the transform and SpriteRenderer components required for rendering a sprite
    ecs::AddComponent(strawberry, Transform{ .position = Vector3(0, 40, 0), .rotation = Vector3(0, 0, 45), .scale = Vector3(10) });
    ecs::AddComponent(strawberry, SpriteRenderer{ .texture = &strawberry_Texture });

     ecs::AddComponent(PlayerMonster,Transform{ .position = Vector3(40, 40, 1), .rotation = Vector3(0, 0, 0), .scale = Vector3(10) });
    ecs::AddComponent(PlayerMonster, SpriteRenderer{ .texture = &monsterTexture });

    ecs::AddComponent(UI_BG_Test, Transform{ .position = Vector3(0,0,0), .rotation = Vector3(0, 0, 0), .scale = Vector3(500) });
    ecs::AddComponent(UI_BG_Test, SpriteRenderer{ .texture = &UI_BG_Test_Texture });

    //wall
    ecs::AddComponent(wall, Transform{ .position = Vector3(-10, 40, 1), .rotation = Vector3(0, 0, 0), .scale = Vector3(10,20) });
    vector<Vector2> wallVerts{ Vector2(2,2),Vector2(2,-2),Vector2(-2,-2),Vector2(-2,2) };

    ecs::AddComponent(wall,  Rigidbody{ .mass = INFINITY, .gravityScale = 0, .restitution = 0, .kinematic = true });

    ecs::AddComponent(wall,  PolygonCollider{ .vertices = wallVerts,  .visualise = true });
    ecs::AddComponent(wall,  SpriteRenderer{ .texture = &wallTexture });
    //Game Loop
    while (!glfwWindowShouldClose(window))
    {
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



        DoActionOnesWhenInputIsDown(GLFW_KEY_P, OnPPressed, window, OnAPressed);


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

    glfwTerminate();
}
