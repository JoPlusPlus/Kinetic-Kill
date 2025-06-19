#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <SFML/Audio.hpp>
#include <sstream>
#include <iomanip>
#include "MainMenu.h"
#include<thread>
#include<chrono>

using namespace std;
using namespace sf;


SoundBuffer healthbuff;
Sound healthsound;
SoundBuffer gow;
Sound gows;
SoundBuffer bfOver;
Sound SOver;
SoundBuffer bfCutscene;
Sound sCutscene;
SoundBuffer bfLevelTwo;
Sound sLevelTwo;
SoundBuffer bfCompletion;
Sound SCompletion;
int y = 0;
bool IsLevelTwo = 0;
bool EnteredLevelTwo = 0;
int Deaths = 0;
bool Won = 0;



// global 

RenderWindow window(VideoMode(1920, 1080), "SFML works!", Style::Fullscreen);

Texture tback;
Sprite sback;
View LevelOneView(FloatRect(0, 0, 1920, 1080));


// menu handling
MainMenu this_mainmenu;


//pausemenu handling
PauseMenu this_pausemenu;
bool entered_pausemenu = false;


// cutscene
Sprite Sprites[4];




// overmenu handling
GameOverMenu thisGameOverMenu;

string PlayerName;
// Drops
Texture tDrops[4];
Sprite sDrops[4];
RectangleShape blocks_real_collider[15];

Font playerFont;

// status
Text playerHealthText;
String playerHealthString;

Text PointsText;
String PointsString;

Text AmmoText;
String AmmoString;

Text SurvivedTimeText;
String SurvivedTimeString;

Text TimerText; 
String TimerString;

Text lowHealthText;

int highestScore = -1;
// _______________________________________________________________________________________________________________

struct Enemy {
    Sprite sEnemy;
    int speed;
    RectangleShape enemyRealcollider;

};

struct LevelTwoEnemies {
    Texture RunTex[6], AttackTex[6], DeathTex[6], IdleTex[6];
    Sprite sEnemy;
    RectangleShape AttackRangeBox, collider, Fire;
    int health, AttackDamage;
    int speed;
    bool allive = 0, Isdead = 1;
    int WalkFrame = 0, AttackFrame = 0, DeathFrame = 0, IdleFrame = 0;
    Clock AttackClock, WalkClock, DeadClock, IdleClock, AttackCoolDown;
} demon[8], dragon[2], medusa[6];

struct Drop {
    Sprite sDropshape;
    RectangleShape dropCollider, targetCollider;
    int Ground;
    int type;
   
};

struct Bullet {
    int speed = 10;
    float coolDown = 1;
    int magazine = 3;
    int move_to_right = -1;
    CircleShape bulletCircle;

};

class Player {
public:

    Sprite Ninja;
    float speed = 360.0f;       // Movement speed
    float DashSpeed = 25000.0f;
    bool facingRight = true;    // Character direction
    bool onground = 1;
    bool moving = false;
    float gravity = 1000.0f;    // Gravity
    float verticalVelocity = 0.0f; // Vertical velocity for jumping
    float groundHeight = 1000.0f;   // Ground level
    const int jumpSpeed = -580.0f; // Jump strength

    RectangleShape playerRealcollider;
    CircleShape playerProtection;


    // status level one
    int health = 10;
    int LVL1Score = 0; int LVL2Score = 0;

    // status level two
    int GameOverTimer = 0;
    Clock TimerCoolDown, TimerCountCoolDown;
    int SurvivedTime = 0;



    // Textures
    Texture idleTex;
    Texture shootingUpTex;
    Texture RunTex;

    // animations
    int WidthFrame_shoting = 0;
    int WidthFrame_idle = 0;
    int widthFrameRun = 0;
    Clock AnimRight, AnimLeft, AnimIdle;
    int AnimShooting = 0;



    // abilities
    bool Protected = 0;
    float  ProtectionCooldown = 0;
    bool CanShoot = 0;
    int bulletIndex = -1;
    float bulletCooldown;
    vector<Bullet> bullets;
    float DashCoolDown = 0;
    bool CanDash = 1;
    bool ProtectedDash = 0;
    



    Player() {
        // Load textures
        if (!idleTex.loadFromFile("Chosen/Soldier_1/Idle.png")) {
            cerr << "Error loading idle texture" << endl;
        }
        if (!shootingUpTex.loadFromFile("Chosen/Soldier_1/Shot_2.png")) {
            cerr << "Error loading shooting texture" << endl;
        }
        if (!RunTex.loadFromFile("Chosen/Soldier_1/Run.png")) {
            cerr << "Error loading running texture" << endl;
        }

        Ninja.setTexture(idleTex);
        Ninja.setOrigin(128 / 2, 128 / 2);
        Ninja.setPosition(500, groundHeight);



        playerRealcollider.setSize(Vector2f(40, 70));
        playerRealcollider.setFillColor(Color::Green);

        playerRealcollider.setPosition(Ninja.getPosition());
        playerRealcollider.setOrigin(Ninja.getOrigin());

        playerProtection.setRadius(35);
        playerProtection.setFillColor(Color::White);
        playerProtection.setPosition(Ninja.getPosition());
        playerProtection.setOrigin(playerProtection.getGlobalBounds().width / 2, playerProtection.getGlobalBounds().height / 2);

        // sounds

    }

    void move(float deltaTime) {
        
        moving = false;
        // Horizontal movement
        if (Keyboard::isKeyPressed(Keyboard::D)) {
            Ninja.move(speed * deltaTime, 0);
            facingRight = true; // Facing right
            moving = true;

        }
        else if (Keyboard::isKeyPressed(Keyboard::A)) {
            Ninja.move(-speed * deltaTime, 0);
            facingRight = false; // Facing left
            moving = true;

        }
        // shooting
        
        if (Mouse::isButtonPressed(Mouse::Left) && bulletIndex >= 0 && CanShoot) {

            if (facingRight) bullets[bulletIndex].bulletCircle.setPosition(Ninja.getPosition().x + 27, Ninja.getPosition().y);
            else bullets[bulletIndex].bulletCircle.setPosition(Ninja.getPosition().x - 50, Ninja.getPosition().y);
            
            if (facingRight) bullets[bulletIndex].move_to_right = 1;
            else bullets[bulletIndex].move_to_right = 0;

            bulletCooldown = bullets[bulletIndex].coolDown;
            bulletIndex--;
            CanShoot = 0;
            AnimShooting = 20;
        }

        // Jumping
        if (Keyboard::isKeyPressed(Keyboard::W) && onground) {
            verticalVelocity = jumpSpeed;
            onground = 0;
        }
        // dashing 
        if (Mouse::isButtonPressed(Mouse::Right) && CanDash && IsLevelTwo) {
            if (facingRight) Ninja.move(DashSpeed * deltaTime, 0);
            else  Ninja.move(-DashSpeed * deltaTime, 0);

            ProtectedDash = 1;
            CanDash = 0;
            DashCoolDown = 2;
        }

        // Apply gravity


        verticalVelocity += gravity * deltaTime;
        Ninja.move(0, verticalVelocity * deltaTime);





        // Prevent falling through the ground
        if (Ninja.getPosition().y >= groundHeight) {
            Ninja.setPosition(Ninja.getPosition().x, groundHeight);
            verticalVelocity = 0.0f;
            onground = 1;
        }

        // prevent passing through screen edges
        for (int i = 0; i < 5; i++) {
            if (Ninja.getPosition().x > 1900) Ninja.setPosition(1900, Ninja.getPosition().y);
            if (Ninja.getPosition().x < 10) Ninja.setPosition(10, Ninja.getPosition().y);
        }





        // Flip the sprite based on facing direction
        Ninja.setScale(facingRight ? 1.0f : -1.0f, 1.0f);

        playerRealcollider.setPosition(Ninja.getPosition().x + 45, Ninja.getPosition().y + 60);
        playerRealcollider.setOrigin(Ninja.getOrigin());

        playerProtection.setPosition(Ninja.getPosition().x + 25 , Ninja.getPosition().y + 60);
        playerProtection.setOrigin(Ninja.getOrigin());
    }

    void PlayerAnimation() {
        if (moving && facingRight) {
            if (AnimRight.getElapsedTime().asSeconds() >= 0.03) {
                Ninja.setTexture(RunTex);
                Ninja.setTextureRect(IntRect(widthFrameRun * 128, 0, 128, 128));
                widthFrameRun = (widthFrameRun + 1) % 8;
                AnimRight.restart();
            }
        }
        if (moving && !facingRight) {
            if (AnimLeft.getElapsedTime().asSeconds() >= 0.03) {
                Ninja.setTexture(RunTex);
                Ninja.setTextureRect(IntRect(widthFrameRun * 128, 0, 128, 128));
                widthFrameRun = (widthFrameRun + 1) % 8;
                AnimLeft.restart();
            }
        }
        if (Mouse::isButtonPressed(Mouse::Left) && AnimShooting !=0) {

            Ninja.setTexture(shootingUpTex);
            Ninja.setTextureRect(IntRect(WidthFrame_shoting * 128, 0, 128, 128));
            WidthFrame_shoting = (WidthFrame_shoting + 1) % 4;
            AnimShooting -= 1;
        }
        
        else if (!moving) {

            if (AnimIdle.getElapsedTime().asSeconds() >= 0.1) {
                Ninja.setTexture(idleTex);
                Ninja.setTextureRect(IntRect(WidthFrame_idle * 128, 0, 128, 128));
                WidthFrame_idle = (WidthFrame_idle + 1) % 7;
                AnimIdle.restart();
            }

        }
    }
};


