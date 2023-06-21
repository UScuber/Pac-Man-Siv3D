#pragma once
#include "PacMan.hpp"


struct Enemy : Position {
	// 食べられた時に戻る場所
	static constexpr int nest_posy = 11, nest_posx = 13;

	Enemy(const int x, const int y, const Rot r, const PacMan &pm, const int n_posy, const int n_posx) :
		Position(x, y, r), pacman(pm), innest_posy(n_posy), innest_posx(n_posx), nest_wait_time(0.5){}

	// パックマンと触れたか
	bool is_touch() const{
		return !(dist(pacman) || get_state() == State::Eaten);
	}

	// 入れないかどうか
	bool check_is_gate(const int py, const int px, const Rot r) const{
		if(get_state() == State::Tonest) return false;
		if(isgate.count({ py, px, r })) return true;
		return false;
	}

	// 方向転換、次に移動すべき回転場所を返す
	void change_direction(const Position &target, Rot dir=Rot::NOP){
		const int move_num = move_calc_rem(); // 動ける量

		if(!ison_block() || move_num <= 0) return;

		const int ry = round_y(), rx = round_x();

		if(get_state() == State::Frightened){
			while(true){
				dir = Random(0, 3);
				const int ny = ry + dy[dir];
				const int nx = rx + dx[dir];

				if(isopposite(dir)) continue;
				if(get_field_val(ny, nx) == FieldState::Wall) continue;
				if(isgate.count({ ny,nx, dir })) continue;
				break;
			}
		}
		// eaten_mode 巣の前まで来たとき
		else if(get_state() == State::Eaten && ry == nest_posy && rx == nest_posx){
			rotate(Rot::D);
			set_state(State::Tonest);
			// 真ん中に半分だけずらす
			adj_posx = size / 2;
			x -= adj_posx;
			rotate(Rot::R);
			return;
		}
		else if(get_state() == State::Tonest && ry == innest_posy && rx == innest_posx){
			rotate(Rot::U);
			set_state(State::Innest);
			cur_wait_time = nest_wait_time;
			return;
		}
		else if(get_state() == State::Innest){
			if(cur_wait_time <= 0 && ry == 14){
				set_state(State::Prepare);
				if(rx == nest_posx) dir = Rot::U;
				else if(rx < nest_posx) dir = Rot::R;
				else dir = Rot::L;
			}else{
				// up
				if(ry == 13) dir = Rot::D;
				// down
				else if(ry == 15) dir = Rot::U;
				else dir = get_r();
			}
		}
		else if(get_state() == State::Prepare && ry == nest_posy && rx == nest_posx){
			set_state(State::Normal);
			// 真ん中から半分だけずらす
			x += adj_posx;
			adj_posx = 0;
		}

		if(!dir.isvalid()){
			int dist = inf;
			for(const auto i : step(4)){
				const int ny = ry + dy[i];
				const int nx = rx + dx[i];

				if(isopposite(i)) continue;
				if(get_field_val(ny, nx) == FieldState::Wall) continue;
				if(check_is_gate(ny, nx, (Rot)i)) continue;
				const int d = target.dist(ny, nx);
				if(dist > d){
					dist = d;
					dir = (Rot)i;
				}
			}
		}

		assert(dir.isvalid());

		// 進行方向がFieldState::Wall
		if(get_field_val(ry+dy[dir], rx+dx[dir]) == FieldState::Wall){
			return;
		}
		rotate(dir);

		// 残りの分を動かす
		y += dy[rot] * move_num;
		x += dx[rot] * move_num;
	}

	// frame更新時に呼び出される
	void update_frame(const double dt){
		cur_wait_time -= dt;
	}

	virtual void move() = 0;

	int adj_posx = 0; // 巣出入りするときのx座標の調整用

protected:
	const PacMan &pacman;
	const int innest_posy, innest_posx;
	const double nest_wait_time;
	double cur_wait_time = 0;
};

struct RedEnemy : Enemy {
	static constexpr int red_posy = 11, red_posx = 13;
	static constexpr int innest_posy = 14, innest_posx = 13;

