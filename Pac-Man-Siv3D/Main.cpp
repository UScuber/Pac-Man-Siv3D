﻿#include <Siv3D.hpp> // OpenSiv3D v0.6.6

#include "game.hpp"

using App = SceneManager<String>;


constexpr int flip_freq = 10;
constexpr int SIZE = 17;

// images modes
enum {
	NORMAL, EATEN, FRIGHTENED, SCORE, FLASH
};

const Array<String> objects = { U"pacman", U"red", U"blue", U"pink", U"orange" };
const Array<String> direction_name = { U"up", U"left", U"down", U"right" };

int last_pressed_key = 3;


void read_all_images(){
	for(int i = 0; i < (int)objects.size(); i++){
		for(int j = 0; j < (int)direction_name.size(); j++){
			for(int k = 0; k < 2; k++){
				String img_name = U"images/{}/{}{}.png"_fmt(objects[i], direction_name[j], k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(NORMAL,i,j,k), img_name);

				img_name = U"images/eaten/{}.png"_fmt(direction_name[j]);
				TextureAsset::Register(U"{}{}{}{}"_fmt(EATEN,i,j,k), img_name);

				img_name = U"images/frightened/0{}.png"_fmt(k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(FRIGHTENED,i,j,k), img_name);

				img_name = U"images/eaten/{}00.png"_fmt(1 << (j + 1));
				TextureAsset::Register(U"{}{}{}{}"_fmt(SCORE,i,j,k), img_name);

				img_name = U"images/frightened/1{}.png"_fmt(k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(FLASH,i,j,k), img_name);
			}
		}
	}
	TextureAsset::Register(U"small_coin", U"images/items/small.png");
	TextureAsset::Register(U"large_coin", U"images/items/big.png");
	TextureAsset::Register(U"board", U"images/stage.png");
}


void draw_coins(){
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			const int type = Python::get_field_value(i, j);
			Texture coin;
			if(type == 7){
				// small
				coin = TextureAsset(U"small_coin");
			}else if(type == 8){
				// large
				coin = TextureAsset(U"large_coin");
			}else continue;
			coin.draw(j*SIZE + 58, i*SIZE + 60);
		}
	}
}

void draw_header(const double current_time){
	static const Font font(25, Typeface::Medium);
	const String mode = Python::get_current_mode() ? U"Chase" : U"Scatter";
	font(U"{:.1f}"_fmt(current_time)).drawAt(120, 40);
	font(mode).drawAt(290, 40);
	font(U"{}"_fmt(Python::get_current_score())).drawAt(450, 40);
}

void draw_images(const int flip){
	static int last_pac_posx = Python::get_posx(0);
	static int last_pac_posy = Python::get_posy(0);

	TextureAsset(U"board").drawAt(Scene::Center() + Point(-2,0));
	draw_coins();
	draw_header(current_time);

	for(const int i : step((int)objects.size())){
		const int s = Python::get_state(i);
		const int x = Python::get_posx(i);
		const int y = Python::get_posy(i);
		const int r = Python::get_rot(i);
		const Point pos(x*SIZE/size + 58, y*SIZE/size + 60);
		if(i == 0){
			if(last_pac_posx == x && last_pac_posy == y){
				TextureAsset(U"{}{}{}{}"_fmt(s,i,r,0)).draw(pos);
				continue;
			}
			last_pac_posx = x;
			last_pac_posy = y;
		}
		if(Python::get_is_stop(i)){
			if(s == EATEN){
				// eaten
				if((x+size/2)/size == (last_pac_posx+size/2)/size && (y+size/2)/size == (last_pac_posy+size/2)/size){
					assert(Python::get_eat_num());
					TextureAsset(U"{}{}{}{}"_fmt(SCORE,i,Python::get_eat_num()-1,flip)).draw(pos);
					continue;
				}
			}
			TextureAsset(U"{}{}{}{}"_fmt(s,i,r,0)).draw(pos);
			continue;
		}
		const int t = Python::get_limit_time(i) * 4;
		if(t <= 8 && !(t & 1)){
			// frightened mode
			TextureAsset(U"{}{}{}{}"_fmt(FLASH,i,r,flip)).draw(pos);
		}else{
			TextureAsset(U"{}{}{}{}"_fmt(s,i,r,flip)).draw(pos);
		}
	}
}

