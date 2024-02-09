#pragma once
//Author: Sauli Hanell month.day.year 2.9.2024

#include <engine/Application.h>
#include <functional>

//Pause Component
ECS_REGISTER_COMPONENT(PauseComponent, Texture)
struct PauseComponent
{
    ecs::Entity upper, lower;
    engine::Texture* Selected_Texture;
    engine::Texture* unSelected_Texture;
    std::function<void()> operation;
};

ECS_REGISTER_SYSTEM(PauseSystem, PauseComponent, Transform)
class PauseSystem : public ecs::System
{
    ecs::Entity currentSelection;
    ecs::Entity resumeButton;
    ecs::Entity optionsButton;
    ecs::Entity menuButton;
    ecs::Entity quitGameButton;
  

    engine::Texture* currentSelected_Texture;

    vector<Texture*> allTextures;



public:
    ~PauseSystem() {
        delete currentSelected_Texture;
        //TODO DELETE ALL TEXTURES
        for (Texture* texture : allTextures)
        {
            delete texture;
        }
    }
    void Update()
    {


    }
    void Init()
    {
        resumeButton = ecs::NewEntity();
        optionsButton = ecs::NewEntity();
        menuButton = ecs::NewEntity();
        quitGameButton = ecs::NewEntity();
        
        currentSelection = menuButton;

        //Odd texture index is always ' _N ' and even is just default name
        allTextures.push_back(new engine::Texture("UI_Resume.png"));        //0
        allTextures.push_back(new engine::Texture("UI_Resume_N.png"));      //1
                                                                            
        allTextures.push_back(new engine::Texture("UI_Options.png"));       //2
        allTextures.push_back(new engine::Texture("UI_Options_N.png"));     //3
                                                                            
        allTextures.push_back(new engine::Texture("UI_Test_Box.png"));      //4
        allTextures.push_back(new engine::Texture("UI_BackToMenu_N.png"));  //5
                                                                            
        allTextures.push_back(new engine::Texture("UI_QuitGame.png"));      //6
        allTextures.push_back(new engine::Texture("UI_QuitGame_N.png"));    //7



        ecs::AddComponent(resumeButton, Transform{ .position = Vector3(0,.6f,-0.5f), .scale = Vector3(0.35f) });
        ecs::AddComponent(resumeButton, SpriteRenderer{ .texture = allTextures[0],  .enabled = false, .uiElement = true });
        ecs::AddComponent(resumeButton, PauseComponent{ .upper = quitGameButton, .lower = optionsButton, .Selected_Texture = allTextures[1], .unSelected_Texture = allTextures[0], .operation = PauseSystem::OnResumePressed });

        ecs::AddComponent(optionsButton, Transform{ .position = Vector3(0,.3,-0.5f), .scale = Vector3(0.35f) });
        ecs::AddComponent(optionsButton, SpriteRenderer{ .texture = allTextures[2],  .enabled = false, .uiElement = true });
        ecs::AddComponent(optionsButton, PauseComponent{ .upper = resumeButton, .lower = menuButton, .Selected_Texture = allTextures[3], .unSelected_Texture = allTextures[2], .operation = PauseSystem::OnOptionsPressed });


        ecs::AddComponent(menuButton, Transform{ .position = Vector3(0,0, -0.5f), .scale = Vector3(0.35f) });
        ecs::AddComponent(menuButton, SpriteRenderer{ .texture = allTextures[4],  .enabled = false, .uiElement = true });
        ecs::AddComponent(menuButton, PauseComponent{ .upper = optionsButton, .lower = quitGameButton, .Selected_Texture = allTextures[5], .unSelected_Texture = allTextures[4], .operation = PauseSystem::OnMainMenuPressed });


        ecs::AddComponent(quitGameButton, Transform{ .position = Vector3(0,-0.7f,-0.5f), .scale = Vector3(0.35f) });
        ecs::AddComponent(quitGameButton, SpriteRenderer{ .texture = allTextures[6],  .enabled = false, .uiElement = true });
        ecs::AddComponent(quitGameButton, PauseComponent{ .upper = menuButton, .lower = resumeButton, .Selected_Texture = allTextures[7], .unSelected_Texture = allTextures[6], .operation = PauseSystem::OnQuitGamePressed});



    }