vector<Drop> dropBag;

// Functions Declration
void Settings();
void Pname();

void setStatus(Player& player);
void updateStatus(Player& player);
void displayStatus();
void resetStatus();
void GamePlay();
void pauseMenu();
void blocks_fun(Sprite blocks[], RectangleShape blocks_Real_collider[]);
void GameOver(Player player);
void SetDrops();
void ChooseDrop(RectangleShape blocks_real_collider[], Clock& Addtimer, Clock& DeleteTimer);
void DropADrop(float dt, Player& player);
void AbbilitiesCoolDown(Player& player);
void Bulletmovement(vector<Bullet> &bullets);
void LeaderBoard();
void Cutscene(Player& player, Sprite blocks[], Enemy enemies[]);
void BackGroundEnemies(Player& player);
void LevelTwoLoadEnemies();
void LevelTwoChooseEnemy(Clock& SetDemonClock, Clock &SetDragonClock, Player player);
void LevelTwoUpdateEnemies(Player& player);
void ResetEnemies();
void LevelTwo(Player& player, Sprite Sprites[]);
void LVL2Instrunctions();
void Completetion(Player& player);


int main() {
    
    window.setMouseCursorVisible(false);

    // coming from level two
    window.setView(LevelOneView);
    IsLevelTwo = 0;
    highestScore = -1;
    Deaths = 0;
    Won = 0;
    //////////////////

    Texture this_mainmenu_tex;
    Sprite this_mainmenu_sprite;

    this_mainmenu_tex.loadFromFile("Chosen/Backgrounds/game_background_3.1.png");
    this_mainmenu_sprite.setTexture(this_mainmenu_tex);


    while (window.isOpen()) {
        
        Event event;
        while (window.pollEvent(event)) {

            if (event.type == Event::Closed) {
                window.close();
            }
            if (event.type == Event::KeyPressed) {

                if (event.key.code == Keyboard::Up) {
                    this_mainmenu.move_up();
                }
                if (event.key.code == Keyboard::Down) {
                    this_mainmenu.move_down();
                }
                if (event.key.code == Keyboard::Enter) {
                    if (this_mainmenu.pressed() == 0) Pname();
                    if (this_mainmenu.pressed() == 1) Settings();
                    if (this_mainmenu.pressed() == 2) LeaderBoard();
                    if (this_mainmenu.pressed() == 3) window.close();
                }
            }
            window.clear();

            if (IsLevelTwo) {
                this_mainmenu_sprite.setPosition(0, -4080);
                this_mainmenu.mainmenu[0].setPosition(300, -3600);
                this_mainmenu.mainmenu[1].setPosition(300, -3600 + 150);
                this_mainmenu.mainmenu[2].setPosition(300, -3600 + 300);
            }

            window.draw(this_mainmenu_sprite);
            this_mainmenu.draw(window);
            window.display();

        }


    }

    return 0;
}

void Settings() {
    Text instructions;
    Font f; f.loadFromFile("Chosen/font/Playername font/ArtisanParis-Regular.otf");
    instructions.setFont(f);
    instructions.setCharacterSize(100);
    instructions.setString("W => jump\nD => move to right\nA => move to left\nLeft click => shoot");
    if (IsLevelTwo) {
        instructions.setPosition(300, -3980);
        instructions.setString("W => jump\nD => move to right\nA => move to left\nLeft Click => shoot\nRight Click => dash");
    }
    else instructions.setPosition(300, 100);


    while (window.isOpen()) {

        Event e;
        while (window.pollEvent(e)) {

            if (e.type == Event::Closed) {
                window.close();
            }
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                return;
            }
            window.clear();
            window.draw(instructions);
            window.display();
        }
    }
}

void Pname() {
    
    // coming from level two
    window.setView(LevelOneView);
    IsLevelTwo = 0;
    highestScore = -1;
    Deaths = 0;
    Won = 0;
    //////////////////

    if (!PlayerName.empty()) {
        PlayerName.clear();
    }

    Font player_name_font;
    Font system_font; 
    player_name_font.loadFromFile("Chosen/font/BreatheFireIv-3zAVZ.ttf");
    system_font.loadFromFile("Chosen/font/chinese rocks rg.otf");
    Text t1, t2;
    t1.setString("Enter your name");
    t1.setFont(system_font), t2.setFont(player_name_font);
    t1.setCharacterSize(100), t2.setCharacterSize(100);
    t1.setFillColor(Color(128, 128, 128)), t2.setFillColor(Color::White);
    t1.setPosition(50, 100), t2.setPosition(50, 300);

    Texture tPnameBackground;
    tPnameBackground.loadFromFile("Chosen/Backgrounds/Castle.png");
    Sprite sPnameBackground(tPnameBackground);
    sPnameBackground.setScale(0.5, 0.5);


    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                break;
            }
            else if (Keyboard::isKeyPressed(Keyboard::BackSpace) && PlayerName.size() > 0) {
                PlayerName.pop_back();
            }
            else if ((Keyboard::isKeyPressed(Keyboard::Enter)) && (PlayerName.size() >= 4)) {
                GamePlay();
               
            }
            else if (Keyboard::isKeyPressed(Keyboard::Escape)) {
                return;
            }
            else if (event.type == Event::TextEntered) {
                PlayerName += event.text.unicode;
            }
            t2.setString(PlayerName);
        }
        
        
        window.clear();
        window.draw(sPnameBackground);
        window.draw(t1);
        window.draw(t2);
        window.display();

    }
}

