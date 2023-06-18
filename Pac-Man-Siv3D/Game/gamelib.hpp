#pragma once
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

enum {
	None, wall, // 黒、壁
	pac, // Pac-Man
	red, blue, orange, pink, // enemies
	dots, DOTS
};
// 敵の状態
enum State {
	normal, // 通常
	eaten, // 食べられた
	frightened, // 青色状態
	tonest, // to nest 巣の中に入るまで(nest -> innestまで)
	innest, // in nest 入ってから待機する状態
	prepare // 出るまでの間
};
// rotate
enum { U,L,D,R };



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


// フィールドの初期状態
constexpr int first_field_board[height][width] = {
	{ wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall },
	{ wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
	{ wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
	{ wall,DOTS,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,DOTS,wall },
	{ wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
	{ wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
	{ wall,dots,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,dots,wall },
	{ wall,dots,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,dots,wall },
	{ wall,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,None,wall,wall,None,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,None,wall,wall,None,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,None,None,None,None,None,None,None,None,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,wall,wall,wall,None,wall,wall,wall,wall,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,wall,None,wall,None,wall,None,wall,wall,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ None,None,None,None,None,None,dots,None,None,None,wall,None,None,None,None,None,wall,wall,None,None,None,dots,None,None,None,None,None,None },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,wall,None,wall,None,wall,None,wall,wall,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,wall,wall,wall,wall,wall,wall,wall,wall,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,None,None,None,None,None,None,None,None,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,wall,wall,wall,wall,wall,wall,wall,wall,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,wall,wall,wall,wall,wall,dots,wall,wall,None,wall,wall,wall,wall,wall,wall,wall,wall,None,wall,wall,dots,wall,wall,wall,wall,wall,wall },
	{ wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
	{ wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
	{ wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
	{ wall,DOTS,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,None,None,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,DOTS,wall },
	{ wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall },
	{ wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall },
	{ wall,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,wall },
	{ wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall },
	{ wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall },
	{ wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
	{ wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall },
};



// enemyが入れないところ{y, x, r}
const std::set<std::tuple<int,int,int>> isgate{
	{ 12,13,D }, { 12,14,D }, //敵の出入り口
	{ 10,12,U }, { 10,15,U }, //上
	{ 22,12,U }, { 22,15,U }, //下
};

inline constexpr bool isvalid(const int y, const int x) noexcept{
	return 0 <= y && y < height && 0 <= x && x < width;
}

constexpr int round(const int a){
	return (a + size/2) / size;
}

inline unsigned int randxor32() noexcept{
	static unsigned int y = (unsigned int)rand() | (unsigned int)rand() << 16;
	y = y ^ (y << 13); y = y ^ (y >> 17);
	return y = y ^ (y << 5);
}
// returns random [l, r)
inline int rnd(const int l, const int r) noexcept{
	return randxor32() % (r - l) + l;
}
