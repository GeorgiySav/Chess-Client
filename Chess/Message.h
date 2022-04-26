#pragma once
#include <iostream>
#include <vector>
#include "SFML/Network.hpp"

enum messageIDs
{
	UNDEFINED,
	REQUEST_JOIN_RANDOM_GAME,
	LEAVE_GAME_QUEUE,
	ADDED_TO_GAME_QUEUE,
	JOINED_GAME,
	DISCONNECTED_FROM_GAME,
	GAME_ENDED,
	CHAT,
	MOVE,
	REQUEST_UNDO_MOVE,
};

static const std::string messageIDStrings[] = {
	"UNDEFINED",
	"REQUEST_JOIN_RANDOM_GAME",
	"ADDED_TO_GAME_QUEUE",
	"JOINED_GAME",
	"DISCONNECTED_FROM_GAME",
	"GAME_ENDED",
	"CHAT",
	"MOVE",
	"REQUEST_UNDO_MOVE",
};

struct Message
{
	uint8_t id;
	std::string body;

	Message()
	{
		id;
		body;
	}

	Message(uint8_t mh, std::string msg)
	{
		id = mh;
		body = msg;
	}

	size_t size() const {
		return body.size();
	}

	friend std::ostream& operator << (std::ostream& os, const Message& msg)
	{
		os << "ID:" << int(msg.id) << "Size:" << msg.size();
		return os;
	}
};

static sf::Packet& operator << (sf::Packet& packet, const Message& message)
{
	return packet << message.id << message.body;
}

static sf::Packet& operator >> (sf::Packet& packet, Message& message)
{
	return packet >> message.id >> message.body;
}