void GamePlay() {

    // coming from level two
    window.setView(LevelOneView);
    IsLevelTwo = 0;
    Deaths = 0;
    Won = 0;
    //////////////////

    

    tback.loadFromFile("Chosen/menu/PNG/1/terrace.png");
    sback.setTexture(tback);
    sback.setScale(0.5, 0.5);

    Clock clock;

    Player player;
    

    setStatus(player);

    gow.loadFromFile("Chosen/Sound/Brothers of Blood(MP3_320K).mp3");
    gows.setBuffer(gow);
    bfOver.loadFromFile("Chosen/Sound/DeathMenu/Wardruna - Einar Selvik - Snake Pit Poetry  [FULL EP](MP3_320K)-[AudioTrimmer.com]-[AudioTrimmer.com].mp3");
    SOver.setBuffer(bfOver);
    gows.play();
    gows.setLoop(1);






    // loading enemy tex

    vector <Texture> tEnemy;

    Texture t;
    for (int i = 1; i <= 6; i++) {
        if (!t.loadFromFile("Chosen/enemies/PNG/lizard/Walk" + to_string(i) + ".png")) {
            cerr << "Failed to load " << i;
        }
        else {
            tEnemy.push_back(t);
        }
    }

    Enemy enemies[6];


    // Set enemy
    for (int i = 0; i < 6; i++) {

        enemies[i].sEnemy.setTexture(tEnemy[i]);
        enemies[i].speed = rand() % (8) + 8;
        int y = rand() % (921) + 80;

        enemies[i].sEnemy.setOrigin(enemies[i].sEnemy.getGlobalBounds().width / 2, enemies[i].sEnemy.getGlobalBounds().height / 2);

        enemies[i].enemyRealcollider.setSize(Vector2f(90, 60));
        enemies[i].enemyRealcollider.setFillColor(Color::Cyan);


        enemies[i].enemyRealcollider.setPosition(enemies[i].sEnemy.getPosition());
        enemies[i].enemyRealcollider.setOrigin(enemies[i].enemyRealcollider.getGlobalBounds().width / 2, enemies[i].enemyRealcollider.getGlobalBounds().height / 2);



        if (i < 3) {
            enemies[i].sEnemy.setPosition(0, y);
            enemies[i].sEnemy.setTextureRect(IntRect(0, 0, 256, 256));
        }
        else {
            enemies[i].sEnemy.setPosition(1900, y);
            enemies[i].sEnemy.setTextureRect(IntRect(0, 0, 256, 256));
        }
    }


    // load blocks tex
    Sprite blocks[15];
    

    Texture blockTexture;
    blockTexture.loadFromFile("Chosen/objects/blocks/pngtree-game-ground-asset-seamless-level-png-image_6373714.png");

    blocks_fun(blocks, blocks_real_collider);

    for (int i = 0; i < 13; i++) {
        blocks[i].setTexture(blockTexture);
        blocks[i].setTextureRect(IntRect(22, 255, 290, 100));
    }

    // RectangleShape collider(Vector2f(290, 44)), collider2(Vector2f(145,45));

    // Drops
    Clock addTimer;
    Clock deleteTimer;
    dropBag.clear();
    SetDrops();

    // sounds



    while (window.isOpen()) {

        window.setFramerateLimit(60);
        float deltaTime = clock.restart().asSeconds();

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                break;
            }
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    gows.pause();
                    pauseMenu();
                }
            }
            
        }
     

        AbbilitiesCoolDown(player);
        Bulletmovement(player.bullets);
        player.move(deltaTime);
        player.PlayerAnimation();



        // enemy move, handle collision between enemy & player

        for (int i = 0; i < 6; i++) {

            if (i < 3) {
                enemies[i].sEnemy.move(enemies[i].speed, 0);
                if (enemies[i].sEnemy.getPosition().x > 1900) {
                    int y = rand() % (921) + 80;
                    enemies[i].sEnemy.setPosition(-200, y);
                    enemies[i].speed = rand() % (8) + 8;
                }
                // enemy hit player
                
                if (!player.Protected) {
                    if (player.playerRealcollider.getGlobalBounds().intersects(enemies[i].enemyRealcollider.getGlobalBounds())) {
                        int y = rand() % (921) + 80;
                        enemies[i].sEnemy.setPosition(0, y);
                        enemies[i].speed = rand() % (8) + 8;
                        player.health--;
                    }
                }

                // player shoot enemy
                for (int j = 0; j < player.bullets.size(); j++) {
                    if (player.bullets[j].bulletCircle.getGlobalBounds().intersects(enemies[i].enemyRealcollider.getGlobalBounds())) {
                        int y = rand() % (921) + 80;
                        enemies[i].sEnemy.setPosition(0, y);
                        enemies[i].speed = rand() % (8) + 8;
                        player.LVL1Score++;
                        player.bullets.erase(player.bullets.begin() + j);
                    }
                }

                enemies[i].enemyRealcollider.setOrigin(enemies[i].sEnemy.getOrigin().x - 80, enemies[i].sEnemy.getOrigin().y - 100);
            }
            else {
                enemies[i].sEnemy.setScale(-1, 1);
                enemies[i].sEnemy.move(-enemies[i].speed, 0);
                if (enemies[i].sEnemy.getPosition().x < 0) {
                    int y = rand() % (744) + 80;
                    enemies[i].sEnemy.setPosition(1900, y);
                    enemies[i].speed = rand() % (8) + 8;
                }
                // enemy hit player
                if (!player.Protected) {
                    if (player.playerRealcollider.getGlobalBounds().intersects(enemies[i].enemyRealcollider.getGlobalBounds())) {
                        int y = rand() % (921) + 80;
                        enemies[i].sEnemy.setPosition(1900, y);
                        enemies[i].speed = rand() % (8) + 8;
                        player.health--;
                    }

                }
                // player shoot enemy
                for (int j = 0; j < player.bullets.size(); j++) {
                    if (player.bullets[j].bulletCircle.getGlobalBounds().intersects(enemies[i].enemyRealcollider.getGlobalBounds())) {
                        int y = rand() % (921) + 80;
                        enemies[i].sEnemy.setPosition(1900, y);
                        enemies[i].speed = rand() % (8) + 8;
                        player.LVL1Score++;
                        player.bullets.erase(player.bullets.begin() + j);
                    }
                }
                enemies[i].enemyRealcollider.setOrigin(enemies[i].sEnemy.getOrigin().x - 90, enemies[i].sEnemy.getOrigin().y - 100);
            }

            enemies[i].enemyRealcollider.setPosition(enemies[i].sEnemy.getPosition());

        }
        
        

        // handle collision between player & blocks

        for (int i = 0; i < 13; i++) {
            if (player.playerRealcollider.getGlobalBounds().intersects(blocks_real_collider[i].getGlobalBounds())) {



                if (i != 8 && i != 9 && i != 11) {

                    if (player.playerRealcollider.getGlobalBounds().left <= blocks_real_collider[i].getPosition().x && Keyboard::isKeyPressed(Keyboard::D)) { // left collision
                        player.Ninja.setPosition(blocks_real_collider[i].getPosition().x, player.Ninja.getPosition().y);
                    }
                    else if (player.playerRealcollider.getGlobalBounds().left >= blocks_real_collider[i].getPosition().x + 240 && Keyboard::isKeyPressed(Keyboard::A)) {
                        player.Ninja.setPosition(blocks_real_collider[i].getPosition().x + 290, player.Ninja.getPosition().y);
                    }
                    else {
                        if (player.playerRealcollider.getGlobalBounds().top >= blocks_real_collider[i].getPosition().y) {
                            player.verticalVelocity = 0.0f;


                            player.Ninja.setPosition(player.Ninja.getPosition().x, blocks_real_collider[i].getPosition().y + 70);
                        }
                        else if (player.playerRealcollider.getGlobalBounds().top + 70 >= blocks_real_collider[i].getPosition().y && player.playerRealcollider.getGlobalBounds().left >= blocks_real_collider[i].getPosition().x) {
                            player.Ninja.setPosition(player.Ninja.getPosition().x, blocks_real_collider[i].getPosition().y - 70);
                            player.onground = 1;
                            player.verticalVelocity -= player.gravity * deltaTime;
                            player.playerProtection.setPosition(player.Ninja.getPosition().x + 25, player.Ninja.getPosition().y + 60);


                        }
                    }
                    if (player.playerRealcollider.getGlobalBounds().left + 20 <= blocks_real_collider[i].getPosition().x) {
                        player.onground = 0;
                    }

                }
                else {

                    if (player.playerRealcollider.getGlobalBounds().left <= blocks_real_collider[i].getPosition().x && Keyboard::isKeyPressed(Keyboard::D)) { // left collision
                        player.Ninja.setPosition(blocks_real_collider[i].getPosition().x, player.Ninja.getPosition().y);
                        player.playerRealcollider.setPosition(blocks_real_collider[i].getPosition().x, player.Ninja.getPosition().y);
                    }
                    else if (player.playerRealcollider.getGlobalBounds().left >= blocks_real_collider[i].getPosition().x + 120 && Keyboard::isKeyPressed(Keyboard::A)) {
                        player.Ninja.setPosition(blocks_real_collider[i].getPosition().x + 145, player.Ninja.getPosition().y);
                        player.playerRealcollider.setPosition(blocks_real_collider[i].getPosition().x + 145, player.Ninja.getPosition().y);
                    }
                    else {
                        if (player.playerRealcollider.getGlobalBounds().top >= blocks_real_collider[i].getPosition().y) {
                            player.verticalVelocity = 0.0f;


                            player.Ninja.setPosition(player.Ninja.getPosition().x, blocks_real_collider[i].getPosition().y + 70);
                            player.playerRealcollider.setPosition(player.Ninja.getPosition().x, blocks_real_collider[i].getPosition().y + 70);
                        }
                        else if (player.playerRealcollider.getGlobalBounds().top + 70 >= blocks_real_collider[i].getPosition().y && player.playerRealcollider.getGlobalBounds().left >= blocks_real_collider[i].getPosition().x) {
                            player.Ninja.setPosition(player.Ninja.getPosition().x, blocks_real_collider[i].getPosition().y - 70);
                            player.playerRealcollider.setPosition(player.Ninja.getPosition().x, blocks_real_collider[i].getPosition().y - 70);
                            player.onground = 1;
                            player.verticalVelocity -= player.gravity * deltaTime;
                            player.playerProtection.setPosition(player.Ninja.getPosition().x + 25, player.Ninja.getPosition().y + 60);

                        }
                    }


                }
            }

        }

        // drops updating
        ChooseDrop(blocks_real_collider, addTimer, deleteTimer);
        DropADrop(deltaTime, player);
        player.playerRealcollider.setPosition(player.Ninja.getPosition().x + 45, player.Ninja.getPosition().y + 60);
        player.playerRealcollider.setOrigin(player.Ninja.getOrigin());


        
        updateStatus(player);
        
        // check death
        if (player.health <= 0) {
            Deaths++;
            if (player.LVL1Score > highestScore) {
                highestScore = player.LVL1Score;

                ofstream offile;
                offile.open("LeaderBoard.txt", ios::app);
                offile << PlayerName << "          " << player.LVL1Score << '*';
                offile.close();
            }

            gows.stop();
            SOver.play();

            this_thread::sleep_for(chrono::milliseconds(2900));

            thisGameOverMenu.SetGameOverMenu();
            GameOver(player);
        }


        window.clear();
        window.draw(sback);
        for (int i = 0; i < 13; i++) {

            //window.draw(blocks_real_collider[i]);
            window.draw(blocks[i]);
        }
        for (int i = 0; i < dropBag.size(); i++) {
            //window.draw(dropBag[i].dropCollider);
            window.draw(dropBag[i].sDropshape);
        }

        for (int i = 0; i < 6; i++) {
            //window.draw(enemies[i].enemyRealcollider);
            window.draw(enemies[i].sEnemy);

        }
        for (int i = 0; i < player.bullets.size(); i++) {
           window.draw(player.bullets[i].bulletCircle);
        }
        if (player.Protected) window.draw(player.playerProtection);
        
        //window.draw(player.playerRealcollider);
        window.draw(player.Ninja);
        displayStatus();

        window.display();

        // check win
        if (player.LVL1Score >= 70) {
            bfCutscene.loadFromFile("Chosen/Sound/Cutscene2/Rage of Sparta(MP3_320K).mp3");
            sCutscene.setBuffer(bfCutscene);


            gows.stop();
            sCutscene.play();
            Cutscene(player, blocks, enemies);
        }

    }

}

