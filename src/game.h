#pragma once
#include <array>
#include <string>

#include "renderer.h"

enum class PlayerType { HUMAN, MINIMAX, NONE };
PlayerType GetPlayerType(const std::string& type);

enum class Piece { NONE, LIGHT, DARK };
enum class Directions { N, NE, E, SE, S, SW, W, NW };
constexpr std::array<Directions, 8> Dirs{Directions::N, Directions::NE, Directions::E, Directions::SE, Directions::S, Directions::SW, Directions::W, Directions::NW};
struct Board {
	Piece pieces[4][4] = {
		{ Piece::NONE, Piece::NONE, Piece::NONE, Piece::NONE },
		{ Piece::NONE, Piece::DARK, Piece::LIGHT, Piece::NONE },
		{ Piece::NONE, Piece::LIGHT, Piece::DARK, Piece::NONE },
		{ Piece::NONE, Piece::NONE, Piece::NONE, Piece::NONE }
	};
	Board() = default;
	Board(const Board& board, const glm::ivec2& placement, Piece piece);
	bool operator ==(const Board& board);
	bool operator !=(const Board& board);
};

inline bool InBoard(const glm::ivec2& position) {
	return position.x >= 0 && position.x < 4 && position.y >= 0 && position.y < 4;
}

void MoveByDirection(glm::ivec2& position, Directions dir);
void ApplyMove(const glm::ivec2 placement, Piece piece);
std::pair<bool, Board> IsValidMove(const Board& board, const glm::ivec2& placement, Piece piece);

void RenderBoard();
void RenderPieces();

void GameMouseMoveCallback(double x, double y);
void GameMouseButtonCallback(bool pressed);

bool ObtainPlayers(char** args);

void Update();
