#pragma once
#include "gamelib.hpp"
#include "InitField.hpp"


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

struct Game {
	Game() : pacman(), score(0){
		enemies[0].reset(new RedEnemy(pacman));
		enemies[1].reset(new BlueEnemy(pacman, enemies[0]));
		enemies[2].reset(new PinkEnemy(pacman));
		enemies[3].reset(new OrangeEnemy(pacman));
		memcpy(field, first_field_board, sizeof(field));
	}

	~Game(){
		for(const auto i : step(enemies_num)){
			enemies[i].reset();
		}
	}

	void start(){
		if(started) return;

		pacman.start();
		for(auto &enem : enemies) enem->start();
		started = true;
	}

	bool is_started() const{
		return started;
	}

	void stop(){
		started = false;
		pacman.stop();
		for(auto &enem : enemies) enem->stop();
	}

	void restart(){
		printf("restart\n");
		this->~Game();
		pacman = PacMan();
		enemies[0].reset(new RedEnemy(pacman));
		enemies[1].reset(new BlueEnemy(pacman, enemies[0]));
		enemies[2].reset(new PinkEnemy(pacman));
		enemies[3].reset(new OrangeEnemy(pacman));

		chase_mode = false;
		cur_table_pos = 0;
		gameover = false;
		game_cleared = false;

		wait_cnt = 0;
		eat_num = 0;
		adjust_time = 0;
		frightened_start_time = -1;
		current_time = 0;
		remain_num--;

		last_y = 0, last_x = 0;
		is_ate_dots = false;
		started = false;
	}

	void reset(){
		restart();
		printf("reset\n");
		memcpy(field, first_field_board, sizeof(field));
		remain_num = 2;
		score = 0;
		dots_remain_num = dots_all_num;
	}

private:
	void change_to_eaten(){
		wait_cnt = eat_cnt;
		pacman.stop();
		for(auto &enem : enemies){
			if(enem->get_state() != State::Eaten){
				enem->stop();
				printf("stopped ");
			}
		}
		eat_num++;
		score += eat_enemy_score << eat_num;
	}

	void change_all_speed(){
		// enemies
		bool c[4] = {};

		if(dots_remain_num <= 10){ // elroy2 dots left
			for(int i = 0; i <= 1; i++){
				enemies[i]->set_speed(85);
				c[i] = true;
			}
		}else if(dots_remain_num <= 20){ // elroy1 dots left
			for(int i = 0; i <= 1; i++){
				enemies[i]->set_speed(80);
				c[i] = true;
			}
		}

		for(int i = 0; i < 4; i++){
			if(enemies[i]->get_state() == State::Eaten) enemies[i]->set_speed(200);
			else if(enemies[i]->is_intunnel()) enemies[i]->set_speed(40);
			else if(enemies[i]->get_state() == State::Frightened) enemies[i]->set_speed(50);
			else if(enemies[i]->get_state() == State::Normal && !c[i]) enemies[i]->set_speed(75);
			else if(enemies[i]->get_state() == State::Tonest) enemies[i]->set_speed(200);
			else if(enemies[i]->get_state() == State::Innest) enemies[i]->set_speed(55);
			else if(enemies[i]->get_state() == State::Prepare) enemies[i]->set_speed(40);
		}

		// pacman
		// frightened_modeの場合
		if(frightened_start_time != -1){
			if(is_ate_dots) pacman.set_speed(79);
			else pacman.set_speed(90);
		}else{
			if(is_ate_dots) pacman.set_speed(71);
			else pacman.set_speed(80);
		}
	}

	bool check_is_touch(){
		for(auto &enem : enemies){
			if(enem->is_touch()){
				if(enem->get_state() == State::Normal){
					gameover = true;
					printf("gameover!\n");
				}
				else if(enem->get_state() == State::Frightened){
					// stop, eaten_modeにする
					enem->stop();
					enem->set_state(State::Eaten);
					change_to_eaten();
				}
				return true;
			}
		}
		return false;
	}

	void set_state_enemies(const State st) const{
		for(auto &enem : enemies){
			if(enem->get_state() != State::Eaten && enem->get_state() < State::Tonest){
				enem->set_state(st);
			}
		}
	}

	void reverse_enemies() const{
		for(auto &enem : enemies){
			if(enem->get_state() != State::Eaten && enem->get_state() < State::Tonest){
				enem->reverse();
			}
		}
	}

	// scatter <=> chase
	void change_scmode(){
		chase_mode ^= true;
		if(chase_mode) printf("changed to chase mode\n");
		else printf("changed to scatter mode\n");
		reverse_enemies();
		cur_table_pos++;
	}

	void start_frightened_mode(){
		set_state_enemies(State::Frightened);
		// adjust_timeを増やして時間を止めるため、adjust_timeを保存
		frightened_start_time = adjust_time;
		eat_num = 0;
		reverse_enemies();
		printf("changed to frightened mode\n");
	}