void update_game(){
	static int cnt = 0, flip = 0;
	static Stopwatch sw(StartImmediately::Yes);
	if(cnt % flip_freq == 0) flip ^= 1;
	cnt++;
	const double current_time = sw.sF();
	const int res = Python::update_frame(current_time, last_pressed_key);

	draw_images(flip);

	if(Python::get_is_game_over()){
		Print << U"Game Over!!!";
	}
}

struct TitleScene : App::Scene {
	TitleScene(const InitData &init) : IScene(init), font(30, Typeface::Medium), title_font(50, Typeface::Medium){
		Python::reset_game();
	}
	void update() override{
		if(KeyEnter.down()){
			changeScene(U"Game", 0s);
		}
	}
	void draw() const override{
		title_font(U"Pac Man").drawAt(Scene::Center() + Point(0,-200));
		Circle(Scene::Center(), 50).drawPie(130_deg, 280_deg, Palette::Yellow);
		font(U"Press Enter").drawAt(Scene::Center() + Point(0,150));
	}
private:
	const Font font, title_font;
};

struct PauseScene : App::Scene {
	PauseScene(const InitData &init) : IScene(init), font(45, Typeface::Bold){}
	void update() override{
		if((KeyEscape | KeyEnter).down()){
			Python::start_game();
			changeScene(U"Game", 0s);
		}
		if(SimpleGUI::ButtonAt(U"Continue (Esc)", Scene::Center() + Point(0,50), 200)){
			Python::start_game();
			changeScene(U"Game", 0.2s);
		}
		if(SimpleGUI::ButtonAt(U"Back To Title", Scene::Center() + Point(0,100), 200)){
			changeScene(U"Title", 0s);
		}
	}
	void draw() const override{
		draw_images(0);
		Rect(Point(0,0), Scene::Size()).draw(Transparency(0.8));
		font(U"Pause").drawAt(Scene::Center());
		SimpleGUI::ButtonAt(U"Continue (Esc)", Scene::Center() + Point(0,50), 200);
		SimpleGUI::ButtonAt(U"Back To Title", Scene::Center() + Point(0,100), 200);
	}
private:
	const Font font;
};

struct GameScene : App::Scene {
	GameScene(const InitData &init) : IScene(init), font(30, Typeface::Medium){}
	void update() override{
		if(Python::get_is_game_started() && KeyEscape.down()){
			Python::stop_game();
			changeScene(U"Pause", 0s);
		}
		if(Python::get_is_game_over() || Python::get_is_game_cleared()){
			Python::stop_game();
			changeScene(U"Finish", 0.2s);
		}
	}
	void draw() const override{
		// Ready
		if(!Python::get_is_game_started()){
			draw_images(0);
			font(U"READY").drawAt(Scene::Center() + Point(2,43));
			for(const int i : { 1,3 }){
				if(arrows[i].down()){
					last_pressed_key = i;
					Python::start_game();
				}
			}
			return;
		}
		for(int i = 0; i < 4; i++){
			if(arrows[i].down()){
				last_pressed_key = i;
			}
		}
		update_game();

		// background
		Rect{ 0,0, 62,640 }.draw(Palette::Black);
		Rect{ 545,0, 600,640 }.draw(Palette::Black);
	}
private:
	const Font font;
	static constexpr decltype(KeyUp) arrows[4] = {
		KeyUp, KeyLeft, KeyDown, KeyRight
	};
};

struct FinishScene : App::Scene {
	FinishScene(const InitData &init) : IScene(init), font(30, Typeface::Medium){}
	void update() override{
		if(KeyEnter.down()){
			changeScene(U"Title", 0s);
		}
		if(SimpleGUI::ButtonAt(U"BackToTitle", Scene::Center() + Point(0,100))){
			changeScene(U"Title", 0s);
		}
	}
	void draw() const override{
		if(Python::get_is_game_cleared()){
			// game clear
			font(U"Game Cleared!!").drawAt(Scene::Center());
		}else{
			// game over
			font(U"Game Over").drawAt(Scene::Center());
		}
		font(U"Time: {:.1f}"_fmt(current_time)).drawAt(Scene::Center() + Point(0,-100));
		SimpleGUI::ButtonAt(U"BackToTitle", Scene::Center() + Point(0,100));
	}
private:
	const Font font;
};

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
	manager.changeScene(U"Title", 0.2s);

	// Init
	read_all_images();

	while(System::Update()){
		if(!manager.update()){
			break;
		}
	}
}
