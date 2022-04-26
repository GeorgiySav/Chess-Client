#pragma once

#include <cstdio>
#include <vector>
#include <stack>
#include <iostream>
#include <map>
#include <string>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <initializer_list>
#include <sstream>
#include <math.h>

#include "Piece.h"
#include "Codec_Moves.h"

template <typename T>
bool is_in(const T& v, std::initializer_list<T> lst)
{
	return std::find(std::begin(lst), std::end(lst), v) != std::end(lst);
}

class Chess_Game
{
protected:

	bool running = true;

	uint8_t turn = WHITE;

	std::map<uint8_t, std::vector<uint8_t>> moves{

	};

	int halfMoves;
	int fullMoves;

	struct {

		bool WQrights = false;
		bool WKrights = false;
		bool BQrights = false;
		bool BKrights = false;

		int blackKingPos;
		int whiteKingPos;

	} kingInfo;

	enum directions {
		UP = -8,
		DOWN = 8,
		RIGHT = 1,
		LEFT = -1,

		DIUR = -7,
		DIUL = -9,
		DIDR = 9,
		DIDL = 7
	};

	int enPassantSquare;
	uint8_t board[64];

	std::vector<unsigned int> moveHistory;

	std::map<uint8_t, std::vector<int>> attackMap{
		{WHITE, std::vector<int>(64)},
		{BLACK, std::vector<int>(64)}
	};

public:

	enum gameStates {
		CHECK,
		CHECKMATE,
		STALEMATE,
		DRAW,
		IN_PROGRESS
	};

	Chess_Game();
	~Chess_Game();

	void resetGame();

	void fenToBoard(std::string fen);
	
	std::string moveToAlgebriac(unsigned int move, uint8_t* bBefore, int check);

	void checkEnPassant(int colour, int position, directions dir);
	int promotePawn(int pieceID) { return colour(pieceID) | QUEEN; }

	bool inCheck(int colour, int pos);

	bool canCastle(uint8_t colour, unsigned int side);

	void makeMove(int prevPos, int curPos, int promotion);
	void undoMove();


	void generatePawnMoves(uint8_t piece);
	void generateKnightMoves(uint8_t piece);
	void generateDiagonalMoves(uint8_t piece);
	void generateOrthogonalMoves(uint8_t piece);
	void generateKingMoves(uint8_t piece, uint8_t otherPiece);
	void generateMoves();
	void filterMoves();

	int BoardPosToIndexPos(std::string boardPos);
	std::string IndexPosToBoardPos(int indexPos);


	uint8_t* getBoardPointer() {
		return board;
	}

	gameStates getGameState() {

		for (auto move : moves) {
			if (isColour(board[move.first], turn) && move.second.size() > 0) {
				return IN_PROGRESS;
			}
		}
		if (inCheck(turn, (turn == BLACK) ? kingInfo.blackKingPos : kingInfo.whiteKingPos))
			return CHECKMATE;
		return STALEMATE;

	}

	bool isCheckmate(uint8_t colour) {
		for (auto move : moves) {
			if (isColour(board[move.first], colour) && move.second.size() > 0) {
				return false;
			}
		}
		if (inCheck(colour, (colour == BLACK) ? kingInfo.blackKingPos : kingInfo.whiteKingPos))
			return true;
		return false;
	}

	uint8_t getTurn() {
		return turn;
	}

	bool isValidMove(uint8_t from, uint8_t to) {
		if (from == to) {
			return false;
		}
		for (auto piece : moves) {
			if (piece.first == from) {
				for (auto move : piece.second) {
					if (move == to) {
						return true;
					}
				}
			}
		}
		return false;
	}

	std::vector<uint8_t>* getMoves(uint8_t index) {
		for (auto& piece : moves) {
			if (piece.first == index)
				return &piece.second;
		}
	}

	std::vector<unsigned int>* getMoveHistory() {
		return &moveHistory;
	}

	unsigned int getLastMove() {
		return moveHistory.back();
	}

	// debug and testing
	int moveGenerationTesting(int depth);
};