void setStatus(Player& player) {
    int CharacterSize = 60;

    //health
    playerFont.loadFromFile("Chosen/font/chinese rocks rg.otf");

    playerHealthText.setFont(playerFont);

    String playerHealthString = to_string(player.health);
    playerHealthText.setString("Health : " + playerHealthString);
    playerHealthText.setCharacterSize(CharacterSize);
    playerHealthText.setFillColor(Color::Green);

    // Ammo

    AmmoText.setFont(playerFont);
    AmmoText.setCharacterSize(CharacterSize);
    AmmoText.setFillColor(Color::White);

    // level one
    if (!IsLevelTwo) {
        AmmoString = to_string(player.bulletIndex + 1);
        AmmoText.setString("Ammo : " + AmmoString);
    }
    // level two
    else AmmoText.setString("Ammo : Infinite");


    //Points

    PointsText.setFont(playerFont);
    PointsText.setCharacterSize(CharacterSize);
    PointsText.setFillColor(Color::Green);
    PointsString = to_string(player.LVL1Score + player.LVL2Score);
    PointsText.setString("Points : " + PointsString);

    // Survived Time
    if (IsLevelTwo) {
        SurvivedTimeText.setFont(playerFont);
        SurvivedTimeText.setCharacterSize(CharacterSize);
        SurvivedTimeText.setFillColor(Color::Green);
        SurvivedTimeString = to_string(player.SurvivedTime);
        SurvivedTimeText.setString("Survived Time : " + SurvivedTimeString);
    }

    // Timer
    TimerText.setFont(playerFont);
    TimerText.setCharacterSize(70);
    TimerText.setFillColor(Color::Green);
    TimerString = to_string(player.GameOverTimer);
    TimerText.setString("Time: " + TimerString);



    if (!IsLevelTwo) {
        playerHealthText.setPosition(0, 0);
        AmmoText.setPosition(0, 75);
        PointsText.setPosition(0, 150);
    }
    else {
        playerHealthText.setPosition(0, -4000);
        AmmoText.setPosition(0, -3950);
        PointsText.setPosition(0, -3900);
        SurvivedTimeText.setPosition(0, -3850);
        TimerText.setPosition(900, -4000);
    }
}

void updateStatus(Player& player) {
    // Update health
    playerHealthString = to_string(player.health);
    playerHealthText.setString("Health : " + playerHealthString);

    // Check Ammo update
    if (!IsLevelTwo) {
        AmmoString = to_string(player.bulletIndex + 1);
        AmmoText.setString("Ammo : " + AmmoString);
    }
    else AmmoText.setString("Ammo : Infinite");

    // points Updatw
    PointsString = to_string(player.LVL1Score + player.LVL2Score);
    PointsText.setString("Points : " + PointsString);

    if (IsLevelTwo) {
        SurvivedTimeString = to_string(player.SurvivedTime);
        SurvivedTimeText.setString("Survived Time : " + SurvivedTimeString);
        TimerString = to_string(player.GameOverTimer);
        TimerText.setString("Time: " + TimerString);
    }

}

void displayStatus() {
    window.draw(playerHealthText);
    window.draw(AmmoText);
    window.draw(PointsText);
    if (IsLevelTwo) {
        window.draw(SurvivedTimeText);
        window.draw(TimerText);
    }
}

void resetStatus() {
    playerHealthText.setPosition(-500, 0);
    AmmoText.setPosition(-500, 0);
    PointsText.setPosition(-500, 0);
    SurvivedTimeText.setPosition(-500, 0);
    TimerText.setPosition(-500, 0);

}

void pauseMenu() {

    Texture tbackground;
    tbackground.loadFromFile("Chosen/menu/PNG/4/dead forest.png");
    Sprite sbackground(tbackground);
    sbackground.setScale(0.5, 0.5);
    
    

    while (window.isOpen()) {

        Event e;
        while (window.pollEvent(e)) {

            if (e.type == Event::Closed) {
                window.close();
                break;
            }

            if (e.type == Event::KeyPressed) {

                if (e.key.code == Keyboard::Escape) {
                    if (!IsLevelTwo)   gows.play();
                    else if (IsLevelTwo == 1) sLevelTwo.play();
                    return;
                }
                if (e.key.code == Keyboard::Up) {
                    this_pausemenu.move_up();
                }
                if (e.key.code == Keyboard::Down) {
                    this_pausemenu.move_down();
                }
                if (e.key.code == Keyboard::Enter) {
                    int selectedItem = this_pausemenu.pressed();
                    if (selectedItem == 0) { // Resume
                        if (!IsLevelTwo)   gows.play();
                        else if (IsLevelTwo) sLevelTwo.play();
                        return;
                    }
                    else if (selectedItem == 1) { // Options (example)
                        Settings();
                    }
                    else if (selectedItem == 2) { // Exit to main menu
                        this_pausemenu.set_selected(0);
                        main();
                    }
                }
            }

        }

        

        window.clear();

        // Draw pause menu
        window.draw(sbackground);

        if (IsLevelTwo) sbackground.setPosition(0, -4080);
        this_pausemenu.draw(window, IsLevelTwo);

        window.display();


    }
}

void blocks_fun(Sprite blocks[], RectangleShape blocks_Real_collider[]) {

    int height = 23;

    blocks[0].setPosition(1400, 640);
    blocks[0].setScale(1, 0.5);
    blocks_Real_collider[0].setPosition(blocks[0].getPosition().x, blocks[0].getPosition().y + 20);
    blocks_Real_collider[0].setSize(Vector2f(290, height));

    blocks[1].setPosition(1100, 500);
    blocks[1].setScale(1, 0.5);
    blocks_Real_collider[1].setPosition(blocks[1].getPosition().x, blocks[1].getPosition().y + 20);
    blocks_Real_collider[1].setSize(Vector2f(290, height));

    blocks[2].setPosition(340, 380);
    blocks[2].setScale(1, 0.5);
    blocks_Real_collider[2].setPosition(blocks[2].getPosition().x, blocks[2].getPosition().y + 20);
    blocks_Real_collider[2].setSize(Vector2f(290, height));

    blocks[3].setPosition(-10, 500);
    blocks[3].setScale(1, 0.5);
    blocks_Real_collider[3].setPosition(blocks[3].getPosition().x, blocks[3].getPosition().y + 20);
    blocks_Real_collider[3].setSize(Vector2f(290, height));

    blocks[4].setPosition(1430, 370);
    blocks[4].setScale(1, 0.5);
    blocks_Real_collider[4].setPosition(blocks[4].getPosition().x, blocks[4].getPosition().y + 20);
    blocks_Real_collider[4].setSize(Vector2f(290, height));

    blocks[5].setPosition(650, 255);
    blocks[5].setScale(1, 0.5);
    blocks_Real_collider[5].setPosition(blocks[5].getPosition().x, blocks[5].getPosition().y + 20);
    blocks_Real_collider[5].setSize(Vector2f(290, height));

    blocks[6].setPosition(510, 780);
    blocks[6].setScale(1, 0.5);
    blocks_Real_collider[6].setPosition(blocks[6].getPosition().x, blocks[6].getPosition().y + 20);
    blocks_Real_collider[6].setSize(Vector2f(290, height));


    blocks[7].setPosition(850, 910);
    blocks[7].setScale(1, 0.5);
    blocks_Real_collider[7].setPosition(blocks[7].getPosition().x, blocks[7].getPosition().y + 20);
    blocks_Real_collider[7].setSize(Vector2f(290, height));

    blocks[8].setPosition(1750, 500);
    blocks[8].setScale(0.5, 0.5);
    blocks_Real_collider[8].setPosition(blocks[8].getPosition().x, blocks[8].getPosition().y + 20);
    blocks_Real_collider[8].setSize(Vector2f(145, height));

    blocks[9].setPosition(340, 640);
    blocks[9].setScale(0.5, 0.5);
    blocks_Real_collider[9].setPosition(blocks[9].getPosition().x, blocks[9].getPosition().y + 20);
    blocks_Real_collider[9].setSize(Vector2f(145, height));

    blocks[10].setPosition(680, 590);
    blocks[10].setScale(1, 0.5);
    blocks_Real_collider[10].setPosition(blocks[10].getPosition().x, blocks[10].getPosition().y + 20);
    blocks_Real_collider[10].setSize(Vector2f(290, height));

    blocks[11].setPosition(1200, 780);
    blocks[11].setScale(0.5, 0.5);
    blocks_Real_collider[11].setPosition(blocks[11].getPosition().x, blocks[11].getPosition().y + 20);
    blocks_Real_collider[11].setSize(Vector2f(145, height));

    blocks[12].setPosition(1050, 255);
    blocks[12].setScale(1, 0.5);
    blocks_Real_collider[12].setPosition(blocks[12].getPosition().x, blocks[12].getPosition().y + 20);
    blocks_Real_collider[12].setSize(Vector2f(290, height));
}

void GameOver(Player player) {

    Text DeathScore;
    Font font; font.loadFromFile("Chosen/font/chinese rocks rg.otf");
    DeathScore.setFont(font);

    string DeathScoreString;

    if (!IsLevelTwo) {
        DeathScore.setPosition(1200, 250);
        DeathScoreString = to_string(player.LVL1Score);
        player.LVL1Score = 0;
    }
    else {
        DeathScore.setPosition(1000, 250);
        DeathScoreString = to_string(player.SurvivedTime);
    }
    DeathScore.setString("Score : " + DeathScoreString);
    
    DeathScore.setFillColor(Color::Green);
    DeathScore.setCharacterSize(100);

    while (window.isOpen()) {
        Event e;

        while (window.pollEvent(e)) {

            if (e.type == Event::Closed) {
                window.close();
            }
            if (e.type == Event::KeyPressed) {

                if (e.key.code == Keyboard::Up) {
                    thisGameOverMenu.move_up();
                }
                else if (e.key.code == Keyboard::Down) {
                    thisGameOverMenu.move_down();
                }
                else if (e.key.code == Keyboard::Enter) {

                    if (thisGameOverMenu.pressed() == 0) {
                        SOver.stop();
                        if (IsLevelTwo) {
                            ResetEnemies();
                            LevelTwo(player, Sprites);
                        }
                        else GamePlay();
                        
                    }
                    else if (thisGameOverMenu.pressed() == 1) {
                        SOver.stop();
                        Pname();
                    }
                    else if (thisGameOverMenu.pressed() == 2) {
                        LeaderBoard();
                    }
                    else if (thisGameOverMenu.pressed() == 3) {
                        SOver.stop();
                        main();
                    }
                    else if (thisGameOverMenu.pressed() == 4) {
                        window.close();
                    }
                }
            }
        }
        window.clear();
        thisGameOverMenu.draw(window, IsLevelTwo);
        if (IsLevelTwo) {
            DeathScore.setPosition(1050, -3830);
        }
        window.draw(DeathScore);
        window.display();
    }

}

