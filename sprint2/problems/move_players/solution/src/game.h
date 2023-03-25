#pragma once
#include "model.h"

namespace game {
	class Game {
	public:
		explicit Game(model::Game& game)
			: game_{ game } {
		}

		model::Game& GetGameInstance() {
			return game_;
		}

	private:
		model::Game& game_;
	};
} // namespace game