#include "Chess_Game.h"

int count(const std::string& str, char aChar)
{
	int count = 0;
	for (int i = 0; i < str.length(); i++) {
		if (str.at(i) == aChar) {
			count++;

		}

	}

	return count;
}

template <typename T>
int count(const std::vector<T> list, T item) {
	int count = 0;
	for (int i = 0; i < list.size(); i++) {
		if (list[i] == item) {
			count++;
		}
	}
	return count;
}

bool instr(const std::string& str, char aChar)
{
	return strpbrk(str.c_str(), &aChar);
}

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

int Chess_Game::BoardPosToIndexPos(std::string boardPos) {

	if (boardPos.length() != 2 || int(boardPos[0]) - 96 < 1 || int(boardPos[0]) - 96 > 8 || !std::isdigit(boardPos[1])) {
		return -1;
	}

	return 63 - (int(boardPos[1] - '1') * 8) - 7 + int(boardPos[0] - 'a');

}

std::string Chess_Game::IndexPosToBoardPos(int indexPos) {

	if (indexPos < 0 || indexPos > 64)
		return "";

	std::string boardPos = "  ";

	boardPos[0] = char(indexPos % 8 + 97);
	boardPos[1] = char(int( (63 - indexPos) / 8)) + '1';

	return boardPos;

}

void Chess_Game::fenToBoard(std::string fen)
{
	static const std::map<char, int> characters{
		{'P', WHITE | PAWN},
		{'N', WHITE | KNIGHT},
		{'B', WHITE | BISHOP},
		{'R', WHITE | ROOK},
		{'Q', WHITE | QUEEN},
		{'K', WHITE | KING},

		{'p', BLACK | PAWN},
		{'n', BLACK | KNIGHT},
		{'b', BLACK | BISHOP},
		{'r', BLACK | ROOK},
		{'q', BLACK | QUEEN},
		{'k', BLACK | KING},
	};


	std::stringstream fenStream;
	fenStream.str(fen);
	std::string fenBoard, fenTurn, fenCastling, fenEP, fenHalfmove, fenFullmove;

	std::getline(fenStream, fenBoard, ' ');
	std::getline(fenStream, fenTurn, ' ');
	std::getline(fenStream, fenCastling, ' ');
	std::getline(fenStream, fenEP, ' ');
	std::getline(fenStream, fenHalfmove, ' ');
	std::getline(fenStream, fenFullmove, ' ');

	//if (fenStream.fail()) {
	//	std::cout << "Invalid FEN syntax\n";
	//	return;
	//}


	if ((count(fenBoard, '/') != 7) || (count(fenBoard, 'k') != 1) || (count(fenBoard, 'K') != 1)) {
		std::cout << "Invalid number of kings or ranks\n";
		return;
	}

	int bIndex = 0;

	for (int index = 0; index < fenBoard.length(); index++) {
		if (fenBoard[index] == '/') {
			index++;
		}
		if (std::isdigit(fen[index])) {
			if (bIndex % 8 + fen[index] - '0' > 8) {
				std::cout << "Too many pieces or empty spaces in file\n";
				return;
			}
			for (int i = 0; i < fen[index] - '0'; i++) {
				board[bIndex] = NONE;
				bIndex++;
			}
		}
		else if (characters.contains(fen[index])) {
			board[bIndex] = characters.at(fen[index]);
			bIndex++;
		}
		else {
			std::cout << "Invalid character in FEN\n";
			return;
		}
	}

	if (fenTurn != "w" && fenTurn != "b") {
		std::cout << "Invalid colour\n";
		return;
	}

	turn = fenTurn == "w" ? WHITE : BLACK;

	kingInfo.WQrights = false;
	kingInfo.WKrights = false;
	kingInfo.BQrights = false;
	kingInfo.BKrights = false;
	
	if (instr(fenCastling, 'K')) {
		kingInfo.WKrights = true;
	}
	if (instr(fenCastling, 'Q')) {
		kingInfo.WQrights = true;
	}
	if (instr(fenCastling, 'k')) {
		kingInfo.BKrights = true;
	}
	if (instr(fenCastling, 'q')) {
		kingInfo.BQrights = true;
	}

	if (fenEP != "-") {
		if (BoardPosToIndexPos(fenEP) == -1) {
			std::cout << "Invalid board coordinates\n";
			return;
		}
		else {
			enPassantSquare = BoardPosToIndexPos(fenEP);
		}
	}

	if (!is_number(fenHalfmove)) {
		halfMoves = 0;
	}

	halfMoves = atoi(fenHalfmove.c_str());

	if (!is_number(fenFullmove)) {
		fullMoves = 0;
	}

	fullMoves = atoi(fenFullmove.c_str());

}