void SetDrops() {
    tDrops[0].loadFromFile("Chosen/objects/Crystals/9.png");
    tDrops[1].loadFromFile("Chosen/objects/Crystals/14.png");
    tDrops[2].loadFromFile("Chosen/objects/Crystals/13.png");
    tDrops[3].loadFromFile("Chosen/Weapons/vecteezy_ai-generated-weapon-isolated-on-transparent-background-png_36743659.png");

    // health sounds
    healthbuff.loadFromFile("Chosen/Sound/mixkit-winning-a-coin-video-game-2069.wav");
    healthsound.setBuffer(healthbuff);

    for (int i = 0; i < 4; i++) {
        sDrops[i].setTexture(tDrops[i]);
    }

    // adjust scales
    sDrops[0].setScale(0.2, 0.2);
    sDrops[1].setScale(0.2, 0.2);
    sDrops[2].setScale(0.2, 0.2);
    sDrops[3].setScale(0.05, 0.05);
}

void ChooseDrop(RectangleShape blocks_real_collider[], Clock& Addtimer, Clock& DeleteTimer) {


    if (Addtimer.getElapsedTime().asSeconds() >= 5) {

        Drop thisDrop;
        int indexDrop = rand () % 4;
    
        if (IsLevelTwo && indexDrop == 3) indexDrop = 1;

        thisDrop.sDropshape = sDrops[indexDrop]; // set a drop
     
        if (!IsLevelTwo) {
            int targetIndex = rand() % 13;
            thisDrop.targetCollider = blocks_real_collider[targetIndex]; // set a block to collide with
            thisDrop.targetCollider.setPosition(blocks_real_collider[targetIndex].getPosition().x, blocks_real_collider[targetIndex].getPosition().y - 17);

            if (targetIndex != 8 && targetIndex != 9 && targetIndex != 11) {
                thisDrop.sDropshape.setPosition(thisDrop.targetCollider.getPosition().x + 110, -20); // allgn drop above block
            }
            else {
                thisDrop.sDropshape.setPosition(thisDrop.targetCollider.getPosition().x, -20); // allgn drop above block
            }
        }
        else {
            int Random_X_Pos = rand() % 1900;
            thisDrop.sDropshape.setPosition(Random_X_Pos, -4000);
        }




        // initialze dropCollider properties
            // Rifle
        if (indexDrop == 3) {
            thisDrop.dropCollider.setSize(Vector2f(93, 45));

        }
        // Crystal, health, protection
        else if (indexDrop == 0 || indexDrop == 1 || indexDrop == 2) {
            thisDrop.dropCollider.setSize(Vector2f(50, 50));
        }

        thisDrop.dropCollider.setFillColor(Color::Yellow);

        // allign dropCollider on drop

            // Rifle
        if (indexDrop == 3) {
            thisDrop.dropCollider.setPosition(thisDrop.sDropshape.getPosition().x, thisDrop.sDropshape.getPosition().y + 25);
        }
        // Crystal, health, protection
        else if (indexDrop == 0 || indexDrop == 1 || indexDrop == 2) {
            thisDrop.dropCollider.setPosition(thisDrop.sDropshape.getPosition().x + 23, thisDrop.sDropshape.getPosition().y + 20);
        }


        thisDrop.type = indexDrop;        // 0 => crystal, 1 => protection , 2 => health
        dropBag.push_back(thisDrop);
        Addtimer.restart().asSeconds();

    }

    if (DeleteTimer.getElapsedTime().asSeconds() >= 15) {
        if (!dropBag.empty()) {
            dropBag.erase(dropBag.begin());
            DeleteTimer.restart().asSeconds();
        }

    }

}

void DropADrop(float dt, Player& player) {


    for (int i = 0; i < dropBag.size(); i++) {


        // continous allign dropCollider on the drop
            //Rifle
        if (dropBag[i].type == 3) {
            dropBag[i].dropCollider.setPosition(dropBag[i].sDropshape.getPosition().x, dropBag[i].sDropshape.getPosition().y + 25);
        }
        // Crystal, health, protection

        else if (dropBag[i].type == 0 || dropBag[i].type == 1 || dropBag[i].type == 2) {
            dropBag[i].dropCollider.setPosition(dropBag[i].sDropshape.getPosition().x + 23, dropBag[i].sDropshape.getPosition().y + 20);
        }



        // moving the drop always down
        dropBag[i].sDropshape.move(0, 500 * dt);

        
        // handle drop Collsion with block in level two
        if (IsLevelTwo && dropBag[i].dropCollider.getPosition().y >= -3500) dropBag[i].sDropshape.move(0, -500 * dt);

        // handle drop Collsion with block in level one
        else if (dropBag[i].dropCollider.getGlobalBounds().intersects(dropBag[i].targetCollider.getGlobalBounds())) {
            dropBag[i].sDropshape.move(0, -500 * dt);
        }


        // handle player Collsion with drop
        if (player.playerRealcollider.getGlobalBounds().intersects(dropBag[i].dropCollider.getGlobalBounds())) {

            // crystal
            if (dropBag[i].type == 0) {
                if (!IsLevelTwo) player.LVL1Score += 10;
                else player.GameOverTimer += 5;
                healthsound.play();
                dropBag.erase(dropBag.begin() + i);
            }
            // health
            else if (dropBag[i].type == 1) {
                player.health++;
                dropBag.erase(dropBag.begin() + i);

                healthsound.play();
            }
            // protection
            else if (dropBag[i].type == 2) {
                player.ProtectionCooldown = 4;
                player.Protected = 1;
                healthsound.play();
                
                dropBag.erase(dropBag.begin() + i);
            }
            else if (dropBag[i].type == 3) {
                Bullet bullet;
                bullet.bulletCircle.setRadius(10);
                bullet.bulletCircle.setPosition(-30, 0);
                bullet.bulletCircle.setFillColor(Color::White);
                bullet.coolDown = 1;
                bullet.magazine = 50;
                bullet.speed = 30;
                player.CanShoot = 1;

                for (int j = 0; j < bullet.magazine; j++) {
                    player.bullets.push_back(bullet);
                    player.bulletIndex++;
                }
                dropBag.erase(dropBag.begin() + i);

            }
        }

    }
}

void AbbilitiesCoolDown(Player& player) {
    if (player.bulletCooldown > 0) {
        player.bulletCooldown -= 0.1;
    }
    else if (player.bulletCooldown <= 0 && player.CanShoot == 0) {
        player.CanShoot = 1;
    }

    if (player.ProtectionCooldown > 0) {
        player.ProtectionCooldown -= 0.01;
    }
    else {
        player.Protected = 0;
    }
    if (player.DashCoolDown > 0) {
        player.DashCoolDown -= 0.0167;
    }
    else  {
        player.CanDash = 1;
        player.ProtectedDash = 0;
    }
    if (player.TimerCoolDown.getElapsedTime().asSeconds() >= 1 && !player.Protected) {
        player.GameOverTimer--;
        player.TimerCoolDown.restart().asSeconds();
    }
    if (player.TimerCountCoolDown.getElapsedTime().asSeconds() >= 1) {
        player.SurvivedTime++;
        player.TimerCountCoolDown.restart().asSeconds();
    }
}

void Bulletmovement(vector<Bullet> &bullets) {

    for (int i = 0; i < bullets.size(); i++) {

        

        if (bullets[i].move_to_right == 1) bullets[i].bulletCircle.move(bullets[i].speed, 0);
        else if(bullets[i].move_to_right == 0) bullets[i].bulletCircle.move(-1 * bullets[i].speed, 0);


    }
}

void LeaderBoard() {

    ifstream infile;
    infile.open("LeaderBoard.txt", ios::in);

    vector<string> lines;
    string line;

    while (getline(infile, line, '*')) {
        lines.push_back(line);
    }
    infile.close();

    Font font;
    font.loadFromFile("Chosen/font/BreatheFireIv-3zAVZ.ttf");

    vector<Text> text;
    

    for (int i = 0; i < lines.size(); i++) {

        Text t;
        t.setFont(font);
        t.setString(lines[i]);
        t.setFillColor(Color::White);
        t.setCharacterSize(40);
        if (Won) t.setPosition(0, -7320 + (i * 100));
        else if (!IsLevelTwo)  t.setPosition(0, 20 + (i * 100));
        else t.setPosition(0, -4060 + (i * 100));

        text.push_back(t);

    }
    

    while (window.isOpen()) {

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    return;
                }
            }
        }
        
        window.clear();
        
        for (int i = 0; i < lines.size(); i++) {
            window.draw(text[i]);
        }
        window.display();
    }

}

