﻿#pragma once
#include "gamelib.hpp"

// フィールドの初期状態
constexpr FieldState first_field_board[height][width] = {
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::DOTS,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::DOTS,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::dots,FieldState::None,FieldState::None,FieldState::None,FieldState::Wall,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::None,FieldState::None,FieldState::dots,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::None,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::DOTS,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::None,FieldState::None,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::DOTS,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::dots,FieldState::Wall },
	{ FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall,FieldState::Wall },
};
