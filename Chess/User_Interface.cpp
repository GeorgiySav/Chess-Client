#include "App.h"

void textCentred(const char* text)
{
	float win_width = ImGui::GetWindowSize().x;
	float text_width = ImGui::CalcTextSize(text).x;

	// calculate the indentation that centers the text on one line, relative
	// to window left, regardless of the `ImGuiStyleVar_WindowPadding` value
	float text_indentation = (win_width - text_width) * 0.5f;

	// if text is too long to be drawn on one line, `text_indentation` can
	// become too small or even negative, so we check a minimum indentation
	float min_indentation = 20.0f;
	if (text_indentation <= min_indentation) {
		text_indentation = min_indentation;
	}

	ImGui::SameLine(text_indentation);
	ImGui::PushTextWrapPos(win_width - text_indentation);
	ImGui::TextWrapped(text);
	ImGui::PopTextWrapPos();
}

void App::menuBar()
{
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("Profile")) {
			
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Settings")) {
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Play")) {
			
			if (ImGui::BeginMenu("Local")) {
				
				if (ImGui::MenuItem("Pass and Play")) {
					createLocalGame();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Multiplayer")) {

				if (connected) {
					ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected to the server");
					ImGui::SameLine();
					if (ImGui::Button("Disconnect")) {
						disconnect();
					}
				}
				else {
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "Not connected to the server");
					ImGui::SameLine();
					if (ImGui::Button("Connect")) {
						connect(serverIP.toString(), port);
					}
				}
				
				if (ImGui::MenuItem("Host")) {
					//createHostGame();
				}

				if (ImGui::MenuItem("Join")) {
					//createJoinGame();
				}

				if (ImGui::MenuItem("Join Random Game")) {
					joinRandomGame();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("AI")) {
				if (ImGui::MenuItem("Play against the AI")) {
					//createAIGame();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void App::chatBox()
{
	
	static bool open = true;

	if (ImGui::Begin("Stuff", &open, ImGuiWindowFlags_NoTitleBar))
	{
		if (ImGui::ImageButton(downloadTexture, sf::Vector2f(32, 32))) {
			//ImGui::OpenPopup("Download");
		}
			
		//if (ImGui::Button("<<", moveNavButtonSize)) {
		//
		//}
		ImGui::SameLine();
		if (ImGui::Button("<", moveNavButtonSize)) {

		}
		ImGui::SameLine();
		if (ImGui::Button(">", moveNavButtonSize)) {

		}
		ImGui::SameLine();
		if (ImGui::Button(">>", moveNavButtonSize)) {

		}

		static float h = 455.0f;
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

		if (ImGui::BeginChild("##scrolling", ImVec2(0, h), true)) {

			if (ImGui::BeginTable("Moves", 3, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders)) {
				for (int i = 0; i < pgnMoves.size(); i++)
				{
					ImGui::TableNextColumn();
					ImGui::SetWindowFontScale(moveFontScale);
					if (!(i % 2)) {
						int row = (i / 2) + 1;
						ImGui::Text("%d", row);
						ImGui::TableNextColumn();

					}
					ImGui::Text(pgnMoves[i].c_str());
				}
				ImGui::EndTable();
			}


			ImGui::EndChild();
		}

		ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
		if (ImGui::IsItemActive())
			h += ImGui::GetIO().MouseDelta.y;
		
		if (ImGui::BeginChild("#scrolling", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true)) {
			
			ImGui::SetWindowFontScale(chatFontScale);

			int flags = ImGuiTableFlags_Resizable;
			if (ImGui::BeginTable("Chat Box", 2, flags)) {
				
				for (Chat& line : chatLog) {
					ImGui::TableNextColumn();

					ImGui::Text("%s: ", line.from.c_str());
					
					if (line.from == "You") {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(youColour));
					}
					else {
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(otherColour));
					}

					ImGui::TableNextColumn();
					ImGui::TextWrapped(line.text.c_str());
					ImGui::PopStyleColor();

				}

				ImGui::EndTable();
				
			}

			ImGui::EndChild();
		
		}

		ImGui::PushItemWidth(inputBoxSize);
		if (ImGui::InputText(" ", chatBoxBuffer, bufferSize, ImGuiInputTextFlags_EnterReturnsTrue))
		{
			sendChat();
		}
		ImGui::SameLine();
		if (ImGui::Button("Send"))
		{
			sendChat();
		}
		
		ImGui::PopStyleVar();

		ImGui::End();

	}

}

void App::sendChat()
{	
	chatLog.push_back(Chat("You", chatBoxBuffer));
	chatBoxBuffer[0] = '\0';

	if (gamemode == MULTIPLAYER)
	{
		sf::Packet packet;
		packet << Message(CHAT, chatLog.back().text);
		sendPacket(packet);
	}
}