    void MoveUpper()
    {
        printf("Move upper\n");
        PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
        SpriteRenderer& unselectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
        unselectedSpriteRenderer.texture = pauseComponent.unSelected_Texture;

        currentSelection = pauseComponent.upper; 
        PauseComponent& pauseComponent1 = ecs::GetComponent<PauseComponent>(currentSelection);
        SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
        selectedSpriteRenderer.texture = pauseComponent1.Selected_Texture;
        
    }
    void MoveLower()
    {
        printf("Move lower\n");
        PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
        SpriteRenderer& unselectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
        unselectedSpriteRenderer.texture = pauseComponent.unSelected_Texture;

        currentSelection = pauseComponent.lower;
        PauseComponent& pauseComponent1 = ecs::GetComponent<PauseComponent>(currentSelection);
        SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
        selectedSpriteRenderer.texture = pauseComponent1.Selected_Texture;

    }
    static  void OnResumePressed()
    {
        printf("OnResumePressed()\n");
    }
    static  void OnOptionsPressed()
    {
        printf("OnOptionsPressed()\n");

    }
    static void OnQuitGamePressed()
    {
        printf("OnQuitGamePressed()\n");
    }
    static  void OnMainMenuPressed()
    {
        printf("OnMainMenuPressed()\n");
    }
    void Selected() {

        PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
        SpriteRenderer& pauseSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
        pauseSpriteRenderer.texture = pauseComponent.Selected_Texture;
        pauseComponent.operation();

    }

    void ToggleShowUIMenu()
    {
        for (auto itr = entities.begin(); itr != entities.end();)
        {
            //Get the entity and increment the iterator
            ecs::Entity entity = *itr++;
            bool& enabled = ecs::GetComponent<SpriteRenderer>(entity).enabled;
            enabled = !enabled;

        }
    }

};


//enum class GameStates 
//{
//      GamePlay,MainMenu,Pause,Credits,Quit
//};
//enum class SelectionsInMenu {
//    Start,
//    Options,
//    Exit
//};
//enum class SelectionsInQuit {
//    Yes,
//    No
//};
//enum class SelectionsInPause {
//    Continue, MainMenu, Options
//};
//
//class GameStateMachine {
//public:
//    // Constructor with initial state
//    GameStateMachine() : currentGameState(GameStates::MainMenu) {}
//
//    // Method to transition to the next state
//    void nextState() {
//        switch (currentGameState) {
//        case GameStates::MainMenu:
//            std::cout << "Show main menu options\n";
//             SelectionsInMenu::Start;
//             SelectionsInMenu::Options;
//             SelectionsInMenu::Exit;
//            break;
//
//        case GameStates::GamePlay:
//            std::cout << "Run game\n";           
//            break;
//
//        case GameStates::Pause:
//            std::cout << "Show pause menu\n";
//
//            currentGameState = SelectionsInPause::Start;
//            break;
//
//        case GameStates::Credits:
//            std::cout << "Transitioning from State2 to State3\n";
//            currentGameState = SelectionsInMenu::Exit;
//            break;
//
//        case GameStates::Quit:
//            std::cout << "Transitioning from State2 to State3\n";
//            currentGameState = SelectionsInMenu::Exit;
//            break;
//        }
//    }
//
//    // Method to perform actions based on the current state
//    void ActionAvailableInThisGameState() {
//        switch (currentGameState) {
//        case GameStates::MainMenu:
//            std::cout << "Show main menu options\n";
//            SelectionsInMenu::Start;
//            SelectionsInMenu::Options;
//            SelectionsInMenu::Exit;
//            break;
//        case SelectionsInMenu::Options:
//            std::cout << "Performing action in State2\n";
//            break;
//        case SelectionsInMenu::Exit:
//            std::cout << "Performing action in State3\n";
//            break;
//        }
//    }
//
//private:
//    GameStates currentGameState;
//    SelectionsInPause SelectionsInPause
//};
//
//int main() {
//    GameStateMachine stateMachine;
//
//    for (int i = 0; i < 3; ++i) {
//        stateMachine.performAction();
//        stateMachine.nextState();
//    }
//
//    return 0;
//}

////USER INTERFACE component
//ECS_REGISTER_COMPONENT(UI, SpriteRenderer, Texture, Transform)
//struct selection : ecs::Component
//{
//	Menu upper;
//	Menu lower;
//	std::function<void()>  callback;
//};
//
//
//
//ECS_REGISTER_SYSTEM(Menu,Transform, SpriteRenderer, selection)
//class Menu : public ecs::System 
//{
//	bool isMenuActive = true;
//	selection oldSelection;
//	selection currentSelection;
//
//public : 
//	void Update() 
//	{
//		if (isMenuActive) 
//		{
//			
//			if (OnUpArrowPressed()) 
//			{
//				currentSelection = currentSelection.upper;
//			}
//			if (OnDownArrowPressed())
//			{
//				currentSelection = currentSelection.lower;
//			}
//			if (OnEnterPressed() && NULL != currentSelection.callback())
//			{
//				currentSelection.callback();
//			}
//		}
//
//	}
//
//	//Draw selected button as selected and draw oldSelection as normal 
//	RendererVisuals()
//	{
//
//	}
//
//};
