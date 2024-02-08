#pragma once
#include <engine/Application.h>

//Pause Component
ECS_REGISTER_COMPONENT(Pause)
struct Pause 
{
    float lifeTime = 5;
};

ECS_REGISTER_SYSTEM(PauseSystem, Pause, Transform)
class PauseSystem : public ecs::System
{
    engine::Texture* strawberry_Texture;
  
    ecs::Entity pauseText = ecs::NewEntity();
 

public:
    ~PauseSystem() {
        delete strawberry_Texture;
    }
    void Update()
    {
       

    }
    void Init()
    {
        strawberry_Texture = new engine::Texture("strawberry.png");
        ecs::AddComponent(pauseText, Pause{ .lifeTime = 3 });
        ecs::AddComponent(pauseText, Transform{ .position =  Vector3(0.5f,0.5f),  .scale = Vector3(66600) });
        ecs::AddComponent(pauseText, SpriteRenderer{ .texture = strawberry_Texture,  .enabled = false, .uiElement = true});
    }

    void ToggleShowUIMenu() 
    {
        for (auto itr = entities.begin(); itr != entities.end();)
        {
            //Get the entity and increment the iterator
            ecs::Entity entity = *itr++;
            auto enabled = ecs::GetComponent<SpriteRenderer>(entity).enabled;
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
//
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


