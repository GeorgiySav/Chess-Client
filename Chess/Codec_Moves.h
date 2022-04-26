#pragma once

/*
	00 0000 0000 0000 0000 00xx xxxx - moved to
	00 0000 0000 0000 xxxx xx00 0000 - moved from
	00 0000 000x xxxx 0000 0000 0000 - moved piece
	00 00xx xxx0 0000 0000 0000 0000 - taken piece
	xx xx00 0000 0000 0000 0000 0000 - flag
*/


#define noFlag			0b0000
#define kingCastle      0b0001
#define queenCastle     0b0010
#define knightPromotion 0b0011
#define bishopPromotion 0b0100
#define rookPromotion   0b0101
#define queenPromotion  0b0111
#define enPassant       0b1000

#define flagMask	   0b11110000000000000000000000
#define takenPieceMask 0b00001111100000000000000000
#define movedPieceMask 0b00000000011111000000000000
#define movedFromMask  0b00000000000000111111000000
#define movedToMask    0b00000000000000000000111111

static unsigned int encodeMove(unsigned int movedFrom, unsigned int movedTo,
	unsigned int movedPiece, unsigned int takenPiece,
	unsigned int flag) {
	return (flag << 22) | (takenPiece << 17) | (movedPiece << 12) | (movedFrom << 6) | movedTo;
}

static unsigned int getFlag(unsigned int move) {
	return (move & flagMask) >> 22;
}

static unsigned int getTakenPiece(unsigned int move) {
	return (move & takenPieceMask) >> 17;
}

static unsigned int getMovedPiece(unsigned int move) {
	return (move & movedPieceMask) >> 12;
}

static unsigned int getMovedFrom(unsigned int move) {
	return (move & movedFromMask) >> 6;
}

static unsigned int getMovedTo(unsigned int move) {
	return (move & movedToMask);
}