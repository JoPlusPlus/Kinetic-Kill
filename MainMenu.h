#pragma once
#include<SFML/Graphics.hpp>
#include<iostream>
using namespace std;
using namespace sf;

class MainMenu {
public:
	int selected;
	Text mainmenu[4];
	Font font;



public:

	MainMenu();
	void draw(RenderWindow& window);
	void move_up();
	void move_down();
	void set_selected(int n);
	int pressed() {
		return selected;
	}
	~MainMenu();

};


class PauseMenu {
private:
	int selected = 0;
	Text pausemenu[3];
	Font font;

public:

	PauseMenu();
	void draw(RenderWindow& window, bool IsLevelTwo);
	void set_selected(int n);
	void move_up();
	void move_down();
	int pressed() {
		return selected;
	}
	~PauseMenu();
};

class GameOverMenu {
private:
	int selected = 0;
	Text gameovermenu[5];
	Text deadText;
	Font font;
	
public:
	void SetGameOverMenu();
	void draw(RenderWindow& window, bool IsLevelTwo);
	void move_up();
	void move_down();
	int pressed() {
		return selected;
	}
};