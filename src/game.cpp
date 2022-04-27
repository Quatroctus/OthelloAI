#include "game.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

PlayerType GetPlayerType(const std::string& type) {
	if (type == "human") return PlayerType::HUMAN;
	if (type == "minimax") return PlayerType::MINIMAX;
	return PlayerType::NONE;
}


Board::Board(const Board& board, const glm::ivec2& placement, Piece piece) {
	for (int x = 0; x < 4; x++) { // Copy the board over.
		for (int y = 0; y < 4; y++) {
			this->pieces[x][y] = board.pieces[x][y];
		}
	}
	
	if (board.pieces[placement.x][placement.y] == Piece::NONE && InBoard(placement)) {
		for (Directions dir : Dirs) {
			glm::ivec2 loopPos = placement;
			MoveByDirection(loopPos, dir);
			if (board.pieces[loopPos.x][loopPos.y] != (piece == Piece::DARK ? Piece::LIGHT : Piece::DARK)) continue;
			while (InBoard(loopPos) && board.pieces[loopPos.x][loopPos.y] != piece && board.pieces[loopPos.x][loopPos.y] != Piece::NONE) {
				MoveByDirection(loopPos, dir);
			}
			if (InBoard(loopPos)) {
				if (board.pieces[loopPos.x][loopPos.y] == Piece::NONE) continue;
				glm::ivec2 placePos = placement;
				while (placePos != loopPos) {
					this->pieces[placePos.x][placePos.y] = piece;
					MoveByDirection(placePos, dir);
				}
			}
		}
	}
}

bool Board::operator ==(const Board& board) {
	return std::memcmp(this->pieces, board.pieces, sizeof(Board::pieces)) == 0;
}
bool Board::operator !=(const Board& board) {
	return std::memcmp(this->pieces, board.pieces, sizeof(Board::pieces)) != 0;
}

bool NoMoves = false;
PlayerType Player1, Player2;
Piece CurrentPiece = Piece::LIGHT;
Piece OtherPiece = Piece::DARK;
Board OthelloBoard;
int MouseX, MouseY;

uint64_t Utility(const Board& board, Piece piece);

void MoveByDirection(glm::ivec2& position, Directions dir) {
	switch (dir) {
		case Directions::N: position.y--; break;
		case Directions::NE: position.y--; position.x++; break;
		case Directions::E: position.x++; break;
		case Directions::SE: position.y++; position.x++; break;
		case Directions::S: position.y++; break;
		case Directions::SW: position.y++; position.x--; break;
		case Directions::W: position.x--; break;
		case Directions::NW: position.y--; position.x--; break;
	}
}

inline PlayerType GetCurrentPlayer() {
	return CurrentPiece == Piece::LIGHT ? Player1 : Player2;
}

void ApplyMove(const glm::ivec2 placement, Piece piece) {
	OthelloBoard = Board(OthelloBoard, placement, piece);
}

std::pair<bool, Board> IsValidMove(const Board& board, const glm::ivec2& placement, Piece piece) {
	Board b = Board(board, placement, piece);
	return { b != board, b };
}

void RenderBoard() {
	for (int x = -2; x < 2; x++) {
		for (int y = 2; y > -2; y--) {
			RenderQuad({ x, y }, x % 2 == 0 ? (y % 2 == 0 ? Textures::DARK_BOARD : Textures::LIGHT_BOARD) : (y % 2 == 0 ? Textures::LIGHT_BOARD : Textures::DARK_BOARD));
		}
	}
}

void RenderPieces() {
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (OthelloBoard.pieces[x][y] == Piece::NONE) continue;
			RenderQuad({ x - 2, -y + 2 }, OthelloBoard.pieces[x][y] == Piece::LIGHT ? Textures::LIGHT_PIECE : Textures::DARK_PIECE);
		}
	}

	// Render mouse move.
	if (GetCurrentPlayer() == PlayerType::HUMAN && !NoMoves) {
		auto change = IsValidMove(OthelloBoard, { MouseX, MouseY }, CurrentPiece);
		auto& board = change.second;
		if (change.first) {
			for (int x = 0; x < 4; x++) {
				for (int y = 0; y < 4; y++) {
					if (board.pieces[x][y] != OthelloBoard.pieces[x][y]) {
						RenderQuad({ x - 2, -y + 2 }, board.pieces[x][y] == Piece::LIGHT ? Textures::LIGHT_PIECE : Textures::DARK_PIECE, .6f);
					}
				}
			}
		}
	}

	if (NoMoves) { // The game has ended.
		uint64_t p1Score = Utility(OthelloBoard, Piece::LIGHT);
		uint64_t p2Score = Utility(OthelloBoard, Piece::DARK);
		if (p1Score > p2Score) {
			RenderText("Player 1 has won with a score of " + std::to_string(p1Score) + ".", { 0.0f, 200.0f });
		} else if (p2Score > p1Score) {
			RenderText("Player 2 has won with a score of " + std::to_string(p2Score) + ".", { 0.0f, 200.0f });
		} else {
			RenderText("The game has ended in a draw with equal scores of " + std::to_string(p1Score) + ".", { 0.0f, 200.0f });
		}
	}
}

void SwapCurrentPiece() {
	Piece p = CurrentPiece;
	CurrentPiece = OtherPiece;
	OtherPiece = p;
}

