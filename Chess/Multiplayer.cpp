#include "App.h"

void App::runClient()
{
	receptionThread = std::thread(&App::receivePackets, this, &socket);
}

void App::connect(std::string ip, unsigned short port)
{
	if (!connected)
	{
		if (socket.connect(ip, port) != sf::Socket::Done) {
			std::cout << "Could not connect to the server\n";
			popUp.setMessagePopUp("Could not connect to the server", "Ok");
		}
		else {
			connected = true;
			std::cout << "Connected to the server\n";
		}
	}
}

void App::disconnect()
{
	socket.disconnect();
	connected = false;
	std::cout << "Disconnected from the server\n";
}

void App::joinRandomGame()
{
	if (connected)
	{
		sf::Packet packet;
		packet << Message(REQUEST_JOIN_RANDOM_GAME, "");
		sendPacket(packet);
		std::cout << "Requested to join a random game\n";
	}
	else {
		std::cout << "You are not connected to the server\n";
		popUp.setMessagePopUp("You must be connected to the server to join a game", "OK");
	}
}

void App::receivePackets(sf::TcpSocket* socket)
{
	while (true)
	{
		if (socket->receive(lastPacket) == sf::Socket::Done) {

			Message receivedMessage;

			lastPacket >> receivedMessage;

			std::cout << "Received message { \n"
						<< "\tID: " << messageIDStrings[receivedMessage.id]
						<< "\n\tBody: " << receivedMessage.body 
					<< "\n}" << std::endl;

			messageQueue.push(receivedMessage);

		}

		std::this_thread::sleep_for((std::chrono::milliseconds)250);

	}
}

void App::handleMessages(const Message& message)
{

	switch (message.id)
	{
	case (UNDEFINED):

		break;
		
	case (ADDED_TO_GAME_QUEUE):
		popUp.setMessagePopUp("Finding a game...", "Cancel");
		break;

	case (JOINED_GAME):
		processJoinedGame(message.body);
		break;

	case (CHAT):
		processChat(message.body);
		break;

	case (MOVE):
		processMove(message.body);
		break;

	case (REQUEST_UNDO_MOVE):
		processUndoMoveRequest(message.body);
		break;

	}
}

void App::processJoinedGame(const std::string& body)
{
	std::stringstream bodyStream;
	bodyStream.str(body);

	std::string gid, c, s;

	std::getline(bodyStream, gid, ' ');
	std::getline(bodyStream, c, ' ');

	gameID = std::stoi(gid);
	yourSide = std::stoi(c);

	popUp.showMessagePopUp = false;
	createMultiplayerGame();
}

void App::processChat(const std::string& chat)
{
	chatLog.push_back(Chat("Oppenent", chat));
}

void App::processMove(const std::string& move)
{
	std::stringstream moveStream;
	moveStream.str(move);

	std::string from, to, prom;

	std::getline(moveStream, from, ' ');
	std::getline(moveStream, to, ' ');
	std::getline(moveStream, prom, ' ');

	chess.makeMove(std::stoi(from), std::stoi(to), std::stoi(prom));

	chess.generateMoves();

	// procedure to make the algebraic move
	int state = chess.getGameState();
	unsigned int m = chess.getLastMove();
	chess.undoMove();
	pgnMoves.push_back(chess.moveToAlgebriac(m, chess.getBoardPointer(), state));
	chess.makeMove(getMovedFrom(m), getMovedTo(m), getFlag(m));

	if (chess.getGameState() == chess.CHECKMATE) {
		popUp.setMessagePopUp("Checkmate!", "OK");
	}

}

void App::processUndoMoveRequest(const std::string& request)
{

}


void App::sendPacket(sf::Packet& packet)
{
	if (packet.getDataSize() > 0 && socket.send(packet) != sf::Socket::Done) {
		std::cout << "Could not send packet\n";
	}
}
