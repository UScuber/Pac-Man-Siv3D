﻿#include <Siv3D.hpp> // OpenSiv3D v0.6.6

#include "game.hpp"

using App = SceneManager<String>;


constexpr int flip_freq = 10;
constexpr int SIZE = 17;
constexpr int adj_x = 58, adj_y = 60;

// images modes
enum {
	NORMAL, EATEN, FRIGHTENED, SCORE, FLASH
};

const Array<String> objects = { U"pacman", U"red", U"blue", U"pink", U"orange" };
const Array<String> direction_name = { U"up", U"left", U"down", U"right" };
const String img_path = U"example/images/";

int last_pressed_key = 3;

Stopwatch sw;

void read_all_images(){
	for(int i = 0; i < (int)objects.size(); i++){
		for(int j = 0; j < (int)direction_name.size(); j++){
			for(int k = 0; k < 2; k++){
				String img_name = img_path + U"{}/{}{}.png"_fmt(objects[i], direction_name[j], k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(NORMAL,i,j,k), img_name);

				img_name = img_path + U"eaten/{}.png"_fmt(direction_name[j]);
				TextureAsset::Register(U"{}{}{}{}"_fmt(EATEN,i,j,k), img_name);

				img_name = img_path + U"frightened/0{}.png"_fmt(k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(FRIGHTENED,i,j,k), img_name);

				img_name = img_path + U"eaten/{}00.png"_fmt(1 << (j + 1));
				TextureAsset::Register(U"{}{}{}{}"_fmt(SCORE,i,j,k), img_name);

				img_name = img_path + U"frightened/1{}.png"_fmt(k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(FLASH,i,j,k), img_name);
			}
		}
	}
	TextureAsset::Register(U"small_coin", img_path + U"items/small.png");
	TextureAsset::Register(U"large_coin", img_path + U"items/big.png");
	TextureAsset::Register(U"board", img_path + U"stage.png");
}


Texture get_texture_asset(const String &asset_name){
	return TextureAsset(asset_name);
}
Texture get_texture_asset(const int a, const int b, const int c, const int d){
	return TextureAsset(U"{}{}{}{}"_fmt(a, b, c, d));
}


void draw_coins(){
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			const int type = Python::get_field_value(i, j);
			const Point pos(j*SIZE + adj_x, i*SIZE + adj_y);
			// small
			if(type == 7){
				get_texture_asset(U"small_coin").draw(pos);
			}
			// large
			if(type == 8){
				get_texture_asset(U"large_coin").draw(pos);
			}
		}
	}
}

void draw_header(const double current_time){
	static const Font font(20, U"example/emulogic.ttf");
	const String mode = Python::get_current_mode() ? U"Chase" : U"Scatter";
	// time
	font(U"TIME").drawAt(120, 16);
	font(U"{:.1f}"_fmt(current_time)).drawAt(120, 40);
	// mode
	font(U"MODE").drawAt(290, 16);
	font(mode).drawAt(290, 40);
	// score
	font(U"SCORE").drawAt(450, 16);
	font(U"{}"_fmt(Python::get_current_score())).drawAt(450, 40);
}

void draw_residue(){
	for(int i = 0; i < Python::get_remain_num(); i++){
		get_texture_asset(0,0,3,0).drawAt(Scene::Center().movedBy(-220 + i*30, 300));
	}
}

