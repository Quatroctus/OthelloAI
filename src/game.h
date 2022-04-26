#pragma once
#include <string>

#include "renderer.h"

enum class PlayerType { HUMAN, MINIMAX, NONE };
PlayerType GetPlayerType(const std::string& type);


void RenderBoard();
