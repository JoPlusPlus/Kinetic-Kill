#include "MainMenu.h"
#include<SFML/Graphics.hpp>
#include<vector>
using namespace std;
using namespace sf;

MainMenu::MainMenu() {
	font.loadFromFile("Chosen/font/chinese rocks rg.otf");

	float text_y_position = 300;
	float text_x_position = 500;

	mainmenu[0].setFont(font);
	mainmenu[0].setFillColor(Color::Yellow);
	mainmenu[0].setString("Play");
	mainmenu[0].setCharacterSize(75);
	mainmenu[0].setPosition(text_x_position, text_y_position);

	mainmenu[1].setFont(font);
	mainmenu[1].setFillColor(Color::White);
	mainmenu[1].setString("Controls");
	mainmenu[1].setCharacterSize(100);
	mainmenu[1].setPosition(text_x_position, text_y_position + 150);

	mainmenu[2].setFont(font);
	mainmenu[2].setFillColor(Color::White);
	mainmenu[2].setString("LeaderBoard");
	mainmenu[2].setCharacterSize(100);
	mainmenu[2].setPosition(text_x_position, text_y_position + 300);

	mainmenu[3].setFont(font);
	mainmenu[3].setFillColor(Color::White);
	mainmenu[3].setString("Exit");
	mainmenu[3].setCharacterSize(100);
	mainmenu[3].setPosition(text_x_position, text_y_position + 450);

	selected = 0;
}
MainMenu::~MainMenu() {}

void MainMenu::set_selected(int n) {
	selected = n;
}

void MainMenu::draw(RenderWindow& window) {

	for (int i = 0; i < 4; i++) {
		window.draw(mainmenu[i]);
	}
}

void MainMenu::move_up() {
	mainmenu[selected].setFillColor(Color::White);
	mainmenu[selected].setCharacterSize(100);
	selected--;
	if (selected == -1) selected = 3;
	mainmenu[selected].setFillColor(Color::Yellow);
	mainmenu[selected].setCharacterSize(75);
}

void MainMenu::move_down() {
	mainmenu[selected].setFillColor(Color::White);
	mainmenu[selected].setCharacterSize(100);
	selected++;
	if (selected == 4) selected = 0;
	mainmenu[selected].setFillColor(Color::Yellow);
	mainmenu[selected].setCharacterSize(75);
}

// ________________________________________________________________________________________


float text_x_position = 800;
float text_y_position = 500;


PauseMenu::PauseMenu() {



	font.loadFromFile("Chosen/font/chinese rocks rg.otf");




	pausemenu[0].setFont(font);
	pausemenu[0].setFillColor(Color::Yellow);
	pausemenu[0].setString("Continue");
	pausemenu[0].setCharacterSize(75);
	pausemenu[0].setPosition(text_x_position, text_y_position);

	pausemenu[1].setFont(font);
	pausemenu[1].setFillColor(Color::White);
	pausemenu[1].setString("Controls");
	pausemenu[1].setCharacterSize(100);
	pausemenu[1].setPosition(text_x_position, text_y_position + 150);

	pausemenu[2].setFont(font);
	pausemenu[2].setFillColor(Color::White);
	pausemenu[2].setString("Exit");
	pausemenu[2].setCharacterSize(100);
	pausemenu[2].setPosition(text_x_position, text_y_position + 300);



	selected = 0;

}
PauseMenu::~PauseMenu() {}



void PauseMenu::draw(RenderWindow& window, bool IsLevelTwo) {

	
	if (IsLevelTwo) {
		text_y_position = -3600;
		pausemenu[0].setPosition(text_x_position, text_y_position);
		pausemenu[1].setPosition(text_x_position, text_y_position + 150);
		pausemenu[2].setPosition(text_x_position, text_y_position + 300);
	}
	else {
		text_y_position = 500;
		pausemenu[0].setPosition(text_x_position, text_y_position);
		pausemenu[1].setPosition(text_x_position, text_y_position + 150);
		pausemenu[2].setPosition(text_x_position, text_y_position + 300);
	}
	for (int i = 0; i < 3; i++) {
		window.draw(pausemenu[i]);
	}
}

void PauseMenu::move_up() {
	pausemenu[selected].setFillColor(Color::White);
	pausemenu[selected].setCharacterSize(100);
	selected--;
	if (selected == -1) selected = 2;
	pausemenu[selected].setFillColor(Color::Yellow);
	pausemenu[selected].setCharacterSize(75);
}

void PauseMenu::move_down() {
	pausemenu[selected].setFillColor(Color::White);
	pausemenu[selected].setCharacterSize(100);
	selected++;
	if (selected == 3) selected = 0;
	pausemenu[selected].setFillColor(Color::Yellow);
	pausemenu[selected].setCharacterSize(75);
}

