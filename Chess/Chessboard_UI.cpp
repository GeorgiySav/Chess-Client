#include "Chessboard_UI.h"

Chessboard_UI::Chessboard_UI()
{
	innitChessPieces("Images/ChessPiecesArray.png");
	innitBoard(sf::Color(124, 76, 62, 255), sf::Color(81, 42, 42, 255));

	possibleMove = sf::CircleShape(15);
	possibleMove.setFillColor(sf::Color(255, 255, 255, 75));
	possibleMove.setOrigin(15, 15);

	currentSquare = sf::CircleShape(15);
	currentSquare.setFillColor(sf::Color(255, 255, 255, 100));
	currentSquare.setOrigin(15, 15);

}

Chessboard_UI::~Chessboard_UI()
{
	pieceSprites.clear();

}

void Chessboard_UI::innitChessPieces(std::string filePath)
{

	sf::Sprite tsprite;
	tsprite.setScale({ pieceScale, pieceScale });

	wp.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(0, 0, 256, 256));
	tsprite.setTexture(wp);
	tsprite.setOrigin(tsprite.getTexture()->getSize().x * 0.5, tsprite.getTexture()->getSize().y * 0.5);
	pieceSprites.insert({ WHITE | PAWN, tsprite });

	wh.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256, 0, 256, 256));
	tsprite.setTexture(wh);
	pieceSprites.insert({ WHITE | KNIGHT, tsprite });

	wb.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 2, 0, 256, 256));
	tsprite.setTexture(wb);
	pieceSprites.insert({ WHITE | BISHOP, tsprite });

	wr.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 3, 0, 256, 256));
	tsprite.setTexture(wr);
	pieceSprites.insert({ WHITE | ROOK, tsprite });

	wq.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 4, 0, 256, 256));
	tsprite.setTexture(wq);
	pieceSprites.insert({ WHITE | QUEEN, tsprite });

	wk.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 5, 0, 256, 256));
	tsprite.setTexture(wk);
	pieceSprites.insert({ WHITE | KING, tsprite });


	bp.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(0, 256, 256, 256));
	tsprite.setTexture(bp);
	pieceSprites.insert({ BLACK | PAWN, tsprite });

	bh.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256, 256, 256, 256));
	tsprite.setTexture(bh);
	pieceSprites.insert({ BLACK | KNIGHT, tsprite });

	bb.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 2, 256, 256, 256));
	tsprite.setTexture(bb);
	pieceSprites.insert({ BLACK | BISHOP, tsprite });

	br.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 3, 256, 256, 256));
	tsprite.setTexture(br);
	pieceSprites.insert({ BLACK | ROOK, tsprite });

	bq.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 4, 256, 256, 256));
	tsprite.setTexture(bq);
	pieceSprites.insert({ BLACK | QUEEN, tsprite });

	bk.loadFromFile("Images/chess_sprite_sheet.png", sf::IntRect(256 * 5, 256, 256, 256));
	tsprite.setTexture(bk);
	pieceSprites.insert({ BLACK | KING, tsprite });

}

void Chessboard_UI::innitBoard(sf::Color white, sf::Color black)
{
	sf::Image image;
	image.create(2, 2);

	image.setPixel(0, 0, white);
	image.setPixel(0, 1, black);
	image.setPixel(1, 0, black);
	image.setPixel(1, 1, white);

	boardTexture.loadFromImage(image);
	boardTexture.setRepeated(true);

	boardSprite.setTexture(boardTexture);
	boardSprite.setTextureRect(sf::IntRect(0, 0, 8, 8));
	boardSprite.setScale({ 100, 100 });
}

void Chessboard_UI::renderChessboard(sf::RenderWindow* window, uint8_t* board, sf::Vector2f pos, uint8_t draggedPiece, std::vector<uint8_t>* draggedPieceMoves, bool isBlack)
{
	boardSprite.setPosition(pos);
	window->draw(boardSprite);

	float bInterval = getBoardInterval();

	sf::Sprite temp;
	
	if (isBlack) {
		for (int i = 0; i < 64; i++) {
			if (board[i] && i != draggedPiece) {
				temp = pieceSprites[board[i]];
				i = 63 - i;
				temp.setPosition({ (i % 8) * bInterval + pos.x + (bInterval / 2), (i / 8) * bInterval + pos.y + (bInterval / 2) });
				window->draw(temp);
				i = i - (i - (63 - i));
			}
		}
	}
	else {
		for (int i = 63; i > -1; i--) {
			if (board[i] && i != draggedPiece) {
				temp = pieceSprites[board[i]];
				temp.setPosition({ (i % 8) * bInterval + pos.x + (bInterval / 2), (i / 8) * bInterval + pos.y + (bInterval / 2) });
				window->draw(temp);
			}
		}
	}

	if (draggedPiece > -1 && draggedPiece < 64) {
		temp = pieceSprites[board[draggedPiece]];
		temp.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));
		window->draw(temp);

		for (auto move : *draggedPieceMoves) {
			if (isBlack)
				move = 63 - move;
			possibleMove.setPosition({ (move % 8) * bInterval + pos.x + (bInterval / 2), (move / 8) * bInterval + pos.y + (bInterval / 2) });
			window->draw(possibleMove);
		}

		currentSquare.setPosition({ (draggedPiece % 8) * bInterval + pos.x + (bInterval / 2), (draggedPiece / 8) * bInterval + pos.y + (bInterval / 2) });
		window->draw(currentSquare);

	}

}
