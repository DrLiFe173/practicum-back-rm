#pragma once

#include "model.h"

class Game_Handler {
public:
	Game_Handler() = default;

	/*static model::Game& GetGameInstance() {
		return game_;
	}*/

//private:
	static model::Game game_;
};