void Cutscene(Player& player,Sprite blocks[], Enemy enemies[]) {

    player.moving = 0;

    // loading texures && sounds

    resetStatus();

    Texture tex[4];

    for (int i = 3; i <= 5; i++) {
        tex[i - 3].loadFromFile("Chosen/Backgrounds/2/" + to_string(i) + ".png");
        Sprites[i - 3].setTexture(tex[i - 3]);

    }
    if (!EnteredLevelTwo) {
        Sprites[0].scale(0.658, 1.34);
        Sprites[0].setPosition(0, -1060);
        Sprites[1].scale(0.5, 1.14);
        Sprites[1].setPosition(0, -1900);
        Sprites[2].scale(0.5, 0.8787);
        Sprites[2].setPosition(0, -2990);

        tex[3].loadFromFile("Chosen/Backgrounds/2/throne room.png");
        Sprites[3].setTexture(tex[3]);
        Sprites[3].setScale(0.5, 0.5);
        Sprites[3].setPosition(0, -4080);
    }



    // text
    Text levelOneCompleted;
    Font font;
    font.loadFromFile("Chosen/font/chinese rocks rg.otf");
    levelOneCompleted.setFont(font);

    levelOneCompleted.setPosition(680, -540);
    levelOneCompleted.setCharacterSize(80);
    levelOneCompleted.setString("LEVEL 1 COMPLETED");

    Text dash("You have unlocked dashing with Right Click", font);
    dash.setPosition(300, -2700);
    dash.setCharacterSize(80);

    bool Acend = 1;



    View PlayerCam(Vector2f(0, 0), Vector2f(window.getSize().x, window.getSize().y));


    player.Ninja.setPosition(950, 540);


    while (window.isOpen()) {



        Event e;
        
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
            
            
        }
        if (player.Ninja.getPosition().y <= -3550) {
            player.Ninja.setPosition(player.Ninja.getPosition().x, -3550);
            Acend = 0;

        }
        else {
            player.Ninja.move(0, -4.7);
            window.setView(PlayerCam);
            PlayerCam.setCenter(player.Ninja.getPosition());
        }
        if (!Acend) {
            
            if (PlayerCam.getCenter().y <= -4080)
            PlayerCam.move(0, -4.7);
            else {
                IsLevelTwo = 1;
                LVL2Instrunctions();
                LevelTwo(player, Sprites);
                
            }
        }



        player.PlayerAnimation();



        // draw ____________________________________________________________________________________________________________________
        window.clear();

        // draw level one
        if (player.Ninja.getPosition().y >= -552 ) {
            window.draw(sback);
            for (int i = 0; i < 13; i++) {

                window.draw(blocks[i]);
            }
            for (int i = 0; i < dropBag.size(); i++) {
                window.draw(dropBag[i].sDropshape);
            }

            for (int i = 0; i < 6; i++) {
                window.draw(enemies[i].sEnemy);

            }
            for (int i = 0; i < player.bullets.size(); i++) {
                window.draw(player.bullets[i].bulletCircle);
            }

            window.draw(player.Ninja);
        }
        ////////
        // draw cutscene sprites
        for (int i = 0; i <= 3; i++) window.draw(Sprites[i]);
            
        window.draw(levelOneCompleted);
        window.draw(dash);
        window.draw(player.Ninja);
        window.display();
    }
}

void BackGroundEnemies(Player& player) {

    Enemy Enemies[12];

    

    Texture tbackgroundEnemy[3];
    for (int i = 0; i < 3; i++) {
        tbackgroundEnemy[i].loadFromFile("Chosen/enemies/PNG/demon/Idle" + to_string(i + 1) + ".png");

    }
    
    int left_x = -20, right_x = 1900;
    int start_y = -3200;

    Enemies[0].sEnemy.setPosition(left_x, -3250); // 0
    Enemies[1].sEnemy.setPosition(left_x, -3350); // 1 
    Enemies[2].sEnemy.setPosition(left_x, -3450); // 2
    Enemies[3].sEnemy.setPosition(left_x, -3900); // 7 
    Enemies[4].sEnemy.setPosition(left_x, -4000); // 8
    Enemies[5].sEnemy.setPosition(left_x, -4100); // 9
    Enemies[6].sEnemy.setPosition(right_x, -3250); // 10
    Enemies[7].sEnemy.setPosition(right_x, -3350); // 11
    Enemies[8].sEnemy.setPosition(right_x, -3450); // 12
    Enemies[9].sEnemy.setPosition(right_x, -3900); // 17
    Enemies[10].sEnemy.setPosition(right_x, -4000); // 18
    Enemies[11].sEnemy.setPosition(right_x, -4100); // 19

    for (int i = 0; i < 12; i++) {
        Enemies[i].sEnemy.setTexture(tbackgroundEnemy[0]);
        if (i > 5) {
            Enemies[i].sEnemy.setScale(-1, 1);
        }

        window.draw(Enemies[i].sEnemy);
    }

    
}

void LevelTwoLoadEnemies() {
    // load demons
    for (int i = 0; i < 6; i++) {

       

        
        // Attack tex
        if (i < 4) { // 4 tex
            for (int j = 0; j < 8; j++) { //  8 demons copy
                demon[j].AttackTex[i].loadFromFile("Chosen/enemies/PNG/demon/Attack" + to_string(i + 1) + ".png");
            }

        }
        // run tex && Death tex
        // 6 tex
        for (int j = 0; j < 8; j++) { // 8 demon copy
            demon[j].RunTex[i].loadFromFile("Chosen/enemies/PNG/demon/Walk" + to_string(i + 1) + ".png");
            demon[j].DeathTex[i].loadFromFile("Chosen/enemies/PNG/demon/Death" + to_string(i + 1) + ".png");
        }

    }

    // load Dragon
    for (int i = 0; i < 6; i++) { // 6 tex
        for (int j = 0; j < 2; j++) { // 2 dragons

            if (i < 3) dragon[j].IdleTex[i].loadFromFile("Chosen/enemies/PNG/dragon/Idle" + to_string(i + 1) + ".png");
            if (i < 4) dragon[j].AttackTex[i].loadFromFile("Chosen/enemies/PNG/dragon/Attack" + to_string(i + 1) + ".png");
            if (i < 5) dragon[j].DeathTex[i].loadFromFile("Chosen/enemies/PNG/dragon/Death" + to_string(i + 1) + ".png");
        }

    }
}

void LevelTwoChooseEnemy(Clock & SetDemonClock, Clock &SetDragonClock, Player player) {

    int randDemon = rand() % 8;

    int DemonGapTime = player.SurvivedTime > 30 ? 2 : 3;
    if (player.SurvivedTime >= 60) DemonGapTime = 1;

    if (SetDemonClock.getElapsedTime().asSeconds() > DemonGapTime) {

        if (!demon[randDemon].allive) {
            // all demons
            demon[randDemon].AttackFrame = 0, demon[randDemon].DeathFrame = 0, demon[randDemon].WalkFrame = 0;
            demon[randDemon].speed = 10;
            demon[randDemon].allive = 1;
            demon[randDemon].Isdead = 0;
            demon[randDemon].AttackRangeBox.setFillColor(Color::Yellow);
            demon[randDemon].AttackDamage = 1;
            demon[randDemon].health = 5;


            demon[randDemon].collider.setFillColor(Color::Blue);

            // left demons
            if (randDemon < 4) {

                demon[randDemon].sEnemy.setScale(2, 2);
                demon[randDemon].sEnemy.setPosition(-200, -3750);
                demon[randDemon].AttackRangeBox.setSize(Vector2f(130, 150));
                demon[randDemon].collider.setSize(Vector2f(120, 175));

            }
            // right demons
            else {
                demon[randDemon].sEnemy.setScale(-2, 2);
                demon[randDemon].sEnemy.setPosition(2120, -3750);
                demon[randDemon].AttackRangeBox.setSize(Vector2f(110, 120));
                demon[randDemon].collider.setSize(Vector2f(130, 200));


            }
        }
        SetDemonClock.restart().asSeconds();
    }

    int randDragon = rand() % 2;
    if (SetDragonClock.getElapsedTime().asSeconds() >= 5) {
        if (!dragon[randDragon].allive) {
            // all dragons
            dragon[randDragon].AttackFrame = 0, dragon[randDragon].DeathFrame = 0, dragon[randDragon].IdleFrame = 0;
            dragon[randDragon].allive = 1;
            dragon[randDragon].Isdead = 0;
            dragon[randDragon].AttackDamage = 3;
            dragon[randDragon].health = 10;
            
            dragon[randDragon].Fire.setSize(Vector2f(40,20));
            dragon[randDragon].Fire.setPosition(0, 0);
            dragon[randDragon].Fire.setFillColor(Color::Red);

            
            dragon[randDragon].collider.setFillColor(Color::Blue);
            


            // left dragons
            if (randDragon == 0) {

                dragon[randDragon].sEnemy.setScale(1.5, 1.5);
                dragon[randDragon].sEnemy.setPosition(-100, -3700);
                dragon[randDragon].collider.setSize(Vector2f(384, 384));
                dragon[randDragon].collider.setSize(Vector2f(200, 100));
                dragon[randDragon].collider.setPosition(dragon[randDragon].sEnemy.getPosition().x + 109, dragon[randDragon].sEnemy.getPosition().y + 182);

            }
            // right dragons
            else {
                dragon[randDragon].sEnemy.setScale(-1.5, 1.5);
                dragon[randDragon].sEnemy.setPosition(2000, -3700);
                dragon[randDragon].collider.setSize(Vector2f(384, 384));
                dragon[randDragon].collider.setSize(Vector2f(200, 100));
                dragon[randDragon].collider.setPosition(dragon[randDragon].sEnemy.getPosition().x - 305, dragon[randDragon].sEnemy.getPosition().y + 185);

            }
        }
        SetDragonClock.restart().asSeconds();
    }
}

