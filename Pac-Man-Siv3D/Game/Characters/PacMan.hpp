#pragma once
#include "Position.hpp"


struct PacMan : Position {
	static constexpr int pac_pos_y = 23*size, pac_pos_x = 13*size+size/2;
	static constexpr int corner_cut = frame_move * 200;

	PacMan() : Position(pac_pos_y, pac_pos_x, Rot::L){}

	// 方向転換、次に移動すべき回転場所を返す
	void change_direction(Rot dir){
		int move_num = move_calc_rem(); // 動ける量

		if(!ison_block()) return;

		const int ry = round_y(), rx = round_x();
		// pac-manからの方向の入力を確かめる
		// 入力なしの場合
		if(!dir.isvalid()) dir = rot;

		const int ny = ry + dy[dir];
		const int nx = rx + dx[dir];
		if(get_field_val(ny, nx) == FieldState::Wall){
			if(get_field_val(ry+dy[rot], rx+dx[rot]) != FieldState::Wall) dir = rot;
			else return;
		}
		if(dir == 2 && ny == 12 && (nx==13 || nx==14)){ // 敵の出入り口
			if(dir == rot) return;
			else dir = rot;
		}
		if(dir != rot) move_num += corner_cut;

		// 進行方向がFieldState::Wall
		if(get_field_val(ry+dy[dir], rx+dx[dir]) == FieldState::Wall){
			return;
		}
		rotate(dir);

		// 残りの分を動かす
		y += dy[rot] * move_num;
		x += dx[rot] * move_num;
	}
};