void draw_images(const int flip){
	static int last_pac_posx = Python::get_posx(0);
	static int last_pac_posy = Python::get_posy(0);

	// stage
	get_texture_asset(U"board").drawAt(Scene::Center().movedBy(-2, 0));
	// cookies
	draw_coins();
	// time,mode,score
	draw_header(current_time);
	// residue
	draw_residue();

	for(const int i : step((int)objects.size())){
		const int s = Python::get_state(i);
		const int x = Python::get_posx(i);
		const int y = Python::get_posy(i);
		const int r = Python::get_rot(i);
		const Point pos(x*SIZE/size + adj_x, y*SIZE/size + adj_y);
		if(i == 0){
			if(last_pac_posx == x && last_pac_posy == y){
				get_texture_asset(s,i,r,0).draw(pos);
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
					get_texture_asset(SCORE,i,Python::get_eat_num()-1,flip).draw(pos);
					continue;
				}
			}
			get_texture_asset(s,i,r,0).draw(pos);
			continue;
		}
		const int t = Python::get_limit_time(i) * 4;
		if(t <= 8 && !(t & 1)){
			// frightened mode
			get_texture_asset(FLASH,i,r,flip).draw(pos);
		}else{
			get_texture_asset(s,i,r,flip).draw(pos);
		}
	}
}

void update_game(){
	static int cnt = 0, flip = 0;
	if(cnt % flip_freq == 0) flip ^= 1;
	cnt++;
	const double current_time = sw.sF();
	Python::update_frame(current_time, last_pressed_key);

	draw_images(flip);
}


struct Button {
	Button(const Point &center_pos, const int w=200, const int h=36) :
		buttonRect(Rect(Arg::center = center_pos, w, h).rounded(4.8)){}

	bool clicked() const{
		return buttonRect.leftClicked();
	}

	void draw(const String &str) const{
		buttonRect.draw(buttonRect.mouseOver() ? mouseOverColor : backgroundColor);
		buttonRect.drawFrame(1, 0, frameColor);
		font(str).drawAt(buttonRect.center(), fontColor);
	}
private:
	const RoundRect buttonRect;
	const Font font{ 12, U"example/emulogic.ttf" };
	static constexpr ColorF mouseOverColor{ 0.9, 0.95, 1.0 };
	static constexpr ColorF backgroundColor{ 1.0 };
	static constexpr ColorF frameColor{ 0.67 };
	static constexpr ColorF fontColor{ 0.25 };
};



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
	const Font font{ 20, U"example/emulogic.ttf" };
	const Font title_font{ 40, U"example/emulogic.ttf" };
};


struct PauseScene : App::Scene {
	PauseScene(const InitData &init) : IScene(init){}
	void update() override{
		if((KeyEscape | KeyEnter).down()){
			Python::start_game();
			changeScene(U"Game", 0s);
		}
		if(continueButton.clicked()){
			Python::start_game();
			changeScene(U"Game", 0.3s);
		}
		if(backToButton.clicked()){
			changeScene(U"Title", 0.5s);
		}
	}
	void draw() const override{
		draw_images(0);
		Rect(Point(0,0), Scene::Size()).draw(Transparency(0.8));
		font(U"Pause").drawAt(Scene::Center());

		backToButton.draw(U"Back To Title");
		continueButton.draw(U"Continue(Esc)");
	}
private:
	const Font font{ 35, U"example/emulogic.ttf" };
	const Button continueButton{ Scene::Center().movedBy(0, 50) };
	const Button backToButton{ Scene::Center().movedBy(0, 100) };
};


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
		Rect{ 0,0, 62,640 }.draw(Palette::Black);
		Rect{ 545,0, 600,640 }.draw(Palette::Black);
	}
private:
	const Font font{ 20, U"example/emulogic.ttf" };
	static constexpr Input arrows[4] = {
		KeyUp, KeyLeft, KeyDown, KeyRight
	};
};


struct FinishScene : App::Scene {
	FinishScene(const InitData &init) : IScene(init){}
	void update() override{
		if(KeyEnter.down() || backToButton.clicked()){
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
		font(U"Time: {:.1f}"_fmt(current_time)).drawAt(Scene::Center().movedBy(0, -100));
		font(U"Score: {}"_fmt(Python::get_current_score())).drawAt(Scene::Center().movedBy(0, -150));
		backToButton.draw(U"Back To Title");
	}
private:
	const Font font{ 25, U"example/emulogic.ttf" };
	const Button backToButton{ Scene::Center().movedBy(0, 100) };
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
