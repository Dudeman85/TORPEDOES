//#pragma once
//#include <engine/Application.h>
//#include <functional>
////Author: Sauli Hanell month.day.year 2.22.2024
////Pause Component
//ECS_REGISTER_COMPONENT(Hedgehog, Texture)
//struct HedgehogComponent
//{
//	ecs::Entity upper, lower;
//	engine::Texture* selectedTexture;
//	engine::Texture* unselectedTexture;
//	std::function<void()> operation;
//	bool isOptionsMenu = false;
//	bool isSlider = false;
//	float sliderValue = 0;
//};
//
//ECS_REGISTER_SYSTEM(ShipSystem, PauseComponent, Transform)
//class ShipSystem : public ecs::System
//{
//	ecs::Entity pausedImage;
//
//	ecs::Entity resumeButton;
//	ecs::Entity optionsButton;
//
//	ecs::Entity mainMenuButton;
//
//	ecs::Entity quitGameButton;
//
//	//std::map<std::string, ecs::Entity> pauseButtons{ {"return", resumeButton}, {"options", optionsButton}, {"mainMenu", mainMenuButton},{"quitGame", quitGameButton} };
//
//
//	//options buttons
//	ecs::Entity optionsResumeButton;
//	ecs::Entity musicSliderEntity, musicSliderNub;;
//	ecs::Entity fullscreenEntity;
//
//	vector<ecs::Entity>  optionsButtons = { {optionsResumeButton},{musicSliderEntity}, {fullscreenEntity} };
//	//std::map<std::string, ecs::Entity> optionsButtons{ {"return", optionsResumeButton}, {"music", musicSliderEntity}, {"fullscreen", fullscreenEntity} };
//
//	engine::Texture* currentSelected_Texture;
//
//	vector<Texture*> allTextures;
//	static GLFWwindow* window;
//
//public:
//	ecs::Entity currentSelection;
//	bool isGamePause;
//
//	~ShipSystem() {
//		delete currentSelected_Texture;
//		//TODO DELETE ALL TEXTURES
//		for (Texture* texture : allTextures)
//		{
//			delete texture;
//		}
//	}
//	void Update()
//	{
//
//
//	}
//	void Init(GLFWwindow* mainWindow)
//	{
//		ShipSystem::window = mainWindow;
//		resumeButton = ecs::NewEntity();
//		optionsButton = ecs::NewEntity();
//		mainMenuButton = ecs::NewEntity();
//		quitGameButton = ecs::NewEntity();
//
//		optionsResumeButton = ecs::NewEntity();
//		musicSliderEntity = ecs::NewEntity();
//		fullscreenEntity = ecs::NewEntity();
//
//
//		musicSliderNub = ecs::NewEntity();
//		currentSelection = mainMenuButton;
//
//		pausedImage = ecs::NewEntity();
//
//
//
//
//		//Odd texture index is always ' _N ' and even is just default name
//		//Pausemenu button textures
//		allTextures.push_back(new engine::Texture("menuUI/UI_Resume_N.png", GL_LINEAR_MIPMAP_NEAREST));
//		allTextures.push_back(new engine::Texture("menuUI/UI_Resume.png", GL_LINEAR_MIPMAP_NEAREST));
//
//		allTextures.push_back(new engine::Texture("menuUI/UI_Options_N.png", GL_LINEAR_MIPMAP_NEAREST));
//		allTextures.push_back(new engine::Texture("menuUI/UI_Options.png", GL_LINEAR_MIPMAP_NEAREST));
//
//		allTextures.push_back(new engine::Texture("menuUI/UI_BackToMenu_N.png", GL_LINEAR_MIPMAP_NEAREST));
//		allTextures.push_back(new engine::Texture("menuUI/UI_BackToMenu.png", GL_LINEAR_MIPMAP_NEAREST));
//
//		allTextures.push_back(new engine::Texture("menuUI/UI_QuitGame_N.png", GL_LINEAR_MIPMAP_NEAREST));
//		allTextures.push_back(new engine::Texture("menuUI/UI_QuitGame.png", GL_LINEAR_MIPMAP_NEAREST));
//
//		//Options buttons  textures
//		allTextures.push_back(new engine::Texture("menuUI/UI_Resume.png", GL_LINEAR_MIPMAP_NEAREST));              // 	9
//		allTextures.push_back(new engine::Texture("menuUI/UI_Resume_N.png", GL_LINEAR_MIPMAP_NEAREST));            // 8
//
//		allTextures.push_back(new engine::Texture("menuUI/UI_Music_Slider.png", GL_LINEAR_MIPMAP_NEAREST));        // 11
//		allTextures.push_back(new engine::Texture("menuUI/UI_Music_Slider_N.png", GL_LINEAR_MIPMAP_NEAREST));      // 10
//
//		allTextures.push_back(new engine::Texture("menuUI/UI_Fullscreen.png", GL_LINEAR_MIPMAP_NEAREST));          // 13
//		allTextures.push_back(new engine::Texture("menuUI/UI_Fullscreen_N.png", GL_LINEAR_MIPMAP_NEAREST));        // 12
//
//		//PAUSED image
//		allTextures.push_back(new engine::Texture("menuUI/UI_Paused.png", GL_LINEAR_MIPMAP_NEAREST));              // 14
//
//		allTextures.push_back(new engine::Texture("menuUI/UI_Slider_Button.png", GL_LINEAR_MIPMAP_NEAREST));       // 15
//
//		//                      ---------  PAUSE BUTTONS init-------
//		ecs::AddComponent(pausedImage, Transform{ .position = Vector3(0,.8f,-0.1f), .scale = Vector3(0.35f) });
//		ecs::AddComponent(pausedImage, SpriteRenderer{ .texture = allTextures[14],  .enabled = false, .uiElement = true });
//		ecs::AddComponent(pausedImage, HedgehogComponent{ .upper = pausedImage, .lower = optionsButton, .selectedTexture = allTextures[14], .unselectedTexture = allTextures[14], .operation = PauseSystem::OnResumePressed });
//
//
//		ecs::AddComponent(resumeButton, Transform{ .position = Vector3(0,.5f,-0.1f), .scale = Vector3(0.25f) });
//		ecs::AddComponent(resumeButton, SpriteRenderer{ .texture = allTextures[0],  .enabled = false, .uiElement = true });
//		ecs::AddComponent(resumeButton, HedgehogComponent{ .upper = quitGameButton, .lower = optionsButton, .selectedTexture = allTextures[1], .unselectedTexture = allTextures[0], .operation = PauseSystem::OnResumePressed });
//
//		ecs::AddComponent(optionsButton, Transform{ .position = Vector3(0,.3f,-0.1f), .scale = Vector3(.25f) });
//		ecs::AddComponent(optionsButton, SpriteRenderer{ .texture = allTextures[2],  .enabled = false, .uiElement = true });
//		ecs::AddComponent(optionsButton, HedgehogComponent{ .upper = resumeButton, .lower = mainMenuButton, .selectedTexture = allTextures[3], .unselectedTexture = allTextures[2], .operation = PauseSystem::OnOptionsPressed });
//
//		ecs::AddComponent(mainMenuButton, Transform{ .position = Vector3(0,.1f,-0.1f), .scale = Vector3(0.25f) });
//		ecs::AddComponent(mainMenuButton, SpriteRenderer{ .texture = allTextures[4],  .enabled = false, .uiElement = true });
//		ecs::AddComponent(mainMenuButton, HedgehogComponent{ .upper = optionsButton, .lower = quitGameButton, .selectedTexture = allTextures[5], .unselectedTexture = allTextures[4], .operation = PauseSystem::OnMainMenuPressed });
//
//		ecs::AddComponent(quitGameButton, Transform{ .position = Vector3(0,-0.7f,-0.1f), .scale = Vector3(0.25f) });
//		ecs::AddComponent(quitGameButton, SpriteRenderer{ .texture = allTextures[6],  .enabled = false, .uiElement = true });
//		ecs::AddComponent(quitGameButton, HedgehogComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[7], .unselectedTexture = allTextures[6], .operation = PauseSystem::OnQuitGamePressed });
//
//
//	}
//
//	void AddButton(ecs::Entity entity, Vector3 pos, Texture selectedTexture, Texture unselectedTexture, Vector3 scaleNormal, Vector3 scaleSelected)
//	{
//		ecs::AddComponent(fullscreenEntity, Transform{ .position = Vector3(0.7,-0.7f,-0.1f), .scale = Vector3(0.25f) });
//		ecs::AddComponent(fullscreenEntity, SpriteRenderer{ .texture = allTextures[12],  .enabled = false, .uiElement = true });
//		ecs::AddComponent(fullscreenEntity, HedgehogComponent{ .upper = mainMenuButton, .lower = resumeButton, .selectedTexture = allTextures[12], .unselectedTexture = allTextures[13], .operation = PauseSystem::OnQuitGamePressed });
//
//	}
//	void MoveUpper()
//	{
//	
//
//	}
//	void MoveLower()
//	{
//		
//	}
//	
//	ecs::Entity GetCurrentSelection()
//	{
//		return currentSelection;
//	}
//	
//	static  void OnMainMenuPressed()
//	{
//		printf("OnMainMenuPressed()\n");
//	}
//	void Selected() {
//
//		HedgehogComponent& hedgehogComponent = ecs::GetComponent<HedgehogComponent>(currentSelection);
//		SpriteRenderer& hedgehogSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
//		hedgehogSpriteRenderer.texture = hedgehogComponent.selectedTexture;
//		hedgehogComponent.operation();
//
//	}
//
//	void ToggleShowUIMenu()
//	{
//
//		printf("menu UI\n");
//		for (auto itr = entities.begin(); itr != entities.end();)
//		{
//			//Get the entity and increment the iterator
//			ecs::Entity entity = *itr++;
//			bool& isOptionsMenu = ecs::GetComponent<HedgehogComponent>(entity).isOptionsMenu;
//			bool& enabled = ecs::GetComponent<SpriteRenderer>(entity).enabled;
//			if (!isOptionsMenu)
//			{
//
//
//				enabled = !enabled;
//			}
//			else
//			{
//				enabled = false;
//			}
//
//
//
//		}
//		currentSelection = resumeButton;
//
//		
//	}
//	
//
//	
//
//	void MoveSliderRight()
//	{
//		
//	}
//	void MoveSliderLeft()
//	{
//
//
//	}
//	void ToggleShowUIOptionsMenu()
//	{
//
//		printf("options menu UI\n");
//
//
//		PauseComponent& pauseComponentOld = ecs::GetComponent<PauseComponent>(currentSelection);
//		SpriteRenderer& selectedSpriteRendererOld = ecs::GetComponent<SpriteRenderer>(currentSelection);
//		Transform& selectedSpriteTransformOld = ecs::GetComponent<Transform>(currentSelection);
//		selectedSpriteTransformOld.scale = Vector3(0.25f);
//		selectedSpriteRendererOld.texture = pauseComponentOld.unselectedTexture;
//
//		currentSelection = optionsResumeButton;
//
//		for (auto itr = entities.begin(); itr != entities.end();)
//		{
//			//Get the entity and increment the iterator
//			ecs::Entity entity = *itr++;
//			bool& isOptionsMenu = ecs::GetComponent<PauseComponent>(entity).isOptionsMenu;
//
//			bool& enabled = ecs::GetComponent<SpriteRenderer>(entity).enabled;
//			PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(entity);
//			SpriteRenderer& spriteRenderer = ecs::GetComponent<SpriteRenderer>(entity);
//			spriteRenderer.texture = pauseComponent.unselectedTexture;
//
//			enabled = !enabled;
//
//
//
//		}
//
//		PauseComponent& pauseComponent = ecs::GetComponent<PauseComponent>(currentSelection);
//		SpriteRenderer& selectedSpriteRenderer = ecs::GetComponent<SpriteRenderer>(currentSelection);
//		Transform& selectedSpriteTransform = ecs::GetComponent<Transform>(currentSelection);
//		selectedSpriteTransform.scale = Vector3(0.32f);
//		selectedSpriteRenderer.texture = pauseComponent.selectedTexture;
//
//	}
//
//};
//GLFWwindow* ShipSystem::window = window;
//
//
////enum class GameStates 
////{
////      GamePlay,MainMenu,Pause,Credits,Quit
////};
////enum class SelectionsInMenu {
////    Start,
////    Options,
////    Exit
////};
////enum class SelectionsInQuit {
////    Yes,
////    No
////};
////enum class SelectionsInPause {
////    Continue, MainMenu, Options
////};
////
////class GameStateMachine {
////public:
////    // Constructor with initial state
////    GameStateMachine() : currentGameState(GameStates::MainMenu) {}
////
////    // Method to transition to the next state
////    void nextState() {
////        switch (currentGameState) {
////        case GameStates::MainMenu:
////            std::cout << "Show main menu options\n";
////             SelectionsInMenu::Start;
////             SelectionsInMenu::Options;
////             SelectionsInMenu::Exit;
////            break;
////
////        case GameStates::GamePlay:
////            std::cout << "Run game\n";           
////            break;
////
////        case GameStates::Pause:
////            std::cout << "Show pause menu\n";
////
////            currentGameState = SelectionsInPause::Start;
////            break;
////
////        case GameStates::Credits:
////            std::cout << "Transitioning from State2 to State3\n";
////            currentGameState = SelectionsInMenu::Exit;
////            break;
////
////        case GameStates::Quit:
////            std::cout << "Transitioning from State2 to State3\n";
////            currentGameState = SelectionsInMenu::Exit;
////            break;
////        }
////    }
////
////    // Method to perform actions based on the current state
////    void ActionAvailableInThisGameState() {
////        switch (currentGameState) {
////        case GameStates::MainMenu:
////            std::cout << "Show main menu options\n";
////            SelectionsInMenu::Start;
////            SelectionsInMenu::Options;
////            SelectionsInMenu::Exit;
////            break;
////        case SelectionsInMenu::Options:
////            std::cout << "Performing action in State2\n";
////            break;
////        case SelectionsInMenu::Exit:
////            std::cout << "Performing action in State3\n";
////            break;
////        }
////    }
////
////private:
////    GameStates currentGameState;
////    SelectionsInPause SelectionsInPause
////};
////
////int main() {
////    GameStateMachine stateMachine;
////
////    for (int i = 0; i < 3; ++i) {
////        stateMachine.performAction();
////        stateMachine.nextState();
////    }
////
////    return 0;
////}
//
//////USER INTERFACE component
////ECS_REGISTER_COMPONENT(UI, SpriteRenderer, Texture, Transform)
////struct selection : ecs::Component
////{
////	Menu upper;
////	Menu lower;
////	std::function<void()>  callback;
////};
////
////
////
////ECS_REGISTER_SYSTEM(Menu,Transform, SpriteRenderer, selection)
////class Menu : public ecs::System 
////{
////	bool isMenuActive = true;
////	selection oldSelection;
////	selection currentSelection;
////
////public : 
////	void Update() 
////	{
////		if (isMenuActive) 
////		{
////			
////			if (OnUpArrowPressed()) 
////			{
////				currentSelection = currentSelection.upper;
////			}
////			if (OnDownArrowPressed())
////			{
////				currentSelection = currentSelection.lower;
////			}
////			if (OnEnterPressed() && NULL != currentSelection.callback())
////			{
////				currentSelection.callback();
////			}
////		}
////
////	}
////
////	//Draw selected button as selected and draw oldSelection as normal 
////	RendererVisuals()
////	{
////
////	}
////
////};
