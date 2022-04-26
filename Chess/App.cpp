#include "App.h"

App::App() : chess(Chess_Game()),
board(Chessboard_UI()),
window(sf::RenderWindow(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Chess"))
{
	auto image = std::vector<sf::Uint8>{ 122,  97, 116, 255,    140, 114, 132, 255,    155, 159, 181, 255,
										 177, 183, 209, 255,    209, 177, 200, 255,    177, 183, 209, 255,
										 155, 159, 181, 255,    140, 114, 132, 255,    122,  97, 116, 255, };
	auto size = sf::Vector2u{ 3, 3 };
	window.setIcon(size.x, size.y, image.data());

	downloadTexture.loadFromFile("Images/download-icon.png");
	downloadSprite.setTexture(downloadTexture);
	
	ImGui::SFML::Init(window);

	board.setChessScale(boardScale);

	yourSide = WHITE;
	gamemode = LOCAL;


}

App::~App()
{
	disconnect();
	ImGui::SFML::Shutdown();
	
	socket.disconnect();
}

void App::runApplication()
{

	
	runClient();

	while (!gameOver) {
		
		while (!messageQueue.empty())
		{
			Message message = messageQueue.front();
			messageQueue.pop();
			handleMessages(message);
		}

		while (window.pollEvent(e))
		{
			ImGui::SFML::ProcessEvent(e);
			inputs();
		}
		ImGui::SFML::Update(window, deltaTime.restart());
		
		menuBar();


		popUp.renderMessagePopUp();
		popUp.renderOptionPopUp();
		popUp.renderPromotionPopUp();

		if (popUp.selectedPromotion) {
			executeMove(popUp.pawnFrom, popUp.pawnTo, popUp.selectedPromotion);
		}

		if (popUp.message == "Finding a game..." && !popUp.showMessagePopUp) {
			sf::Packet packet;
			packet << Message(LEAVE_GAME_QUEUE, "");
			sendPacket(packet);
			popUp.message = "";
		}

		chatBox();


		render();

	}

}

void App::inputs()
{

	static sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	mousePos = sf::Mouse::getPosition(window);
	sf::Vector2i relPos;

	if (e.type == sf::Event::Closed)
		window.close();

	if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == sf::Keyboard::Backspace) {
			if (pgnMoves.size() > 0) {
				chess.undoMove();
				pgnMoves.pop_back();
				chess.generateMoves();

				if (gamemode == LOCAL) {
					yourSide = chess.getTurn();
				}
			}
		}
	}

	if (e.type == sf::Event::MouseButtonPressed) {

		if (e.key.code == sf::Mouse::Left) {

			if (chess.getGameState() != chess.CHECKMATE && board.getBoardSpritePointer()->getGlobalBounds().contains(mousePos.x, mousePos.y)) {
				relPos = mousePos - sf::Vector2i(boardPos);
				uint8_t pieceIndex = abs(( (yourSide == BLACK) * 63 ) - int(floor(relPos.x / board.getBoardInterval()) + (floor(relPos.y / board.getBoardInterval())) * 8));
				uint8_t piece = chess.getBoardPointer()[pieceIndex];

				if (isColour(piece, chess.getTurn()) && colour(piece) == yourSide) {

					movingPiece.id = piece;
					movingPiece.index = pieceIndex;

				}
			}
			
		}
		
	}

	if (e.type == sf::Event::MouseButtonReleased) {
		if (e.key.code == sf::Mouse::Left) {



			if (board.getBoardSpritePointer()->getGlobalBounds().contains(mousePos.x, mousePos.y) && movingPiece.index != -1) {

				relPos = mousePos - sf::Vector2i(boardPos);
				movingPiece.newPos = abs(( (yourSide == BLACK) * 63) - int(floor(relPos.x / board.getBoardInterval()) + (floor(relPos.y / board.getBoardInterval())) * 8));

				if (chess.isValidMove(movingPiece.index, movingPiece.newPos)) {


					if ((chess.getBoardPointer()[movingPiece.index] == (WHITE | PAWN) && (movingPiece.newPos >= 0 && movingPiece.newPos <= 7)) ||
						(chess.getBoardPointer()[movingPiece.index] == (BLACK | PAWN) && (movingPiece.newPos >= 56 && movingPiece.newPos <= 64)))
					{
						popUp.setPromotionPopUp(movingPiece.index, movingPiece.newPos);
						movingPiece.index = -1;
						movingPiece.id = 0;
						return;
					}

					executeMove(movingPiece.index, movingPiece.newPos, 0);
				}
				else {
					movingPiece.index = -1;
				}
			}
			else {
				movingPiece.index = -1;
			}
		}
	}


}

void App::executeMove(uint8_t from, uint8_t to, uint8_t promotion) {
	
	chess.makeMove(from, to, promotion);

	if (promotion)
		popUp.selectedPromotion = 0;

	if (gamemode == LOCAL) {
		yourSide = chess.getTurn();
	}
	if (gamemode == MULTIPLAYER) {
		sf::Packet movePacket;
		movePacket << Message(MOVE, std::to_string(from) + " " + std::to_string(to) + " " + std::to_string(promotion));
		sendPacket(movePacket);
	}



	movingPiece.index = -1;
	movingPiece.id = 0;

	chess.generateMoves();

	// procedure to make the algebraic move
	int state = chess.getGameState();
	unsigned int move = chess.getLastMove();
	chess.undoMove();
	pgnMoves.push_back(chess.moveToAlgebriac(move, chess.getBoardPointer(), state));
	chess.makeMove(getMovedFrom(move), getMovedTo(move), getFlag(move));
	
	if (chess.getGameState() == chess.CHECKMATE) {
		popUp.setMessagePopUp("Checkmate!", "OK");
		gameOver = true;
	}
	else if (chess.getGameState() == chess.STALEMATE) {
		popUp.setMessagePopUp("Stalemate!", "OK");
		gameOver = true;
	}
}

void App::createLocalGame()
{
	gamemode = LOCAL;

	chess.resetGame();
	pgnMoves.clear();
	chatLog.clear();

	gameOver = false;
}

void App::createMultiplayerGame()
{
	gamemode = MULTIPLAYER;

	chess.resetGame();
	pgnMoves.clear();
	chatLog.clear();

	gameOver = false;

}

void App::render()
{
	window.clear();
	board.renderChessboard(&window, chess.getBoardPointer(), boardPos, movingPiece.index, chess.getMoves(movingPiece.index), (yourSide == BLACK));
	ImGui::SFML::Render(window);
	window.display();
}