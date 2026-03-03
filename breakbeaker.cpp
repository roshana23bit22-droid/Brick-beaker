#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <conio.h>
#include <windows.h>

using namespace std;

const int WIDTH = 60;
const int HEIGHT = 25;
const int PADDLE_WIDTH = 8;

struct Brick {
    int x, y;
    bool alive;
};

// Hide console cursor
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Move cursor to top-left for smooth redraw
void setCursorTop() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(hConsole, pos);
}

// Clear screen for menu or level message
void clearScreen() {
    system("cls");
}

// Display menu
void showMenu() {
    clearScreen();
    cout << "==============================\n";
    cout << "      BRICK BREAKER GAME      \n";
    cout << "==============================\n";
    cout << "1. Start Game\n";
    cout << "2. Exit\n";
    cout << "Choose option: ";
}

// Generate bricks based on level
void createBricks(vector<Brick>& bricks, int level) {
    bricks.clear();
    int rows = 3 + level; // more rows per level
    for (int y = 3; y < 3 + rows; y++) {
        for (int x = 2; x < WIDTH - 6; x += 6) {
            bricks.push_back({x, y, true});
        }
    }
}

// Draw the game frame
void drawFrame(int paddleX, int paddleY, float ballX, float ballY,
               vector<Brick>& bricks, int score, int lives, int level) {
    string screen = "";

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1)
                screen += "#";
            else if ((int)ballY == y && (int)ballX == x)
                screen += "O";
            else if (y == paddleY &&
                     x >= paddleX &&
                     x <= paddleX + PADDLE_WIDTH)
                screen += "=";
            else {
                bool brickDrawn = false;
                for (auto &brick : bricks) {
                    if (brick.alive &&
                        y == brick.y &&
                        x >= brick.x &&
                        x <= brick.x + 4) {
                        screen += char(219); // solid block
                        brickDrawn = true;
                        break;
                    }
                }
                if (!brickDrawn)
                    screen += " ";
            }
        }
        screen += "\n";
    }

    screen += "Level: " + to_string(level) +
              "  Score: " + to_string(score) +
              "  Lives: " + to_string(lives) + "\n";

    setCursorTop();
    cout << screen;
}

// Play a level
void playGame(int level) {
    vector<Brick> bricks;
    createBricks(bricks, level);

    int paddleX = WIDTH / 2 - PADDLE_WIDTH / 2;
    int paddleY = HEIGHT - 2;

    // Ball speed: slower than previous version
    float speedFactor = 0.3f + level * 0.05f; // start slow
    float ballX = WIDTH / 2;
    float ballY = HEIGHT / 2;
    float ballDX = speedFactor;
    float ballDY = -speedFactor;

    int score = 0;
    int lives = 3;

    while (lives > 0) {
        // Input
        if (_kbhit()) {
            char key = _getch();
            if (key == 'a' && paddleX > 1)
                paddleX -= 2;
            if (key == 'd' && paddleX < WIDTH - PADDLE_WIDTH - 1)
                paddleX += 2;
        }

        // Move ball
        ballX += ballDX;
        ballY += ballDY;

        // Wall collision
        if (ballX <= 1 || ballX >= WIDTH - 2)
            ballDX *= -1;
        if (ballY <= 1)
            ballDY *= -1;

        // Paddle collision
        if ((int)ballY == paddleY - 1 &&
            (int)ballX >= paddleX &&
            (int)ballX <= paddleX + PADDLE_WIDTH)
            ballDY *= -1;

        // Brick collision
        for (auto &brick : bricks) {
            if (brick.alive &&
                (int)ballY == brick.y &&
                (int)ballX >= brick.x &&
                (int)ballX <= brick.x + 4) {
                brick.alive = false;
                ballDY *= -1;
                score += 10;
            }
        }

        // Check win condition
        bool allDestroyed = true;
        for (auto &brick : bricks) {
            if (brick.alive) {
                allDestroyed = false;
                break;
            }
        }

        if (allDestroyed) {
            clearScreen();
            cout << "LEVEL " << level << " COMPLETED!\n";
            if(level < 5) {
                cout << "Next level will be harder!\n";
                _getch();
                playGame(level + 1); // next level
            } else {
                cout << "You completed all levels! Difficulty reduced!\n";
                _getch();
                // Reduce ball speed on replay
                playGame(level);
            }
            return;
        }

        // Ball missed paddle
        if (ballY >= HEIGHT - 1) {
            lives--;
            ballX = WIDTH / 2;
            ballY = HEIGHT / 2;
            // Reduce speed slightly if last level
            if(level >= 5){
                ballDX *= 0.7f;
                ballDY *= 0.7f;
            } else {
                ballDX = speedFactor;
                ballDY = -speedFactor;
            }
        }

        // Draw frame
        drawFrame(paddleX, paddleY, ballX, ballY, bricks, score, lives, level);

        // Smooth frame rate (~60 FPS)
        this_thread::sleep_for(chrono::milliseconds(16));
    }

    clearScreen();
    cout << "GAME OVER!\n";
    cout << "Press any key to return to menu...";
    _getch();
}

// Main
int main() {
    hideCursor();

    while (true) {
        showMenu();
        char choice;
        cin >> choice;

        if (choice == '1') {
            playGame(1);
        }
        else if (choice == '2') {
            break;
        }
    }

    return 0;
}