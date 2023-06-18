#pragma once
#include "guilib.hpp"
#include "../Game/Game.hpp"


struct GameScene : App::Scene {
	GameScene(const InitData &init) : IScene(init){}

	void update() override{
		if(Python::get_is_game_started() && KeyEscape.down()){
			Python::stop_game();
			sw.pause();
			changeScene(U"Pause", 0s);
			return;
		}
		if(Python::get_is_game_cleared() || (Python::get_remain_num() == 0 && Python::get_is_game_over())){
			Python::stop_game();
			sw.pause();
			System::Sleep(1.0s);
			changeScene(U"Finish", 0s);
			return;
		}
		if(Python::get_is_game_over()){
			Python::stop_game();
			Python::restart_game();
			sw.reset();
			System::Sleep(1.0s);
			changeScene(U"Game", 0s);
			return;
		}
	}

	void draw() const override{
		// Ready
		if(!Python::get_is_game_started()){
			draw_images(0);
			font(U"READY!").drawAt(Scene::Center().movedBy(2, 43), Palette::Yellow);
			for(const int i : { 1,3 }){
				if(arrows[i].down()){
					last_pressed_key = i;
					Python::start_game();
				}
			}
			return;
		}
		sw.start();
		for(int i = 0; i < 4; i++){
			if(arrows[i].down()){
				last_pressed_key = i;
			}
		}
		update_game();

		// background
		Rect(0,0, 62,640).draw(Palette::Black);
		Rect(545,0, 600,640).draw(Palette::Black);
	}

private:
	const Font font{ 20, font_path };
	static constexpr Input arrows[4] = {
		KeyUp, KeyLeft, KeyDown, KeyRight
	};
};
