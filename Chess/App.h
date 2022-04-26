#pragma once
#include <queue>
#include <thread>
#include <string.h>

#include "SFML/Network.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "Chess_Game.h"
#include "Chessboard_UI.h"
#include "Message.h"

void textCentred(const char* text);

enum gameOptions
{
	LOCAL,
	AI,
	MULTIPLAYER
};

struct Chat
{
	std::string from;
	std::string text;
};

class App
{

private:

	// User Interface
	unsigned int WINDOW_WIDTH = 1200;
	unsigned int WINDOW_HEIGHT = 1000;

	sf::Vector2f boardPos = {25.f, 40.f};

	float boardScale = 1.f;

	Chessboard_UI board;
	
	sf::RenderWindow window;
	sf::Event e;
	
	sf::Clock deltaTime;

	struct {
		bool showMessagePopUp = false;
		bool showOptionPopUp = false;
		bool showPromotionPopUp = false;
		const char* message = "";
		const char* okButtonText = "OK";
		ImVec2 buttonSize = { 30.0f, 20.0f };
		ImVec2 promButtonSize = { 50.0f, 20.0f };
		const float buttonPadding = -8.0f;
		float buttonSpacing = 5.0f;

		int selectedPromotion;
		uint8_t pawnFrom;
		uint8_t pawnTo;

		void setMessagePopUp(const char* newMessage, const char* okText) {
			showMessagePopUp = true;
			message = newMessage;
			okButtonText = okText;
			

		}

		void renderMessagePopUp() {
			if (showMessagePopUp) {
				ImGui::OpenPopup("PopUp");
				if (ImGui::BeginPopupModal("PopUp", NULL, ImGuiWindowFlags_AlwaysUseWindowPadding | 
														  ImGuiWindowFlags_NoCollapse | 
														  ImGuiWindowFlags_NoResize |
														  ImGuiWindowFlags_NoMove))
				{
					ImGui::Spacing();
					textCentred(message);
					
					ImGui::SetCursorPosX((ImGui::GetWindowContentRegionWidth() - ImGui::CalcTextSize(okButtonText).x) / 2);
					if (ImGui::Button(okButtonText)) {
						showMessagePopUp = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
		}

		
		void setOptionPopUp(const char* newMessage, void(*newOkFunc)(), void(*newNoFunc)()) {
			showOptionPopUp = true;
			message = newMessage;
		}

		void renderOptionPopUp() {
			if (showOptionPopUp) {
				ImGui::OpenPopup("PopUp");
				if (ImGui::BeginPopupModal("PopUp", NULL, ImGuiWindowFlags_AlwaysUseWindowPadding |
														  ImGuiWindowFlags_NoCollapse |
														  ImGuiWindowFlags_NoResize |
														  ImGuiWindowFlags_NoMove))
				{
					ImGui::Spacing();
					textCentred(message);
					ImGui::SetCursorPosX((ImGui::GetWindowContentRegionWidth() - buttonSize.x - buttonSize.x - buttonSpacing) / 2);
					if (ImGui::Button("Yes", buttonSize)) {
						showOptionPopUp = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + buttonPadding + buttonSpacing);
					if (ImGui::Button("No", buttonSize)) {
						showOptionPopUp = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
		}

		
		void setPromotionPopUp(uint8_t from, uint8_t to) {
			showPromotionPopUp = true;
			pawnFrom = from;
			pawnTo = to;
			message = "Promote pawn to:";
		}

		void renderPromotionPopUp() {
			if (showPromotionPopUp) {
				ImGui::OpenPopup("PromotionPopUp");
				if (ImGui::BeginPopupModal("PromotionPopUp", NULL, ImGuiWindowFlags_AlwaysUseWindowPadding |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove))
				{
					textCentred(message);
					if (ImGui::Button("Queen", promButtonSize)) {
						showPromotionPopUp = false;
						selectedPromotion = queenPromotion;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Rook", promButtonSize)) {
						showPromotionPopUp = false;
						selectedPromotion = rookPromotion;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Bishop", promButtonSize)) {
						showPromotionPopUp = false;
						selectedPromotion = bishopPromotion;
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Knight", promButtonSize)) {
						showPromotionPopUp = false;
						selectedPromotion = knightPromotion;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
		}

	} popUp;

	float moveFontScale = 1.25f;
	ImVec2 moveNavButtonSize = { 78.5, 25 };

	std::vector<Chat> chatLog;
	static const int bufferSize = 100;
	char chatBoxBuffer[bufferSize];
	float inputBoxSize = 250.0f;
	ImColor youColour = ImColor(0.0f, 1.0f, 0.0f, 1.0f);
	ImColor otherColour = ImColor(1.0f, 0.0f, 0.0f, 1.0f);
	float chatBoxIndent = 30.0f;
	float chatFontScale = 1.0f;

	sf::Sprite downloadSprite;
	sf::Texture downloadTexture;

	// Game
	unsigned int gamemode;
	unsigned int yourSide;

	Chess_Game chess;
	std::vector<std::string> pgnMoves;

	bool gameOver = false;

	struct {

		int index = -1;
		uint8_t id;
		int newPos;

	} movingPiece;

	
	// Multiplayer
	sf::IpAddress serverIP = sf::IpAddress::getLocalAddress();
	unsigned short port = 6969;

	sf::TcpSocket socket;
	sf::Packet lastPacket;
	
	std::queue<Message> messageQueue;

	std::thread receptionThread;

	bool connected = false;

	int gameID = -1;
	
	// AI

	

public:
	App();
	~App();

	void runApplication();

	void inputs();
	void chessboardInputs();

	void executeMove(uint8_t from, uint8_t to, uint8_t promotion);

	void createLocalGame();
	void createMultiplayerGame();


	// User Interface
	void menuBar();
	
	void chatBox();
	void sendChat();
	
	void render();
	
	// Multiplayer functions 
	void runClient();

	void connect(std::string ip, unsigned short port);
	void disconnect();

	void joinRandomGame();

	void receivePackets(sf::TcpSocket* socket);
	void handleMessages(const Message& message);
	void processJoinedGame(const std::string& body);
	void processChat(const std::string& chat);
	void processMove(const std::string& move);
	void processUndoMoveRequest(const std::string& request);

	void sendPacket(sf::Packet& packet);

};