	void end_frightened_mode(){
		frightened_start_time = -1;
		set_state_enemies(State::Normal);
		eat_num = 0;
		printf("return to normal mode\n");
	}

	void move_all(const Rot r){
		for(auto &enem : enemies){
			enem->move();
		}
		pacman.change_direction(r);
	}

public:
	// Pythonから毎フレーム呼び出される
	// rはキーボードから受け付けた方向
	int update(const double time, const Rot r){
		if(!started){
			current_time = time;
			adjust_time = time;
			return -1;
		}

		const double dt = time - current_time;
		current_time = time;

		if(frightened_start_time != -1){
			adjust_time += dt;
			// frightened_modeが終わったときor全部食べた時
			if(adjust_time - frightened_start_time >= frightened_time){
				end_frightened_mode();
			}
		}
		// 時間になったらモードの変更をする
		else if(time - adjust_time >= time_table[cur_table_pos]){
			change_scmode();
		}

		move_all(r);

		int res = -1;
		const int y = pacman.round_y();
		const int x = pacman.round_x();
		if(last_y != y || last_x != x) is_ate_dots = false;
		last_y = y; last_x = x;
		// dotsを取った時の処理
		const int v = get_field_val(y, x);
		if(v == dots || v == DOTS){
			set_field_val(y, x, FieldState::None);
			res = y*width + x;
			dots_remain_num--;
			is_ate_dots = true;
			if(v == DOTS){
				start_frightened_mode();
				score += DOT_score;
			}else{
				score += dot_score;
			}
			if(dots_remain_num == 0){
				// ステージクリア
				game_cleared = true;
				printf("cleared!!!\n");
			}
		}

		if(wait_cnt){
			wait_cnt--;
			// 時間を停止する
			frightened_start_time += dt;
		}else{
			pacman.start();
			for(auto &enem : enemies){
				enem->start();
			}

			if(frightened_start_time != -1 && eat_num == 4){
				end_frightened_mode();
			}

			if(check_is_touch()) return res;
		}
		change_all_speed();
		// 時間の更新
		for(auto &enem : enemies){
			enem->update_frame(dt);
		}

		return res;
	}

	const Enemy &get_enemy(const int i) const{
		assert(0 <= i && i < enemies_num);
		return *enemies[i];
	}
	const PacMan &get_pacman() const{ return pacman; }
	int score;

private:
	static constexpr int dot_score = 10;
	static constexpr int DOT_score = 50;
	static constexpr int eat_enemy_score = 100; // 200の半分
	//Enemy *enemies[enemies_num];
	std::array<std::shared_ptr<Enemy>, enemies_num> enemies;
	PacMan pacman;
	bool started = false;
	// 最後にパックマンがいたマスの場所
	int last_y = 0, last_x = 0;
	bool is_ate_dots = false;
};

Game game;


namespace Python {

int update_frame(const double time, const Rot r){
	assert(0 <= r && r <= 4);
	return game.update(time, r);
}

void start_game(){
	game.start();
}

void stop_game(){
	game.stop();
}

void restart_game(){
	game.restart();
}

void reset_game(){
	game.reset();
}

bool get_is_game_started(){
	return game.is_started();
}

int get_field_value(const int y, const int x){
	return get_field_val(y, x);
}

// pacman, red,blue,orange,pink = 0,1,2,3,4
// 現在の位置を出力する
int get_posy(const int i){
	if(!i) return game.get_pacman().get_y();
	return game.get_enemy(i-1).get_y();
}

int get_posx(const int i){
	if(!i) return game.get_pacman().get_x();
	return game.get_enemy(i-1).get_x() + game.get_enemy(i-1).adj_posx;
}

Rot get_rot(const int i){
	if(!i) return game.get_pacman().get_r();
	return game.get_enemy(i-1).get_r();
}

State get_state(const int i){
	if(!i) return game.get_pacman().get_state();
	const State st = game.get_enemy(i-1).get_state();
	if(st < State::Tonest) return st;
	if(st == State::Tonest) return State::Eaten;
	return State::Normal;
}

bool get_is_stop(const int i){
	if(!i) return game.get_pacman().is_stop();
	return game.get_enemy(i-1).is_stop();
}

// frightened_modeの制限時間
double get_limit_time(const int i){
	if(!i || game.get_enemy(i-1).get_state() != State::Frightened) return inf;
	return frightened_time - (adjust_time - frightened_start_time);
}

int get_current_score(){
	return game.score;
}

int get_eat_num(){
	return eat_num;
}

int get_remain_num(){
	return remain_num;
}

int get_current_mode(){
	return chase_mode;
}

bool get_is_game_over(){
	return gameover;
}

bool get_is_game_cleared(){
	return game_cleared;
}

} // namespace Python
