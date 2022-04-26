#include "game.h"


PlayerType GetPlayerType(const std::string& type) {
	if (type == "human") return PlayerType::HUMAN;
	if (type == "minimax") return PlayerType::MINIMAX;
	return PlayerType::NONE;
}

void RenderBoard() {
	for (int x = -2; x < 2; x++) {
		for (int y = 2; y > -2; y--) {
			RenderQuad({ x, y }, x % 2 == 0 ? (y % 2 == 0 ? Textures::DARK_BOARD : Textures::LIGHT_BOARD) : (y % 2 == 0 ? Textures::LIGHT_BOARD : Textures::DARK_BOARD));
		}
	}
	RenderQuad({ 0, 0 }, Textures::LIGHT_PIECE);
	RenderQuad({ 1, 0 }, Textures::LIGHT_PIECE, .5f);

	RenderQuad({ 0, 1 }, Textures::DARK_PIECE);
	RenderQuad({ 1, 1 }, Textures::DARK_PIECE, .5f);
}
