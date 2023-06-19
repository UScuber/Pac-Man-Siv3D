#pragma once
#include <Siv3D.hpp>
#include "../Game/InitField.hpp"


using App = SceneManager<String>;

constexpr int flip_freq = 10;
constexpr int SIZE = 17;
constexpr int adj_x = 58, adj_y = 60;

// images modes
enum Mode {
	NORMAL, EATEN, FRIGHTENED, SCORE, FLASH
};

const Array<String> objects = { U"pacman", U"red", U"blue", U"pink", U"orange" };
const Array<String> direction_name = { U"up", U"left", U"down", U"right" };
const String img_path = U"example/images/";
const String font_path = U"example/emulogic.ttf";

Rot last_pressed_key = Rot::R;

Stopwatch sw;


void read_all_images(){
	for(const auto i : step((int)objects.size())){
		for(const auto j : step((int)direction_name.size())){
			for(const auto k : step(2)){
				String img_name = img_path + U"{}/{}{}.png"_fmt(objects[i], direction_name[j], k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(Mode::NORMAL,i,j,k), img_name);

				img_name = img_path + U"eaten/{}.png"_fmt(direction_name[j]);
				TextureAsset::Register(U"{}{}{}{}"_fmt(Mode::EATEN,i,j,k), img_name);

				img_name = img_path + U"frightened/0{}.png"_fmt(k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(Mode::FRIGHTENED,i,j,k), img_name);

				img_name = img_path + U"eaten/{}00.png"_fmt(1 << (j + 1));
				TextureAsset::Register(U"{}{}{}{}"_fmt(Mode::SCORE,i,j,k), img_name);

				img_name = img_path + U"frightened/1{}.png"_fmt(k);
				TextureAsset::Register(U"{}{}{}{}"_fmt(Mode::FLASH,i,j,k), img_name);
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
	for(const auto i : step(height)){
		for(const auto j : step(width)){
			const int type = Python::get_field_value(i, j);
			const Point pos(j*SIZE + adj_x, i*SIZE + adj_y);
			// small
			if(type == FieldState::dots){
				get_texture_asset(U"small_coin").draw(pos);
			}
			// large
			if(type == FieldState::DOTS){
				get_texture_asset(U"large_coin").draw(pos);
			}
		}
	}
}

void draw_header(const double cur_time){
	static const Font font(20, font_path);
	// time
	font(U"TIME").draw(Arg::topCenter = Point(120, 0));
	font(U"{:.1f}"_fmt(cur_time)).draw(Arg::topCenter = Point(120, 24));
	// mode
	const String mode = Python::get_current_mode() ? U"Chase" : U"Scatter";
	font(U"MODE").draw(Arg::topCenter = Point(290, 0));
	font(mode).draw(Arg::topCenter = Point(290, 24));
	// score
	font(U"SCORE").draw(Arg::topCenter = Point(450, 0));
	font(U"{}"_fmt(Python::get_current_score())).draw(Arg::topCenter = Point(450, 24));
}

void draw_residue(){
	for(const auto i : step(Python::get_remain_num())){
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

	for(const auto i : step((int)objects.size())){
		const int s = (int)Python::get_state(i);
		const int x = Python::get_posx(i);
		const int y = Python::get_posy(i);
		const Rot r = Python::get_rot(i);
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
					get_texture_asset(Mode::SCORE,i,Python::get_eat_num()-1,flip).draw(pos);
					continue;
				}
			}
			get_texture_asset(s,i,r,0).draw(pos);
			continue;
		}
		const int t = (int)(Python::get_limit_time(i) * 4);
		if(t <= 8 && !(t & 1)){
			// frightened mode
			get_texture_asset(Mode::FLASH,i,r,flip).draw(pos);
		}else{
			get_texture_asset(s,i,r,flip).draw(pos);
		}
	}
}

void update_game(){
	static int cnt = 0, flip = 0;
	if(cnt % flip_freq == 0) flip ^= 1;
	cnt++;
	const double cur_time = sw.sF();
	Python::update_frame(cur_time, last_pressed_key);

	draw_images(flip);
}