void GameMouseMoveCallback(double x, double y) {
	x = (-720.0 / 2.0 + x) + 128.; // Convert x coord from 0->width to -width/2 -> width/2 and then move x=0 to the left of board.
	y =  (480.0 / 2.0 - y) - 128.; // Same as x, but with height and y = 0 to the top of board.
	MouseX = x / 64; // Calculate tile coordinates.
	MouseY = -y / 64;
}

void GameMouseButtonCallback(bool pressed) {
	if (!pressed && !NoMoves && GetCurrentPlayer() == PlayerType::HUMAN) {
		ApplyMove({ MouseX, MouseY }, CurrentPiece);
		SwapCurrentPiece();
	}
}


bool ObtainPlayers(char** args) {
	Player1 = GetPlayerType(args[1]);
	Player2 = GetPlayerType(args[2]);

	if (Player1 == PlayerType::NONE) {
		std::cerr << "Invalid player_type: " << args[1] << ".\n    Valid player_types are: human and minimax." << std::endl;
		return false;
	}
	if (Player2 == PlayerType::NONE) {
		std::cerr << "Invalid player_type: " << args[2] << ".\n    Valid player_types are: human and minimax." << std::endl;
		return false;
	}

	return true;
}

std::vector<Board> Successors(const Board& board, Piece piece) {
	std::vector<Board> successors;
	std::vector<std::pair<bool, Board>> all;
	all.reserve(16);
	successors.reserve(16);
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			all.push_back(IsValidMove(board, {x, y}, piece));
		}
	}
	all.erase(
		std::remove_if(all.begin(), all.end(), [](const std::pair<bool, Board>& p) { return !p.first; }),
		all.end()
	);
	std::transform(all.begin(), all.end(), std::back_inserter(successors), [](const std::pair<bool, Board>& p) { return p.second; });
	return successors;
}

std::pair<Board, uint64_t> MaxValue(const Board& b);
std::pair<Board, uint64_t> MinValue(const Board& b);

bool IsTerminal(const Board& board, const std::vector<Board>& successors, Piece otherPiece) {
	return successors.empty() && Successors(board, otherPiece).empty();
}

uint64_t Utility(const Board& board, Piece piece) {
	uint64_t count = 0;
	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			if (board.pieces[x][y] == piece) count++;
		}
	}
	return count;
}

std::pair<Board, uint64_t> MaxValue(const Board& b) {
	auto successors = Successors(b, CurrentPiece);
	if (IsTerminal(b, successors, OtherPiece) || successors.empty()) {
		return std::pair<Board, uint64_t>{b, Utility(b, CurrentPiece)};
	}
	std::vector<std::pair<Board, uint64_t>> minValues;
	minValues.reserve(successors.size());
	std::transform(successors.begin(), successors.end(), std::back_inserter(minValues), [](const Board& board) {
		return MinValue(board);
	});
	auto maximum = std::max_element(minValues.begin(), minValues.end(), [](const std::pair<Board, uint64_t>& p1, const std::pair<Board, uint64_t>& p2) { return p1.second < p2.second; });
	
	return std::pair<Board, uint64_t>{b, maximum->second};
}

std::pair<Board, uint64_t> MinValue(const Board& b) {
	auto successors = Successors(b, OtherPiece);
	if (IsTerminal(b, successors, CurrentPiece) || successors.empty()) {
		return std::pair<Board, uint64_t>{b, Utility(b, CurrentPiece)};
	}
	std::vector<std::pair<Board, uint64_t>> maxValues;
	maxValues.reserve(successors.size());
	std::transform(successors.begin(), successors.end(), std::back_inserter(maxValues), [](const Board& board) {
		return MaxValue(board);
	});
	auto minimum = std::min_element(maxValues.begin(), maxValues.end(), [](const std::pair<Board, uint64_t>& p1, const std::pair<Board, uint64_t>& p2) { return p1.second < p2.second; });

	return std::pair<Board, uint64_t>{b, minimum->second};
}

Board MiniMaxDecision() {
	auto successors = Successors(OthelloBoard, CurrentPiece);
	if (IsTerminal(OthelloBoard, successors, OtherPiece) || successors.empty()) {
		return OthelloBoard;
	}
	std::vector<std::pair<Board, uint64_t>> minValues;
	minValues.reserve(successors.size());
	std::transform(successors.begin(), successors.end(), std::back_inserter(minValues), [](const Board& board) {
		return MinValue(board);
	});
	auto maximum = std::max_element(minValues.begin(), minValues.end(), [](const std::pair<Board, uint64_t>& p1, const std::pair<Board, uint64_t>& p2) { return p1.second < p2.second; });
	
	return maximum->first;
}

void Update() {
	static Clock::time_point lastTime = Clock::now();
	static double elapsed = 0.0;
	auto currentTime = Clock::now();
	elapsed += (currentTime - lastTime).count() / 1000000000.;
	lastTime = currentTime;
	if (NoMoves) return;
	if (Successors(OthelloBoard, CurrentPiece).empty()) {
		if (Successors(OthelloBoard, OtherPiece).empty())
			NoMoves = true;
		else SwapCurrentPiece();
		return;
	}
	PlayerType currentPlayer = CurrentPiece == Piece::LIGHT ? Player1 : Player2;
	if (currentPlayer == PlayerType::MINIMAX) {
		if (elapsed >= .4) {
			OthelloBoard = MiniMaxDecision();
			SwapCurrentPiece();
			elapsed = 0.;
		}
	} else elapsed = 0.;
}