void ResetEnemies() {
    for (int i = 0; i < 8; i++)  demon[i].allive = 0;
    for (int i = 0; i < 2; i++) dragon[i].allive = 0;
}

void LevelTwoUpdateEnemies(Player &player) {

    // loop over 8 demons
    for (int i = 0; i < 8; i++) {

        if (demon[i].allive) {
            // player hit demon
            for (int j = 0; j < player.bullets.size(); j++) {
                if (player.bullets[j].bulletCircle.getGlobalBounds().intersects(demon[i].collider.getGlobalBounds())) {
                    
                    player.bullets.erase(player.bullets.begin() + j);
                    demon[i].health--;

                    if (demon[i].health <= 0 && !demon[i].Isdead) {
                        demon[i].Isdead = 1;
                        player.LVL2Score += 5;
                        player.GameOverTimer += 3;
                    }
                }
            }

            // DeathAnimation
            if (demon[i].Isdead && demon[i].DeadClock.getElapsedTime().asSeconds() > 0.125) {
                demon[i].sEnemy.setTexture(demon[i].DeathTex[demon[i].DeathFrame]);
                demon[i].DeathFrame++;
                if (demon[i].DeathFrame == 7) {
                    demon[i].allive = 0;
                    demon[i].sEnemy.setPosition(0, 0);
                }
                demon[i].DeadClock.restart().asSeconds();
            }
            // left demons
            else if (i < 4) {
                // demon hit player
                if (!demon[i].Isdead && demon[i].AttackRangeBox.getGlobalBounds().intersects(player.playerRealcollider.getGlobalBounds())) {

                    if (demon[i].AttackClock.getElapsedTime().asSeconds() >= 0.25) {
                        if (!player.ProtectedDash && !player.Protected) player.health--;
                           
                        // Attack animation
                        demon[i].sEnemy.setTexture(demon[i].AttackTex[demon[i].AttackFrame]);
                        demon[i].AttackFrame++;
                        if (demon[i].AttackFrame == 4) demon[i].AttackFrame = 0;
                        demon[i].AttackClock.restart().asSeconds();


                    }
                }
                // demon movement
                else if (!demon[i].Isdead) {
                    demon[i].sEnemy.move(demon[i].speed, 0);
                    // Walk Animation

                    if (!demon[i].Isdead && demon[i].WalkClock.getElapsedTime().asSeconds() > 0.125) {
                        demon[i].sEnemy.setTexture(demon[i].RunTex[demon[i].WalkFrame]);
                        demon[i].WalkFrame++;
                        if (demon[i].WalkFrame == 6) demon[i].WalkFrame = 0;
                        demon[i].WalkClock.restart().asSeconds();
                    }
                }
            }
            // right demons
            else {
                // demon hit player
                if (demon[i].AttackRangeBox.getGlobalBounds().intersects(player.playerRealcollider.getGlobalBounds())) {
                    // Attack Animation
                    if (!demon[i].Isdead && demon[i].AttackClock.getElapsedTime().asSeconds() >= 0.25) {
                        if(!player.ProtectedDash && !player.Protected)  player.health--;
                           
                        demon[i].sEnemy.setTexture(demon[i].AttackTex[demon[i].AttackFrame]);
                        demon[i].AttackFrame++;
                        if (demon[i].AttackFrame == 4) demon[i].AttackFrame = 0;
                        demon[i].AttackClock.restart().asSeconds();
                    }
                }
                // demon movement
                else if (!demon[i].Isdead) {
                    demon[i].sEnemy.move(-1 * demon[i].speed, 0);

                    if (demon[i].WalkClock.getElapsedTime().asSeconds() > 0.125) {
                        demon[i].sEnemy.setTexture(demon[i].RunTex[demon[i].WalkFrame]);
                        demon[i].WalkFrame++;
                        if (demon[i].WalkFrame == 4) demon[i].WalkFrame = 0;
                        demon[i].WalkClock.restart().asSeconds();
                    }
                }


            }
            if (demon[i].sEnemy.getPosition().x < -205 || demon[i].sEnemy.getPosition().x > 2125) demon[i].allive = 0;
        }


    }
    // loop over 2 dragons
    for (int i = 0; i < 2; i++) {

        if (dragon[i].allive) {



            if (i == 0) {
                dragon[i].Fire.move(15, 0);
            }
            else {
                dragon[i].Fire.move(-15, 0);
            }
            
            // DeathAnimation
            if (dragon[i].Isdead && dragon[i].DeadClock.getElapsedTime().asSeconds() > 0.125) {
                dragon[i].sEnemy.setTexture(dragon[i].DeathTex[dragon[i].DeathFrame]);
                dragon[i].DeathFrame++;
                if (dragon[i].DeathFrame == 5) {
                    dragon[i].allive = 0;
                    dragon[i].sEnemy.setPosition(0, 0);
                }
                dragon[i].DeadClock.restart().asSeconds();


            }
            else if (dragon[i].AttackCoolDown.getElapsedTime().asSeconds() > 3) {

                if (!dragon[i].Isdead && dragon[i].AttackClock.getElapsedTime().asSeconds() >= 0.25) {
                    // Attack animation
                    dragon[i].sEnemy.setTexture(dragon[i].AttackTex[dragon[i].AttackFrame]);
                    dragon[i].AttackFrame++;
                    if (dragon[i].AttackFrame == 4) {
                        if (i == 0)  dragon[i].Fire.setPosition(Vector2f(dragon[i].sEnemy.getPosition().x + 232, dragon[i].sEnemy.getPosition().y + 170));
                        else  dragon[i].Fire.setPosition(Vector2f(dragon[i].sEnemy.getPosition().x - 265, dragon[i].sEnemy.getPosition().y + 229));
                        dragon[i].AttackFrame = 0;
                        dragon[i].AttackCoolDown.restart().asSeconds();
                    }
                    dragon[i].AttackClock.restart().asSeconds();
                }
                
            }
            else if (!dragon[i].Isdead && dragon[i].IdleClock.getElapsedTime().asSeconds() >= 0.33) {
                // Idle animation
                dragon[i].sEnemy.setTexture(dragon[i].IdleTex[dragon[i].IdleFrame]);
                dragon[i].IdleFrame++;
                if (dragon[i].IdleFrame == 3) dragon[i].IdleFrame = 0;
                dragon[i].IdleClock.restart().asSeconds();
            
            }
            //dragon hit player
            if (dragon[i].Fire.getGlobalBounds().intersects(player.playerRealcollider.getGlobalBounds())) {
                if (!player.ProtectedDash && !player.Protected) player.health -= 2;
                dragon[i].Fire.setPosition(0, 0);
            }
            // player hit dragon
            for (int j = 0; j < player.bullets.size(); j++) {
                if (player.bullets[j].bulletCircle.getGlobalBounds().intersects(dragon[i].collider.getGlobalBounds())) {
                    dragon[i].health--;
                    player.bullets.erase(player.bullets.begin() + j);
                    if (dragon[i].health == 0) {
                        dragon[i].Isdead = 1;
                        player.LVL2Score += 10;
                        player.GameOverTimer += 3;
                        
                    }

                }
            }

        }
    }


}

