#pragma once
#include <SFML/Graphics.hpp>
#include "Piece.h"

class Chessboard_UI
{

private:
	unsigned int BOARD_SIZE;

	sf::Texture wp;
	sf::Texture wh;
	sf::Texture wb;
	sf::Texture wr;
	sf::Texture wq;
	sf::Texture wk;
	sf::Texture bp;
	sf::Texture bh;
	sf::Texture bb;
	sf::Texture br;
	sf::Texture bq;
	sf::Texture bk;

	std::map<int, sf::Sprite> pieceSprites;

	sf::Texture boardTexture;
	sf::Sprite boardSprite;

	sf::CircleShape possibleMove;
	sf::CircleShape currentSquare;

	float pieceScale = 0.37f;
	float boardScale = 100.f;

	float scale = 1.f;

public:

	Chessboard_UI();
	~Chessboard_UI();

	void innitChessPieces(std::string filePath);
	void innitBoard(sf::Color white, sf::Color black);

	void renderChessboard(sf::RenderWindow* window, uint8_t* board, sf::Vector2f pos, uint8_t draggedPiece, std::vector<uint8_t>* draggedPieceMoves, bool isBlack);
	
	void setChessScale(float scale) {

		boardSprite.setScale(boardScale * scale, boardScale * scale);


		for (auto& piece : pieceSprites) {
			piece.second.setScale(pieceScale * boardSprite.getGlobalBounds().width / 8 / 100, pieceScale * boardSprite.getGlobalBounds().width / 8 / 100);
		}

		possibleMove.setScale(scale, scale);
		currentSquare.setScale(scale, scale);

	}

	sf::Sprite* getBoardSpritePointer() {
		return &boardSprite;
	}

	float getBoardInterval() {
		return boardSprite.getGlobalBounds().width / 8;
	}

};

