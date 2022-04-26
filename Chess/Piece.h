#pragma once
#include <iostream>

#define NONE 0b000
#define PAWN 0b001
#define	KNIGHT 0b010
#define	BISHOP 0b011
#define ROOK 0b100
#define QUEEN 0b101
#define KING 0b111

#define WHITE 0b01000
#define BLACK 0b10000

#define TYPE_MASK 0b00111
#define COLOUR_MASK 0b11000

static unsigned int flipColour(int colour) {
	return ~colour & COLOUR_MASK;
}

static int colour(int pieceID) {
	return pieceID & COLOUR_MASK;
}
	
static bool isColour(int pieceID, int colour) {
	return (pieceID & COLOUR_MASK) == colour;
}

static bool isType(int pieceID, int type) {
	return (pieceID & TYPE_MASK) == type;
}

static int type(int pieceID) {
	return pieceID & TYPE_MASK;
}

static bool isDiagonal(int pieceID) {
	int t = type(pieceID);
	return t == QUEEN || t == BISHOP ? true : false;
}

static bool isOrthogonal(int pieceID) {
	int t = type(pieceID);
	return t == QUEEN || t == ROOK ? true : false;
}