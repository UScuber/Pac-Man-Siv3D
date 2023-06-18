#pragma once
#include "guilib.hpp"
#include "Button.hpp"
#include "../Game/Game.hpp"


struct FinishScene : App::Scene {
	FinishScene(const InitData &init) : IScene(init){}

	void update() override{
		if(KeyEnter.down() || backToButton.clicked()){
			changeScene(U"Title", 0s);
		}
	}

	void draw() const override{
		static const Point Center = Scene::Center();
		if(Python::get_is_game_cleared()){
			// game clear
			font(U"Game Cleared!!").drawAt(Center);
		}else{
			// game over
			font(U"Game Over").drawAt(Center);
		}
		font(U"Time: {:.1f}"_fmt(current_time)).drawAt(Center.movedBy(0, -100));
		font(U"Score: {}"_fmt(Python::get_current_score())).drawAt(Center.movedBy(0, -150));
		backToButton.draw(U"Back To Title");
	}

private:
	const Font font{ 25, font_path };
	const Button backToButton{ Scene::Center().movedBy(0, 100) };
};
