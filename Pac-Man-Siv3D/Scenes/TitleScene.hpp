#pragma once
#include "guilib.hpp"


struct TitleScene : App::Scene {
	TitleScene(const InitData &init) : IScene(init){
		Python::reset_game();
	}

	void update() override{
		if(KeyEnter.down()){
			changeScene(U"Game", 0s);
		}
	}

	void draw() const override{
		title_font(U"Pac-Man").drawAt(Scene::Center().movedBy(0, -200));
		Circle(Scene::Center(), 50).drawPie(130_deg, 280_deg, Palette::Yellow);
		font(U"Press Enter").drawAt(Scene::Center().movedBy(0, 150));
	}

private:
	const Font font{ 20, font_path };
	const Font title_font{ 40, font_path };
};

