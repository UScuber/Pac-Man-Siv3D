#pragma once
#include "InitField.hpp"
#include "Characters.hpp"



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
