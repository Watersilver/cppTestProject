#include <array>
#include <vector>
#include <iostream>

#include "Game.h"

int main()
{
	Game& game = Game::getInstance();
  if (game.Construct(game.s_width, game.s_height, 1, 1, true))
		game.Start();
	return 0;
};
