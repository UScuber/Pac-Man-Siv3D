#pragma once
#include "guilib.hpp"
#include "Button.hpp"
#include "../Game/Game.hpp"


struct PauseScene : App::Scene {
	PauseScene(const InitData &init) : IScene(init){}

	void update() override{
		if((KeyEscape | KeyEnter).down()){
			Python::start_game();
			changeScene(U"Game", 0s);
		}
		if(continueButton.clicked()){
			Python::start_game();
			changeScene(U"Game", 0.5s);
		}
		if(backToButton.clicked()){
			changeScene(U"Title", 0.5s);
		}
	}

	void draw() const override{
		draw_images(0);
		Rect(Point(0,0), Scene::Size()).draw(Transparency(0.8));
		font(U"Pause").drawAt(Scene::Center().movedBy(0, -50));

		backToButton.draw(U"Back To Title");
		continueButton.draw(U"Continue(Esc)");
	}

private:
	const Font font{ 40, font_path };
	const Button continueButton{ Scene::Center().movedBy(0, 50) };
	const Button backToButton{ Scene::Center().movedBy(0, 100) };
};