void PauseMenu::set_selected(int n) {
	selected = n;

	pausemenu[0].setFont(font);
	pausemenu[0].setFillColor(Color::Yellow);
	pausemenu[0].setString("Continue");
	pausemenu[0].setCharacterSize(75);
	pausemenu[0].setPosition(text_x_position, text_y_position);

	pausemenu[1].setFont(font);
	pausemenu[1].setFillColor(Color::White);
	pausemenu[1].setString("Settings");
	pausemenu[1].setCharacterSize(100);
	pausemenu[1].setPosition(text_x_position, text_y_position + 150);

	pausemenu[2].setFont(font);
	pausemenu[2].setFillColor(Color::White);
	pausemenu[2].setString("Exit");
	pausemenu[2].setCharacterSize(100);
	pausemenu[2].setPosition(text_x_position, text_y_position + 300);


	
}

// ________________________________________________________________________________________



void GameOverMenu::SetGameOverMenu() {

	int start_x_position = 500, start_y_position = 400, rate = 100;

	font.loadFromFile("Chosen/font/chinese rocks rg.otf");

	deadText.setFont(font);
	deadText.setFillColor(Color::Red);
	deadText.setString("YOU DIED!");
	deadText.setCharacterSize(150);
	deadText.setPosition(50, 200);

	gameovermenu[0].setFont(font);
	gameovermenu[0].setFillColor(Color::Yellow);
	gameovermenu[0].setString("Try again");
	gameovermenu[0].setCharacterSize(75);
	gameovermenu[0].setPosition(start_x_position, start_y_position + rate);



	gameovermenu[1].setFont(font);
	gameovermenu[1].setFillColor(Color::White);
	gameovermenu[1].setString("Enter as a new player");
	gameovermenu[1].setCharacterSize(100);
	gameovermenu[1].setPosition(start_x_position, start_y_position + rate * 2);

	gameovermenu[2].setFont(font);
	gameovermenu[2].setFillColor(Color::White);
	gameovermenu[2].setString("LeaderBoard");
	gameovermenu[2].setCharacterSize(100);
	gameovermenu[2].setPosition(start_x_position, start_y_position + rate * 3);

	gameovermenu[3].setFont(font);
	gameovermenu[3].setFillColor(Color::White);
	gameovermenu[3].setString("Return to MainMenu");
	gameovermenu[3].setCharacterSize(100);
	gameovermenu[3].setPosition(start_x_position, start_y_position + rate * 4);

	gameovermenu[4].setFont(font);
	gameovermenu[4].setFillColor(Color::White);
	gameovermenu[4].setString("Exit");
	gameovermenu[4].setCharacterSize(100);
	gameovermenu[4].setPosition(start_x_position, start_y_position + rate * 5);



	selected = 0;
}

void GameOverMenu::move_up() {
	gameovermenu[selected].setFillColor(Color::White);
	gameovermenu[selected].setCharacterSize(100);
	selected--;
	if (selected == -1) selected = 4;
	gameovermenu[selected].setFillColor(Color::Yellow);
	gameovermenu[selected].setCharacterSize(75);
}

void GameOverMenu::move_down() {

	gameovermenu[selected].setFillColor(Color::White);
	gameovermenu[selected].setCharacterSize(100);
	selected++;
	if (selected == 5) selected = 0;
	gameovermenu[selected].setFillColor(Color::Yellow);
	gameovermenu[selected].setCharacterSize(75);
	
}

void GameOverMenu::draw(RenderWindow& window, bool IsLevelTwo) {

	Texture tbackground;
	tbackground.loadFromFile("Chosen/Backgrounds/game_background_3.1.png");
	Sprite sbackground(tbackground);


	if (IsLevelTwo) {
		int start_x_position2 = 500, start_y_position2 = -3680, rate2 = 100;

		sbackground.setPosition(0, -4080);
		deadText.setPosition(200, -3880);
		gameovermenu[0].setPosition(start_x_position2, start_y_position2 + rate2 * 1);
		gameovermenu[1].setPosition(start_x_position2, start_y_position2 + rate2 * 2);
		gameovermenu[2].setPosition(start_x_position2, start_y_position2 + rate2 * 3);
		gameovermenu[3].setPosition(start_x_position2, start_y_position2 + rate2 * 4);
		gameovermenu[4].setPosition(start_x_position2, start_y_position2 + rate2 * 5);
	}
	window.draw(sbackground);
	window.draw(deadText);

	for (int i = 0; i < 5; i++) window.draw(gameovermenu[i]);
}