	RedEnemy(const PacMan &pm) : Enemy(red_posy*size, red_posx*size+size/2, Rot::L, pm, innest_posy, innest_posx){}

	void move() override {
		Position target(-4*size, (width-3)*size); // scatter
		if(get_state() == State::Eaten) // eaten
			target = Position(nest_posy*size, nest_posx*size);
		else if(get_state() == State::Tonest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Innest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Prepare)
			target = Position(nest_posy*size, nest_posx*size);
		else if(chase_mode) // chase
			target = pacman;

		change_direction(target);
	}
};

struct PinkEnemy : Enemy {
	static constexpr int pink_posy = 14, pink_posx = 13;
	static constexpr int innest_posy = 14, innest_posx = 13;
	static constexpr double first_nest_wait_time = 1.0;

	PinkEnemy(const PacMan &pm) : Enemy(pink_posy*size, pink_posx*size, Rot::D, pm, innest_posy, innest_posx){
		cur_wait_time = first_nest_wait_time;
		adj_posx = size / 2;
		set_state(State::Innest);
	}

	void move() override {
		Position target(-4*size, 2*size); // scatter
		if(get_state() == State::Eaten) // eaten
			target = Position(nest_posy*size, nest_posx*size);
		else if(get_state() == State::Tonest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Innest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Prepare)
			target = Position(nest_posy*size, nest_posx*size);
		else if(chase_mode){ // chase
			const int r = pacman.get_r();
			const int py = pacman.round_y() + pty[r];
			const int px = pacman.round_x() + ptx[r];
			target = Position(py*size, px*size);
		}
		change_direction(target);
	}
};

struct BlueEnemy : Enemy {
	static constexpr int blue_posy = 14, blue_posx = 12-1;
	static constexpr int innest_posy = 14, innest_posx = 12-1;
	static constexpr double first_nest_wait_time = 2.0;
	const std::shared_ptr<Enemy> red_enemy;

	BlueEnemy(const PacMan &pm, const std::shared_ptr<Enemy> &red) : Enemy(blue_posy*size, blue_posx*size, Rot::U, pm, innest_posy, innest_posx), red_enemy(red){
		cur_wait_time = first_nest_wait_time;
		adj_posx = size / 2;
		set_state(State::Innest);
	}

	void move() override {
		Position target((height+1)*size, width*size); // scatter
		if(get_state() == State::Eaten) // eaten
			target = Position(nest_posy*size, nest_posx*size);
		else if(get_state() == State::Tonest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Innest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Prepare)
			target = Position(nest_posy*size, nest_posx*size);
		else if(chase_mode){ // chase
			const int r = pacman.get_r();
			const int py = pacman.round_y() + bcy[r];
			const int px = pacman.round_x() + bcx[r];

			const int ty = 2*py - red_enemy->round_y();
			const int tx = 2*px - red_enemy->round_x();
			target = Position(ty*size, tx*size);
		}
		change_direction(target);
	}
};

struct OrangeEnemy : Enemy {
	static constexpr int oran_posy = 14, oran_posx = 15;
	static constexpr int innest_posy = 14, innest_posx = 15;
	static constexpr double first_nest_wait_time = 4.0;

	OrangeEnemy(const PacMan &pm) : Enemy(oran_posy*size, oran_posx*size, Rot::U, pm, innest_posy, innest_posx){
		cur_wait_time = first_nest_wait_time;
		adj_posx = size / 2;
		set_state(State::Innest);
	}

	void move() override {
		static constexpr int max_dist = 8 * 8; // 最大距離の2乗
		Position target((height+1)*size, 0); // scatter
		const int d = dist(pacman);

		if(get_state() == State::Eaten) // eaten
			target = Position(nest_posy*size, nest_posx*size);
		else if(get_state() == State::Tonest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Innest)
			target = Position(innest_posy*size, innest_posx*size);
		else if(get_state() == State::Prepare)
			target = Position(nest_posy*size, (nest_posx+1)*size);
		else if(d >= max_dist && chase_mode) // chase
			target = pacman;

		change_direction(target);
	}
};