void LevelTwo(Player& player, Sprite Sprites[]) {
    Clock Playerclock, SetDemonClock, SetDragonClock;
    Clock AddTimer, DeleteTimer;
    
    dropBag.clear();

    bfLevelTwo.loadFromFile("Chosen/Sound/Poseidon_s Wrath(MP3_320K).mp3");

    sLevelTwo.setBuffer(bfLevelTwo);
    sLevelTwo.setLoop(true);

    sCutscene.stop();
    sLevelTwo.play();
    EnteredLevelTwo = 1;



    player.health = 10;
    player.groundHeight = -3500;
    player.GameOverTimer = 20;
    player.SurvivedTime = 0;
    player.bulletIndex = -1;

    LevelTwoLoadEnemies();
    ResetEnemies();
    setStatus(player);



    while (window.isOpen()) {


        window.setFramerateLimit(60);

        float dt = Playerclock.restart().asSeconds();

        Event e;

        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
            }
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::Escape) {
                    sLevelTwo.pause();
                    pauseMenu();
                }
            }
        }

        // infinite bulltes
        if (player.bulletIndex == -1) {
            Bullet bullet;
            bullet.bulletCircle.setRadius(10);
            bullet.bulletCircle.setPosition(-30, 0);
            bullet.bulletCircle.setFillColor(Color::White);
            bullet.coolDown = 1;
            bullet.magazine = 50;
            bullet.speed = 30;
            player.CanShoot = 1;

            for (int j = 0; j < bullet.magazine; j++) {
                player.bullets.push_back(bullet);
                player.bulletIndex++;
            }
        }

        player.move(dt);
        player.PlayerAnimation();
        Bulletmovement(player.bullets);
        AbbilitiesCoolDown(player);

        LevelTwoChooseEnemy(SetDemonClock, SetDragonClock, player);
        LevelTwoUpdateEnemies(player);

        ChooseDrop(blocks_real_collider, AddTimer, DeleteTimer);
        DropADrop(dt, player);

        updateStatus(player);




        window.clear();
        // drawings____________________________
        
        // LevelTwo Background
        window.draw(Sprites[3]);

        displayStatus();
        // Bullets
        for (int i = 0; i < player.bullets.size(); i++) window.draw(player.bullets[i].bulletCircle);
        // drops
        for (int i = 0; i < dropBag.size(); i++) window.draw(dropBag[i].sDropshape);
        // demons
        for (int i = 0; i < 8; i++) {

            if (demon[i].allive) {
                // update colliders
                if (i < 4) {
                    
                    demon[i].AttackRangeBox.setPosition(demon[i].sEnemy.getPosition().x + 235, demon[i].sEnemy.getPosition().y + 216);
                     
                        
                        demon[i].collider.setPosition(demon[i].sEnemy.getPosition().x + 120, demon[i].sEnemy.getPosition().y + 190);
                }
                else {
                    demon[i].AttackRangeBox.setPosition(demon[i].sEnemy.getPosition().x - 360, demon[i].sEnemy.getPosition().y + 236);
                    demon[i].collider.setPosition(demon[i].sEnemy.getPosition().x - 245, demon[i].sEnemy.getPosition().y + 190);
                }
                window.draw(demon[i].sEnemy);
            }
        }



        // dragons
        for (int i = 0; i < 2; i++) {
            if (dragon[i].allive) {
                window.draw(dragon[i].Fire);
                window.draw(dragon[i].sEnemy);
            }
        }
        // player
        if (player.Protected) window.draw(player.playerProtection);
        window.draw(player.Ninja);

        ///////////////////////////////
        window.display();

        // Check Death
        if (player.GameOverTimer <= 0 || player.health <= 0) {
            Deaths++;
            player.SurvivedTime += player.LVL1Score + player.LVL2Score;
            player.LVL2Score = 0;

            if (player.SurvivedTime > highestScore) {
                highestScore = player.SurvivedTime;
                
                ofstream offile;
                offile.open("LeaderBoard.txt", ios::app);
                offile << PlayerName << "          " << player.SurvivedTime << '*';
                offile.close();
            }
            sLevelTwo.stop();
            SOver.play();

            this_thread::sleep_for(chrono::milliseconds(2900));
            thisGameOverMenu.SetGameOverMenu();
            
            GameOver(player);
        }
        // check win
        if (player.SurvivedTime >= 90) {
            player.SurvivedTime += player.LVL1Score + player.LVL2Score;
            player.LVL2Score = 0;
            player.LVL1Score = 0;
            if (player.SurvivedTime > highestScore) {
                highestScore = player.SurvivedTime;

                ofstream offile;
                offile.open("LeaderBoard.txt", ios::app);
                offile << PlayerName << "          " << player.SurvivedTime << '*';
                offile.close();
            }
            
            sLevelTwo.stop();
            Completetion(player);
        }
    }
}

void LVL2Instrunctions() {

    Text instructions;
    Font font;
    font.loadFromFile("Chosen/font/chinese rocks rg.otf");
    instructions.setFont(font); instructions.setFillColor(Color::White); instructions.setPosition(0, -4000);
    instructions.setString("You can now dash with right click\nyou are immune to damage when dashing\n\nSURVIVE for 90 seconds and DO NOT let the timer runs out\n\nKILL enemies to EXTEND the timer\n\nPress Enter to continue");
    instructions.setCharacterSize(80);

    while (window.isOpen()) {

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
                break;
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) return;
        }
        window.clear();
        window.draw(Sprites[3]);
        window.draw(instructions);
        displayStatus();
        window.display();
    }
        


            
        

}

void Completetion(Player &player) {

    player.moving = 0;

    Won = 1;

    
    Clock textClocks;

    bfCompletion.loadFromFile("Chosen/Sound/Completion/Bear McCreary - The All-Father _ God of War Ragnarök (Original Soundtrack)(MP3_320K)-[AudioTrimmer.com]-[AudioTrimmer.com].mp3");
    SCompletion.setBuffer(bfCompletion);

    SCompletion.play();
    

    Texture tcompletion[3]; Sprite sCompletion[3];
    tcompletion[0].loadFromFile("Chosen/menu/PNG/1/terrace.png");
    sCompletion[0].setTexture(tcompletion[0]); sCompletion[0].setScale(0.5, 0.5); sCompletion[0].setPosition(0, -5160);

    tcompletion[1].loadFromFile("Chosen/menu/PNG/2/throne room.png");
    sCompletion[1].setTexture(tcompletion[1]); sCompletion[1].setScale(0.5, 0.5); sCompletion[1].setPosition(0, -6240);

    tcompletion[2].loadFromFile("Chosen/menu/PNG/3/Castle.png");
    sCompletion[2].setTexture(tcompletion[2]); sCompletion[2].setScale(0.5, 0.5); sCompletion[2].setPosition(0, -7320);

    Text text[11]; 
    Font fCompletion;
    fCompletion.loadFromFile("Chosen/font/chinese rocks rg.otf");
    for (int i = 0; i < 11; i++) {
        text[i].setFont(fCompletion);
        text[i].setCharacterSize(80);
        text[i].setFillColor(Color::White);
    }
    string score = to_string(player.LVL1Score + player.LVL2Score + player.SurvivedTime);
    text[0].setString("With a  " + score + "  Score"); text[0].setPosition(680, -4620); 

    
    if(Deaths == 0) text[1].setString("And in  1  try");
    else {
        string deaths = to_string(Deaths + 1);
        text[1].setString("And in " + deaths + " tries");
    }
    text[1].setPosition(680, -5640); 

   
    text[2].setString("You");  text[2].setPosition(600, -6820);
    text[3].setString("Have"); text[3].setPosition(750, -6820);
    text[4].setString("Passed"); text[4].setPosition(1000, -6820);
    text[5].setString("The"); text[5].setPosition(750, -6720);
    text[6].setString("Game"); text[6].setPosition(950, -6720);
    text[7].setString("Congratulations!"); text[7].setPosition(500, -7000); text[7].setFillColor(Color::Yellow);
    text[7].setCharacterSize(120);

    for (int i = 8; i <= 10; i++) text[i].setCharacterSize(50);
    text[8].setString("Press T to try again"); text[8].setPosition(50, -6420); text[8].setFillColor(Color::Yellow); 
    text[9].setString("Press L for Leaderboard"); text[9].setPosition(650, -6420); text[9].setFillColor(Color::Yellow); 
    text[10].setString("Press M for MainMenu"); text[10].setPosition(1250, -6420); text[10].setFillColor(Color::Yellow); 


    bool Acend = 1;



    View PlayerCam(Vector2f(0, 0),Vector2f(1920,1080));


    player.Ninja.setPosition(950,player.groundHeight);


    while (window.isOpen()) {


        Event e;

        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                break;
            }
            if (e.type == Event::KeyPressed) {
                if (e.key.code == Keyboard::T && textClocks.getElapsedTime().asMilliseconds() >= 25000) {
                    SCompletion.stop();
                    GamePlay();
                }
                if (e.key.code == Keyboard::L && textClocks.getElapsedTime().asMilliseconds() >= 25000) {
                    LeaderBoard();
                }
                if (e.key.code == Keyboard::M && textClocks.getElapsedTime().asMilliseconds() >= 25000) {
                    SCompletion.stop();
                    main();
                }
            }
        }
        if (player.Ninja.getPosition().y <= -6790) {
            player.Ninja.setPosition(player.Ninja.getPosition().x, -6790);
            Acend = 0;

        }
        else {
            player.Ninja.move(0, -4.7);
            window.setView(PlayerCam);
            PlayerCam.setCenter(player.Ninja.getPosition());
        }
        if (!Acend) {

            if (PlayerCam.getCenter().y <= -7320) {
                PlayerCam.move(0, -4.7);
            }
        }

        player.PlayerAnimation();


        // draw ____________________________________________________________________________________________________________________

        window.clear();

        // draw level one
        if (player.Ninja.getPosition().y >= -5000) {
            // draw level 2 background
            window.draw(Sprites[3]);

            // draw demons
            for (int i = 0; i < 8; i++) if(demon[i].allive) window.draw(demon[i].sEnemy);
                
            // draw drops
            for (int i = 0; i < dropBag.size(); i++) {
                window.draw(dropBag[i].sDropshape);
            }

            // draw dragons
            for (int i = 0; i < 2; i++) if (dragon[i].allive) window.draw(demon[i].sEnemy);
            
            // draw bullets 
            for (int i = 0; i < player.bullets.size(); i++) window.draw(player.bullets[i].bulletCircle);

            window.draw(player.Ninja);
        }
        ////////
        // draw cutscene sprites
        for (int i = 0; i < 3; i++) {
            window.draw(sCompletion[i]);
            if (i < 2) window.draw(text[i]);
        }

        //cout << textClocks.getElapsedTime().asMilliseconds() << endl;
        
        if (textClocks.getElapsedTime().asMilliseconds() >= 13600) window.draw(text[2]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 14600) window.draw(text[3]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 15600) window.draw(text[4]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 16200) window.draw(text[5]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 16800) window.draw(text[6]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 25000) window.draw(text[7]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 18850) window.draw(text[8]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 19700) window.draw(text[9]);
        if (textClocks.getElapsedTime().asMilliseconds() >= 20700) window.draw(text[10]);

        window.draw(player.Ninja);

        window.display();

    }


}
