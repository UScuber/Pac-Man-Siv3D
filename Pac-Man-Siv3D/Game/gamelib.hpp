﻿#pragma once
#include <Siv3D.hpp>


// ブロックの数(縦、横)
constexpr int height = 31;
constexpr int width = 28;

constexpr int size = 6000; // 1blockの大きさ
constexpr int frame_move = 8; // 1frameで動く量(1%)
constexpr int normal_spd = frame_move * 80; // 初期状態の速さ80%

constexpr int enemies_num = 4;

// frightened_modeが終了する何秒[s]前から点滅させるか
constexpr double frightened_limit_time = 9*0.25;
// frightened_modeの制限時間[s]
constexpr double frightened_time = 6 + frightened_limit_time;
// 食べたときに止まるフレーム数
constexpr int eat_cnt = 35;

constexpr int dots_all_num = 244;

constexpr int inf = 100000000;

enum FieldState {
	None, Wall, // 黒、壁
	dots, DOTS
};

// 敵の状態
enum State {
	Normal, // 通常
	Eaten, // 食べられた
	Frightened, // 青色状態
	Tonest, // to nest 巣の中に入るまで(nest -> innestまで)
	Innest, // in nest 入ってから待機する状態
	Prepare // 出るまでの間
};

// rotate
struct Rot {
	constexpr Rot(const uint8 r) : val(r){
		assert(0 <= r && r <= 4);
	}
	constexpr Rot(const int r) : val((uint8)r){
		assert(0 <= r && r <= 4);
	}

	constexpr bool isvalid() const{ return this->val != Rot::NOP; }

	constexpr operator uint8() const{ return this->val; }

	static const Rot U, L, D, R, NOP;

private:
	uint8 val;
};
constexpr Rot Rot::U = 0;
constexpr Rot Rot::L = 1;
constexpr Rot Rot::D = 2;
constexpr Rot Rot::R = 3;
constexpr Rot Rot::NOP = 4;


constexpr int dy[] = { -1,0,1,0 };
constexpr int dx[] = { 0,-1,0,1 };
// pink target
constexpr int pty[] = { -4,0,4,0 };
constexpr int ptx[] = { -4,-4,0,4 };
// blue center target
constexpr int bcy[] = { -2,0,2,0 };
constexpr int bcx[] = { -2,-2,0,2 };


// chase,scatter modeを変える時間[s]
constexpr int time_table[] = {
	7, // when 7[s], changes to chase_mode
	20 +7, // when 20+7[s], changes to scatter_mode
	7 +20+7, // chase
	20 +7+20+7, // scatter
	5 +20+7+20+7, // chase
	20 +5+20+7+20+7, // scatter
	5 +20+5+20+7+20+7, // chase
	inf // won't change to scatter
};


// enemyが入れないところ{y, x, r}
const std::set<std::tuple<int,int,Rot>> isgate{
	{ 12,13,Rot::D }, { 12,14,Rot::D }, //敵の出入り口
	{ 10,12,Rot::U }, { 10,15,Rot::U }, //上
	{ 22,12,Rot::U }, { 22,15,Rot::U }, //下
};

inline constexpr bool isvalid(const int y, const int x) noexcept{
	return 0 <= y && y < height && 0 <= x && x < width;
}

constexpr int round(const int a){
	return (a + size/2) / size;
}

bool chase_mode = false; // 最初はscatter mode
bool gameover = false;
bool game_cleared = false;

int wait_cnt = 0;
int eat_num = 0; // 食べた数
int remain_num = 2; // 残基(現在を除く)
double adjust_time = 0; // frightenedの時の時間を引く
double frightened_start_time = -1;
double current_time = 0;

int dots_remain_num = dots_all_num;


int cur_table_pos = 0;

FieldState field[height][width];


// fieldの値を取得
FieldState get_field_val(const int y, const int x){
	if(y == 14 && (x < 0 || x >= width)) return FieldState::None; // 14はワープのところ
	if(!isvalid(y, x)) return FieldState::Wall;
	return field[y][x];
}

void set_field_val(const int y, const int x, const FieldState t){
	if(isvalid(y, x)) field[y][x] = t;
}
