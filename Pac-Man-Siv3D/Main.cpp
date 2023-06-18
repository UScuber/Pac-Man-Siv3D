#include "Game/Game.hpp"
#include "Scenes.hpp"


void Main(){
	Scene::SetBackground(Palette::Black);
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	Window::Resize(600, 640);
	Window::SetTitle(U"Pac-Man");

	App manager;
	manager.add<TitleScene>(U"Title");
	manager.add<GameScene>(U"Game");
	manager.add<PauseScene>(U"Pause");
	manager.add<FinishScene>(U"Finish");
	manager.changeScene(U"Title", 0s);

	// Init
	read_all_images();

	while(System::Update()){
		if(!manager.update()){
			break;
		}
	}
}