std::string Chess_Game::moveToAlgebriac(unsigned int move, uint8_t* bBefore, int state)
{

	std::string algebriac = "";
	
	unsigned int movedTo, movedFrom, movedPiece, takenPiece, flag;
	movedTo = getMovedTo(move);
	movedFrom = getMovedFrom(move);
	movedPiece = getMovedPiece(move);
	takenPiece = getTakenPiece(move);
	flag = getFlag(move);

	if (flag == kingCastle) {
		return "O-O";
	}
	else if (flag == queenCastle) {
		return "O-O-O";
	}
	
	if (type(movedPiece) == PAWN) {
		if (takenPiece) {
			if (colour(movedPiece) == WHITE) {
				if (bBefore[movedTo + DIDL] == (WHITE | PAWN) && bBefore[movedTo + DIDR] == (WHITE | PAWN)) {
					algebriac = IndexPosToBoardPos(movedFrom)[0];
				}

			}
			else {
				if (bBefore[movedTo + DIUL] == (BLACK | PAWN) && bBefore[movedTo + DIUR] == (BLACK | PAWN)) {
					algebriac = IndexPosToBoardPos(movedFrom)[0];
				}
			}
		}
	}
	else if (type(movedPiece) == KNIGHT) {
		algebriac = "N";

		for (int offset : {-17, -15, -10, -6, 6, 10, 15, 17}) {
			
			if (movedFrom == movedTo + offset) {
				continue;
			}

			if (movedTo % 8 < 2) {
				if (is_in(offset, { 6, -10 }))
					continue;
				if (movedTo % 8 == 0) {
					if (is_in(offset, { 15, -17 }))
						continue;
				}
			}
			if (movedTo % 8 > 5) {
				if (is_in(offset, { -6, 10 }))
					continue;
				if (movedTo % 8 == 7) {
					if (is_in(offset, { -15, 17 }))
						continue;
				}
			}

			if (movedTo + offset > 63 || movedTo + offset < 0)
				continue;

			if (bBefore[movedTo + offset] == movedPiece) {
				// checks if the piece is in the same file
				if ((movedTo + offset) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "N" + IndexPosToBoardPos(movedFrom);
						break;
					}
					else if (algebriac.length() < 3) {
						algebriac = "N" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "N" + IndexPosToBoardPos(movedFrom);
						break;
					}
					else if (algebriac.length() < 3) {
						algebriac = "N" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

	}
	else if (type(movedPiece) == BISHOP) {
		algebriac = "B";

		for (int d = movedFrom + DIUR; (d % 8) != 0 && d > -1; d += DIUR) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;
			
			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}

		}

		for (int d = movedFrom + DIUL; ((d + 1) % 8) != 0 && d > -1; d += DIUL) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

		for (int d = movedFrom + DIDR; (d % 8) != 0 && d < 64; d += DIDR) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

		for (int d = movedFrom + DIDL; ((d + 1) % 8) != 0 && d < 64; d += DIDL) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "B" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "B" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

	}
	else if (type(movedPiece) == ROOK) {
		algebriac = "R";

		for (int x = movedTo + RIGHT; x < movedTo + 8 - (movedTo % 8); x += RIGHT) {
			if ((bBefore[x] && bBefore[x] != movedPiece) || x == movedFrom)
				break;

			if (bBefore[x] == movedPiece) {
				if ((x % 8) == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		for (int x = movedTo + LEFT; x >= movedTo - (movedTo % 8); x += LEFT) {
			if ((bBefore[x] && bBefore[x] != movedPiece) || x == movedFrom)
				break;

			if (bBefore[x] == movedPiece) {
				if ((x % 8) == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		for (int y = movedTo + UP; y >= 0; y += UP) {
			if ((bBefore[y] && bBefore[y] != movedPiece) || y == movedFrom)
				break;

			if (bBefore[y] == movedPiece) {
				if ((y % 8) == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		for (int y = movedTo + DOWN; y < 64; y += DOWN) {
			if ((bBefore[y] && bBefore[y] != movedPiece) || y == movedFrom)
				break;

			if (bBefore[y] == movedPiece) {
				if ((y % 8) == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "R" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 2) {
						algebriac = "R" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

	}
	else if (type(movedPiece) == QUEEN) {
		algebriac = "Q";

		for (int d = movedFrom + DIUR; (d % 8) != 0 && d > -1; d += DIUR) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}

		}

		for (int d = movedFrom + DIUL; ((d + 1) % 8) != 0 && d > -1; d += DIUL) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

		for (int d = movedFrom + DIDR; (d % 8) != 0 && d < 64; d += DIDR) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

		for (int d = movedFrom + DIDL; ((d + 1) % 8) != 0 && d < 64; d += DIDL) {
			if ((bBefore[d] && bBefore[d] != movedPiece) || d == movedFrom)
				break;

			if (bBefore[d] == movedPiece) {
				if ((d) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}

		for (int x = movedTo + RIGHT; x < movedTo + 8 - (movedTo % 8); x += RIGHT) {
			if ((bBefore[x] && bBefore[x] != movedPiece) || x == movedFrom)
				break;

			if (bBefore[x] == movedPiece) {
				if ((x) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		for (int x = movedTo + LEFT; x >= movedTo - (movedTo % 8); x += LEFT) {
			if ((bBefore[x] && bBefore[x] != movedPiece) || x == movedFrom)
				break;

			if (bBefore[x] == movedPiece) {
				if ((x) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		for (int y = movedTo + UP; y >= 0; y += UP) {
			if ((bBefore[y] && bBefore[y] != movedPiece) || y == movedFrom)
				break;

			if (bBefore[y] == movedPiece) {
				if ((y) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		for (int y = movedTo + DOWN; y < 64; y += DOWN) {
			if ((bBefore[y] && bBefore[y] != movedPiece) || y == movedFrom)
				break;

			if (bBefore[y] == movedPiece) {
				if ((y) % 8 == movedFrom % 8) {
					if (algebriac.length() > 1 && std::isalpha(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[1]);
					}
				}
				else {
					if (algebriac.length() > 1 && std::isdigit(algebriac[1])) {
						algebriac = "Q" + IndexPosToBoardPos(movedFrom);
						goto fin;
					}
					else if (algebriac.length() < 3) {
						algebriac = "Q" + std::string(1, IndexPosToBoardPos(movedFrom)[0]);
					}
				}
			}
		}
		
	}
	else {
		algebriac = "K";
	}
	
fin:
	if (takenPiece)
		algebriac += "x";

	algebriac += IndexPosToBoardPos(movedTo);
	
	switch (flag) {
	case queenPromotion:
		algebriac += "=Q";
		break;
	case rookPromotion:
		algebriac += "=R";
		break;
	case bishopPromotion:
		algebriac += "=B";
		break;
	case knightPromotion:
		algebriac += "=N";
		break;
	}

	if (state == CHECK)
		algebriac += "+";
	else if (state == CHECKMATE)
		algebriac += "#";
		
	return algebriac;
}

int Chess_Game::moveGenerationTesting(int depth) {
	int numPositions = 0;

	if (depth == 0) {
		//sf::sleep(sf::Time(sf::seconds(0.2)));
		if (getTakenPiece(moveHistory[moveHistory.size() - 1]))
			return 1;
		else
			return 1;
	}

	generateMoves();
	std::map<uint8_t, std::vector<uint8_t>> tempMoves = moves;



	for (auto piece : tempMoves) {
		if (isColour(board[piece.first], turn)) {
			for (auto move : piece.second) {
				makeMove(piece.first, move, NULL);
				//window.clear();
				//displayBoard();
				//displayPieces();
				//window.display();
				numPositions += moveGenerationTesting(depth - 1);
				undoMove();
			}
		}
	}

	return numPositions;

}

Chess_Game::Chess_Game()
{

	//fenToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	//generateMoves();
}

Chess_Game::~Chess_Game()
{

}

void Chess_Game::resetGame()
{
	fenToBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	generateMoves();
}


void Chess_Game::makeMove(int prevPos, int curPos, int promotion) {
	enPassantSquare = -1;
	uint8_t pieceID = board[prevPos];

	if (promotion) {
		switch (promotion) {
		case queenPromotion:
			pieceID = colour(pieceID) | QUEEN;
			break;
		case rookPromotion:
			pieceID = colour(pieceID) | ROOK;
			break;
		case bishopPromotion:
			pieceID = colour(pieceID) | BISHOP;
			break;
		case knightPromotion:
			pieceID = colour(pieceID) | KNIGHT;
			break;
		}

		moveHistory.push_back(encodeMove(prevPos, curPos, colour(pieceID) | PAWN, board[curPos], promotion));
	}
	else if (isType(pieceID, KING)) {
		if (curPos == 62 && prevPos == 60) {
			moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, NONE, kingCastle));
			board[61] = WHITE | ROOK;
			board[63] = 0;
		}
		else if (curPos == 58 && prevPos == 60) {
			moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, NONE, queenCastle));
			board[59] = WHITE | ROOK;
			board[56] = 0;
		}
		else if (curPos == 6 && prevPos == 4) {
			moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, NONE, kingCastle));
			board[5] = BLACK | ROOK;
			board[7] = 0;
		}
		else if (curPos == 2 && prevPos == 4) {
			moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, NONE, queenCastle));
			board[3] = BLACK | ROOK;
			board[0] = 0;
		}
		else 
			moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, board[curPos], noFlag));
	}
	else if (isType(pieceID, PAWN) && abs(curPos - prevPos) == 16) {
		enPassantSquare = curPos;
		moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, board[curPos], noFlag));
	}
	else if (isType(pieceID, PAWN) && is_in(curPos - prevPos, { -7, -9, 9, 7 }) && !board[curPos]) {
		moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, flipColour(colour(pieceID)) | PAWN, enPassant));
		board[isColour(board[prevPos], BLACK) ? curPos + UP : curPos + DOWN] = 0;
	}
	else
		moveHistory.push_back(encodeMove(prevPos, curPos, pieceID, board[curPos], noFlag));

	board[curPos] = pieceID;
	board[prevPos] = 0;

	turn = flipColour(turn);

	if (colour(pieceID) == BLACK) {
		fullMoves++;
	}
	halfMoves++;
}

void Chess_Game::undoMove() {
	if (!moveHistory.empty()) {
		unsigned int m = moveHistory[moveHistory.size() - 1];
		moveHistory.pop_back();
		unsigned int movedTo, movedFrom, movedPiece, takenPiece, flag;
		movedTo = getMovedTo(m);
		movedFrom = getMovedFrom(m);
		movedPiece = getMovedPiece(m);
		takenPiece = getTakenPiece(m);
		flag = getFlag(m);

		board[movedFrom] = movedPiece;

		if (flag == noFlag) {
			board[movedTo] = takenPiece;
		}
		else if (flag == enPassant) {
			board[movedTo] = 0;
			if (isColour(movedPiece, WHITE)) {
				board[movedTo + DOWN] = BLACK | PAWN;
			}
			else {
				board[movedTo + UP] = WHITE | PAWN;
			}
		}
		else if (flag == knightPromotion ||
				 flag == bishopPromotion ||
				 flag == rookPromotion ||
				 flag == queenPromotion) {
			board[movedFrom] = colour(movedPiece) | PAWN;
			board[movedTo] = takenPiece;
		}
		else if (flag == kingCastle) {
			if (isColour(movedPiece, WHITE)) {
				board[60] = WHITE | KING;
				board[63] = WHITE | ROOK;
				board[62] = NONE;
				board[61] = NONE;
			}
			else {
				board[4] = BLACK | KING;
				board[7] = BLACK | ROOK;
				board[6] = NONE;
				board[5] = NONE;
			}
		}
		else if (flag == queenCastle) {
			if (isColour(movedPiece, WHITE)) {
				board[60] = WHITE | KING;
				board[56] = WHITE | ROOK;
				board[59] = NONE;
				board[58] = NONE;
			}
			else {
				board[4] = BLACK | KING;
				board[0] = BLACK | ROOK;
				board[3] = NONE;
				board[2] = NONE;
			}
		}
		turn = flipColour(turn);

	}
}

void Chess_Game::checkEnPassant(int colour, int position, directions dir)
{

	if (isType(board[position + dir], PAWN) && !isColour(board[position + dir], colour) &&
		isType(getMovedPiece(moveHistory[moveHistory.size() - 1]), PAWN) &&
		abs((int)getMovedTo(moveHistory[moveHistory.size() - 1]) - (int)getMovedFrom(moveHistory[moveHistory.size() - 1])) == 16) {
		moves[position].push_back(position + dir - 8 + (colour == BLACK) * 16);
	}

}

void Chess_Game::generatePawnMoves(uint8_t piece) {
	uint8_t c = colour(board[piece]);

	if (isColour(board[piece], WHITE)) {
		if (piece >= 48 && piece <= 55 && !board[piece + UP + UP] && !board[piece + UP]) {
			moves[piece].push_back(piece + UP + UP);
		}


		if (!board[piece + UP])
			moves[piece].push_back(piece + UP);

		if (piece % 8 != 0 && piece + LEFT == enPassantSquare) {
			moves[piece].push_back(piece + DIUL);
			attackMap[c][piece + DIUL]++;
		}
		else if (piece % 8 != 7 && piece + RIGHT == enPassantSquare) {
			moves[piece].push_back(piece + DIUR);
			attackMap[c][piece + DIUR]++;
		}

		if (board[piece + DIUR] && !isColour(colour(board[piece + DIUR]), c) && (piece + 1) % 8 != 0) {
			moves[piece].push_back(piece + DIUR);
		}
		attackMap[c][piece + DIUR]++;

		if (board[piece + DIUL] && !isColour(colour(board[piece + DIUL]), c) && (piece) % 8 != 0) {
			moves[piece].push_back(piece + DIUL);
		}
		attackMap[c][piece + DIUL]++;


	}
	else if (isColour(board[piece], BLACK)) {
		if (piece == 14)
			printf("");

		if (piece >= 8 && piece <= 15 && !board[piece + DOWN + DOWN] && !board[piece + DOWN]) {
			moves[piece].push_back(piece + DOWN + DOWN);
		}

		if (!board[piece + DOWN])
			moves[piece].push_back(piece + DOWN);

		if (piece % 8 != 0 && piece + LEFT == enPassantSquare) {
			moves[piece].push_back(piece + DIDL);
			attackMap[c][piece + DIDL]++;
		}
		else if (piece % 8 != 7 && piece + RIGHT == enPassantSquare) {
			moves[piece].push_back(piece + DIDR);
			attackMap[c][piece + DIDR]++;
		}

		if (board[piece + DIDR] && !isColour(colour(board[piece + DIDR]), c) && (piece + 1) % 8 != 0) {
			moves[piece].push_back(piece + DIDR);
		}
		attackMap[c][piece + DIDR]++;

		if (board[piece + DIDL] && !isColour(colour(board[piece + DIDL]), c) && (piece) % 8 != 0) {
			moves[piece].push_back(piece + DIDL);
		}
		attackMap[c][piece + DIDL]++;
	}
}

void Chess_Game::generateKnightMoves(uint8_t piece) {
	uint8_t c = colour(board[piece]);

	for (int offset : {-17, -15, -10, -6, 6, 10, 15, 17}) {

		if (piece % 8 < 2) {
			if (is_in(offset, { 6, -10 }))
				continue;
			if (piece % 8 == 0) {
				if (is_in(offset, { 15, -17 }))
					continue;
			}
		}
		if (piece % 8 > 5) {
			if (is_in(offset, { -6, 10 }))
				continue;
			if (piece % 8 == 7) {
				if (is_in(offset, { -15, 17 }))
					continue;
			}
		}

		if (piece + offset > 63 || piece + offset < 0)
			continue;

		if (!isColour(board[piece + offset], c)) {
			moves[piece].push_back(piece + offset);
			attackMap[c][piece + offset]++;
		}
	}
}

void Chess_Game::generateDiagonalMoves(uint8_t piece) {
	uint8_t c = colour(board[piece]);

	for (int d = piece + DIUR; (d % 8) != 0 && d > -1; d += DIUR) {
		if (!board[d]) {
			moves[piece].push_back(d);
			attackMap[c][d]++;
		}
		else {
			if (!isColour(colour(board[d]), c)) {
				moves[piece].push_back(d);
				attackMap[c][d]++;
			}
			break;
		}
	}

	for (int d = piece + DIUL; ((d + 1) % 8) != 0 && d > -1; d += DIUL) {
		if (!board[d]) {
			moves[piece].push_back(d);
			attackMap[c][d]++;
		}
		else {
			if (!isColour(colour(board[d]), c)) {
				moves[piece].push_back(d);
				attackMap[c][d]++;
			}
			break;
		}
	}

	for (int d = piece + DIDR; (d % 8) != 0 && d < 64; d += DIDR) {
		if (!board[d]) {
			moves[piece].push_back(d);
			attackMap[c][d]++;
		}
		else {
			if (!isColour(colour(board[d]), c)) {
				moves[piece].push_back(d);
				attackMap[c][d]++;
			}
			break;
		}
	}

	for (int d = piece + DIDL; ((d + 1) % 8) != 0 && d < 64; d += DIDL) {
		if (!board[d]) {
			moves[piece].push_back(d);
			attackMap[c][d]++;
		}
		else {
			if (!isColour(colour(board[d]), c)) {
				moves[piece].push_back(d);
				attackMap[c][d]++;
			}
			break;
		}
	}
}

void Chess_Game::generateOrthogonalMoves(uint8_t piece) {
	uint8_t c = colour(board[piece]);


	for (int x = piece + RIGHT; x < piece + 8 - (piece % 8); x += RIGHT) {
		if (!board[x]) {
			moves[piece].push_back(x);
			attackMap[c][x]++;
		}
		else {
			if (!isColour(colour(board[x]), c)) {
				moves[piece].push_back(x);
				attackMap[c][x]++;
			}
			break;
		}
	}
	for (int x = piece + LEFT; x >= piece - (piece % 8); x += LEFT) {
		if (!board[x]) {
			moves[piece].push_back(x);
			attackMap[c][x]++;
		}
		else {
			if (!isColour(colour(board[x]), c)) {
				moves[piece].push_back(x);
				attackMap[c][x]++;
			}
			break;
		}
	}
	for (int y = piece + UP; y >= 0; y += UP) {
		if (!board[y]) {
			moves[piece].push_back(y);
			attackMap[c][y]++;
		}
		else {
			if (!isColour(colour(board[y]), c)) {
				moves[piece].push_back(y);
				attackMap[c][y]++;
			}
			break;
		}
	}
	for (int y = piece + DOWN; y < 64; y += DOWN) {
		if (!board[y]) {
			moves[piece].push_back(y);
			attackMap[c][y]++;
		}
		else {
			if (!isColour(colour(board[y]), c)) {
				moves[piece].push_back(y);
				attackMap[c][y]++;
			}
			break;
		}
	}
}

void Chess_Game::generateKingMoves(uint8_t piece, uint8_t otherPiece) {
	uint8_t c = colour(board[piece]);

	for (int offset : {DIUL, UP, DIUR, RIGHT, DIDR, DOWN, DIDL, LEFT}) {

		if (piece + offset > 63 || piece + offset < 0)
			continue;

		if (!(piece % 8) && is_in(offset, { 7, -1, -9 }))
			continue;

		if (piece % 8 == 7 && is_in(offset, { 9, -7, 1 }))
			continue;

		if (isColour(colour(board[piece + offset]), c)) {
			continue;
		}

		if (isType(board[piece + offset], KING) && !isColour(board[piece + offset], c))
			continue;

		makeMove(piece, piece + offset, NULL);

		if (!inCheck(c, piece + offset)) {
			moves[piece].push_back(piece + offset);
		}

		undoMove();
	}

	if (canCastle(c, kingCastle)) {
		moves[piece].push_back(62 - (c == BLACK) * 56);
	}
	if (canCastle(c, queenCastle)) {
		moves[piece].push_back(58 - (c == BLACK) * 56);
	}
}


bool Chess_Game::canCastle(uint8_t c, unsigned int side) {

	if (c == WHITE) {
		if (side == kingCastle) {
			if (!kingInfo.WKrights)
				return false;
		}
		else
			if (!kingInfo.WQrights)
				return false;
	}
	else {
		if (side == kingCastle) {
			if (!kingInfo.BKrights)
				return false;
		}
		else
			if (!kingInfo.BQrights)
				return false;
	}

	int k, r;
	k = 61 - (side == queenCastle) * 3 - (c == BLACK) * 56;
	r = 63 - (side == queenCastle) * 7 - (c == BLACK) * 56;

	if (inCheck(c, 60 - (c == BLACK) * 56) || 
		board[k] || board[k + 1] ||
		attackMap[flipColour(c)][k] || 
		attackMap[flipColour(c)][k + 1] ||
		(board[60 - (c == BLACK) * 56] != (c | KING)) ||
		(board[r] != (c | ROOK))) {
		return false;
	}

	if (side == queenCastle && board[k - 1])
		return false;

	for (auto move : moveHistory) {
		if ((getMovedPiece(move) == (c | ROOK) && getMovedFrom(move) == r) ||
			(getMovedPiece(move) == (c | KING))) {
			return false;
			break;
		}
	}

	return true;

}


void Chess_Game::generateMoves()
{
	moves.clear();

	attackMap.clear();
	attackMap.insert(std::pair(WHITE, std::vector<int>(64)));
	attackMap.insert(std::pair(BLACK, std::vector<int>(64)));

	uint8_t piece = 0;
	int c;
	while (piece < 64) {
		while (!board[piece])
			piece++;
		if (piece > 63)
			break;

		c = colour(board[piece]);

		if (isType(board[piece], KING) && isColour(board[piece], WHITE)) {
			kingInfo.whiteKingPos = piece;
			piece++;
			continue;
		}

		if (isType(board[piece], KING) && isColour(board[piece], BLACK)) {
			kingInfo.blackKingPos = piece;
			piece++;
			continue;
		}


		if (isType(board[piece], KNIGHT)) {
			moves.insert({ piece, {} });

			generateKnightMoves(piece);

			piece++;
			continue;
		}

		if (isType(board[piece], PAWN)) {
			if (!moves.contains(piece));
			moves.insert({ piece, {} });

			generatePawnMoves(piece);

			piece++;
			continue;
		}

		if (isOrthogonal(board[piece])) {
			moves.insert({ piece, {} });
			generateOrthogonalMoves(piece);
		}

		if (isDiagonal(board[piece])) {
			if (!moves.contains(piece))
				moves.insert({ piece, {} });
			generateDiagonalMoves(piece);
		}

		piece++;
	}

	filterMoves();


	moves.insert({ piece, {} });

	generateKingMoves(kingInfo.whiteKingPos, kingInfo.blackKingPos);


	moves.insert({ piece, {} });

	generateKingMoves(kingInfo.blackKingPos, kingInfo.whiteKingPos);

}

bool Chess_Game::inCheck(int colour, int pos) {
	//check all spots where the king could be attacked from
	if (colour == WHITE) {
		if (board[pos + DIUL] == uint8_t(BLACK | PAWN) || board[pos + DIUR] == uint8_t(BLACK | PAWN))
			return true;
	}
	if (colour == BLACK) {
		if (board[pos + DIDL] == uint8_t(WHITE | PAWN) || board[pos + DIDR] == uint8_t(WHITE | PAWN))
			return true;
	}

	for (int offset : {-17, -15, -10, -6, 6, 10, 15, 17}) {
		if (pos + offset > 63 || pos + offset < 0)
			continue;

		if (pos % 8 < 2) {
			if (is_in(offset, { 6, -10 }))
				continue;
			if (pos % 8 == 0) {
				if (is_in(offset, { 15, -17 }))
					continue;
			}
		}
		if (pos % 8 > 5) {
			if (is_in(offset, { -6, 10 }))
				continue;
			if (pos % 8 == 7) {
				if (is_in(offset, { -15, 17 }))
					continue;
			}
		}

		if (isType(board[pos + offset], KNIGHT) && !isColour(board[pos + offset], colour))
			return true;
	}

	for (int x = pos + RIGHT; x < pos + 8 - (pos % 8); x += RIGHT) {
		if (!board[x])
			continue;
		if (isColour(board[x], colour))
			break;
		if (isOrthogonal(board[x]))
			return true;
		else
			break;
	}
	for (int x = pos + LEFT; x >= pos - (pos % 8); x += LEFT) {
		if (!board[x])
			continue;
		if (isColour(board[x], colour))
			break;
		if (isOrthogonal(board[x]))
			return true;
		else
			break;
	}
	for (int y = pos + UP; y >= 0; y += UP) {
		if (!board[y])
			continue;
		if (isColour(board[y], colour))
			break;
		if (isOrthogonal(board[y]))
			return true;
		else
			break;
	}
	for (int y = pos + DOWN; y < 64; y += DOWN) {
		if (!board[y])
			continue;
		if (isColour(board[y], colour))
			break;
		if (isOrthogonal(board[y]))
			return true;
		else
			break;
	}


	for (int d = pos + DIUR; (d % 8) != 0 && d > -1; d += DIUR) {
		if (!board[d])
			continue;
		if (isColour(board[d], colour))
			break;
		if (isDiagonal(board[d]))
			return true;
		else
			break;
	}

	for (int d = pos + DIUL; ((d + 1) % 8) != 0 && d > -1; d += DIUL) {
		if (!board[d])
			continue;
		if (isColour(board[d], colour))
			break;
		if (isDiagonal(board[d]))
			return true;
		else
			break;
	}

	for (int d = pos + DIDR; (d % 8) != 0 && d < 64; d += DIDR) {
		if (!board[d])
			continue;
		if (isColour(board[d], colour))
			break;
		if (isDiagonal(board[d]))
			return true;
		else
			break;
	}

	for (int d = pos + DIDL; ((d + 1) % 8) != 0 && d < 64; d += DIDL) {
		if (!board[d])
			continue;
		if (isColour(board[d], colour))
			break;
		if (isDiagonal(board[d]))
			return true;
		else
			break;
	}

	return false;
}

void Chess_Game::filterMoves() {

	// check for moves that will cause the king to be in check
	for (auto& piece : moves) {
		if (!isColour(board[piece.first], turn))
			continue;

		for (int i = 0; i < piece.second.size() && i > -1; i++) {
			uint8_t move = piece.second[i];

			makeMove(piece.first, move, NULL);

			if (inCheck(colour(getMovedPiece(moveHistory[moveHistory.size() - 1])), isColour(colour(getMovedPiece(moveHistory[moveHistory.size() - 1])), WHITE) ? kingInfo.whiteKingPos : kingInfo.blackKingPos)) {
				attackMap[colour(getMovedPiece(moveHistory[moveHistory.size() - 1]))][move]--;
				piece.second.erase(piece.second.begin() + i);
				i--;
			}

			undoMove();
		}
	}
}
