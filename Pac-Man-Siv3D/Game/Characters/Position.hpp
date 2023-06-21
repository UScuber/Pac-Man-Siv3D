#pragma once
#include "../gamelib.hpp"


// direction: 0,1,2,3 = up,left,down,right
// 方向は小さいほうから優先度高め
struct Position {
	Position(const int y, const int x, const Rot r=Rot::U) : y(y), x(x), rot(r){}

	int get_y() const{ return y; }

	int get_x() const{ return x; }

	Rot get_r() const{ return rot; }

	int round_y() const{ return round(y); }

	int round_x() const{ return round(x); }

	bool is_stop() const{ return Stop; }

	State get_state() const{ return state; }

	// 方向をrにセット
	void rotate(const Rot r){
		assert(r.isvalid());
		rot = r;
	}

	void reverse(){ rot = (rot+2) % 4; }

	// 最大速度のt%の速度に設定
	void set_speed(const int t){ spd = t * frame_move; }

	void start(){ Stop = false; }

	void stop(){ Stop = true; }

	void set_state(const State t){ state = t; }

	void check_warp(){
		// (y,x) = (14, -2), (14, width + 2)
		if(y == 14*size){
			if(x < -2*size) x = (width+2)*size - (-2*size - x);
			else if(x > (width+2)*size) x = -2*size + (x - (width+2)*size);
		}
	}

	// 壁に当たった場合、残りの移動量を返す
	int move_calc_rem(){
		if(is_stop()) return 0;
		const int ry = round(y);
		const int rx = round(x);
		const int ty = (ry*size - y) * dy[rot];
		const int tx = (rx*size - x) * dx[rot];

		// 現在の方向と同じだった場合
		if(0 <= ty && ty < spd && 0 <= tx && tx < spd){
			const int res = ty + tx; // 動く量
			y += dy[rot] * res;
			x += dx[rot] * res;
			//if(get_field_val(ry+dy[rot], rx+dx[rot]) == FieldState::Wall) return 0;
			return spd - res; // 残りの量
		}else{
			check_warp();
			y += dy[rot] * spd;
			x += dx[rot] * spd;
			return 0;
		}
		return 0;
	}

	// thisとaとの距離
	int dist(const Position &a) const{
		const int ay = a.round_y(), ax = a.round_x();
		const int ty = round_y(), tx = round_x();
		return (ay-ty)*(ay-ty) + (ax-tx)*(ax-tx);
	}

	// thisと(y*size,x*size)との距離
	int dist(const int sy, const int sx) const{
		const int ty = round_y(), tx = round_x();
		return (sy-ty)*(sy-ty) + (sx-tx)*(sx-tx);
	}

	bool isopposite(const int r) const{ return (rot + 2) % 4 == r; }

	bool ison_block() const{ return !(y % size || x % size); }

	// ワープする所の通路にいるかどうか
	bool is_intunnel() const{
		if(y != size*14) return false;
		const int d = abs(size*14 - x); // フィールドの中心からのx軸方向の距離
		return size*9 <= d;
	}

protected:
	int y,x; Rot rot;
	int spd = normal_spd;
	State state = State::Normal;
	bool Stop = true;
};
