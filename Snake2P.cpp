#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <chrono>
#include <ncurses.h>
#include <mutex>
 
class SNAKE {
private:
    static const int SIZE = 40;
    static const char HEAD = 'O';
    static const char BODY = '#';
    static const char FRUIT = '*';
    static const int RUNNING = 1;
    static const int STOPPED = 0;
    const std::vector<std::vector<int>> DIRECTIONS = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
 
    std::vector<std::vector<int>> snake1 = {{10, 10}};
    std::vector<std::vector<int>> snake2 = {{30, 10}};
    std::vector<int> fruit = {20, 10};
    int direction1 = 0; // current direction snake1
    int direction2 = 2; // current direction snake2
    int status = RUNNING; // status of the game
    int winner = 0; // winner condition 
    std::mutex mtx; // mutex for thread-safe access
    bool simple;
 
public:
    bool replay = false;

    //Constructor
    SNAKE(bool simplearg) {
        simple = simplearg;
    }

    void clearScreen() {
        std::cout << "\033[2J\033[H"; // Clear Screen and position the Cursor
        std::cout << "\033[?25l"; // Hide the Cursor
    }
 
    void drawWalls() {
        std::cout << "\x1b[0m"; // set draw color to default
        std::cout << "\033[1;1H┌"; // Wall
        std::cout << "\033[1;" << SIZE << "H┐"; // Wall
        std::cout << "\033[" << SIZE << ";1H└"; // Wall
        std::cout << "\033[" << SIZE << ";" << SIZE << "H┘"; // Wall
        for (int i = 2; i < SIZE; ++i) {
            std::cout << "\033[" << i << ";1H│"; // Wall
            std::cout << "\033[" << i << ";" << SIZE << "H│"; // Wall
            std::cout << "\033[1;" << i << "H─"; // Wall
            std::cout << "\033[" << SIZE << ";" << i << "H─"; // Wall
        }
    }
 
    void drawSnakes() {
        std::cout << "\x1b[31m"; // set draw color to red
        for (int i = snake1.size() -1; i >= 0 ; i--) {
            std::cout << "\033[" << snake1[i][0] << ";" << snake1[i][1] << "H";
            std::cout << (i == 0 ? HEAD : BODY);
        }

        std::cout << "\x1b[33m"; // set draw color to red
        for (int i = snake2.size() -1; i >= 0 ; i--) {
            std::cout << "\033[" << snake2[i][0] << ";" << snake2[i][1] << "H";
            std::cout << (i == 0 ? HEAD : BODY);
        }

        std::cout << "\x1b[0m"; // reset color
    }
 
    void drawFruit() {
        std::cout << "\x1b[32m"; // set draw color to green
        std::cout << "\033[" << fruit[0] << ";" << fruit[1] << "H" << FRUIT;
        std::cout << "\x1b[0m"; // reset color
    }
 
    void checkCollisions() {
        std::lock_guard<std::mutex> lock(mtx); // Protect access to snake and status
        // make queries simpler
        int head_row1 = snake1[0][0];
        int head_col1 = snake1[0][1];
        int head_row2 = snake2[0][0];
        int head_col2 = snake2[0][1];

        // Snake 1
        // Check collision with walls
        if (head_row1 == 1 || head_row1 == SIZE || head_col1 == 1 || head_col1 == SIZE) {
            status = STOPPED;
            winner = 1;
            return;
        }
        // Check collision with fruit
        else if (head_row1 == fruit[0] && head_col1 == fruit[1]) {
            setNewFruitPosition();
            snake1.push_back(snake1.back()); // add a new segment to the snake
            return;
        }
 
        for (int i = 1; i < snake1.size(); i++) {
            // Check collision with itself and other snake
            if (head_row1 == snake1[i][0] && head_col1 == snake1[i][1]) {
                status = STOPPED;
                winner = 2;
            }
            // and snake1 with snake2
            if (head_row2 == snake1[i][0] && head_col2 == snake1[i][1]) {
                status = STOPPED;
                winner = 1;
            }
        }

        // Snake 2
        // Check collision with walls
        if (head_row2 == 1 || head_row2 == SIZE || head_col2 == 1 || head_col2 == SIZE) {
            status = STOPPED;
            winner = 1;
            return;
        }
        // Check collision with fruit
        else if (head_row2 == fruit[0] && head_col2 == fruit[1]) {
            setNewFruitPosition();
            snake2.push_back(snake2.back()); // add a new segment to the snake
            return;
        }
 
        for (int i = 1; i < snake2.size(); i++) {
            // Check collision with itself
            if (head_row2 == snake2[i][0] && head_col2 == snake2[i][1]) {
                status = STOPPED;
                winner = 1;
            }
            // and snake1 with snake2
            if (head_row1 == snake2[i][0] && head_col1 == snake2[i][1]) {
                status = STOPPED;
                winner = 2;
            }
        }

    }
 
    void moveSnake() {
        // Move Snake, from Tail to Head in given direction
        for (int i = snake1.size() - 1; i >= 0; i--) {
            if (i == 0) {
                // HEAD-Case
                snake1[0][0] += DIRECTIONS[direction1][0];
                snake1[0][1] += DIRECTIONS[direction1][1];
            } else {
                snake1[i] = snake1[i - 1];
            }
        }

        for (int i = snake2.size() - 1; i >= 0; i--) {
            if (i == 0) {
                // HEAD-Case
                snake2[0][0] += DIRECTIONS[direction2][0];
                snake2[0][1] += DIRECTIONS[direction2][1];
            } else {
                snake2[i] = snake2[i - 1];
            }
        }
    }
 
    void steerSnake() {
        initscr();              // start ncurses-Mode
        timeout(100);           // set Timeout according to our sleep-time
        keypad(stdscr, TRUE);   // activate arrow-keys
        cbreak();               // activate raw-mode for ncurses
        noecho();               // disable output from keyboard entry
 
        while (status == RUNNING) {
            int input = getch(); // non-blocking variant
 
            // Use a mutex to ensure thread-safe access to direction
            std::lock_guard<std::mutex> lock(mtx);
            if(simple) {
                switch (input) {
                    case KEY_LEFT:
                        direction1 = 3;
                        break;
                    case KEY_RIGHT:
                        direction1 = 1;
                        break;
                    case KEY_UP:
                        direction1 = 2;
                        break;
                    case KEY_DOWN:
                        direction1 = 0;
                        break;
                    case 'a':
                        direction2 = 3;
                        break;
                    case 'd':
                        direction2 = 1;
                        break;
                    case 'w':
                        direction2 = 2;
                        break;
                    case 's':
                        direction2 = 0;
                        break;
                }
            } else {
                switch (input) {
                    case KEY_LEFT:
                        direction1 = (direction1 + 1) % 4;
                        break;
                    case KEY_RIGHT:
                        direction1 = (direction1 + 3) % 4;
                        break;
                    case 'a':
                        direction2 = (direction2 + 1) % 4;
                        break;
                    case 'd':
                        direction2 = (direction2 + 3) % 4;
                        break;
                }
            }
        }
 
        endwin(); // Close ncurses mode
    }
 
    void timeControl() {
        /* set gameespeed */
        int dividend = snake1.size() + snake2.size();
        int speed = (1000 + dividend +1 ) / dividend;
        std::this_thread::sleep_for(std::chrono::milliseconds(speed));
    }

    void initGame() {
        /* init the variables needed */
        setNewFruitPosition();
    }

    void playGame() {
        /* loopfunction for the gameplay */
        std::thread steering_thread([this]() {
            steerSnake();
        });
 
        while (status == RUNNING) {
            timeControl();
            clearScreen();
            drawFruit();
            drawSnakes();
            drawWalls();
            checkCollisions();
            moveSnake();
            std::cout << std::flush; // Needed for a stable drawing
        }
 
        // Stop steering
        steering_thread.join(); // Wait for the steering thread to finish
    }
 
    void setNewFruitPosition() {
        /* set new Position for the next fruit */
        std::mt19937 generator(static_cast<unsigned int>(std::time(0)));
        std::uniform_int_distribution<int> distribution(2, SIZE - 1);
 
        fruit = {distribution(generator), distribution(generator)};
    }

    void drawGameEnd() {
        /* show score */
        std::cout << "\033[" << SIZE+1<< ";" << 5 << "H";
        std::cout << "SCORE 1ST PLAYER: " << snake1.size() - 1 << "\t\t";
        std::cout << "SCORE 2ST PLAYER: " << snake2.size() - 1 << std::endl;
        std::cout << "\033[" << SIZE+2<< ";" << 19 << "H";
        std::cout << "WINNER PLAYER: " << winner << std::endl;
    }

    void askReplay() {
        /* ask for replay */
        initscr();              // start ncurses-Mode
        cbreak();               // activate raw-mode for ncurses 

        char user_in;
        std::cout << "\033[" << SIZE+3<< ";" << 0 << "H";
        std::cout << "Neues Spiel? (j): ";
        std::cin >> user_in;
        replay = (user_in == 'j' || user_in == 'J');

        endwin();
    }

    void endGame(SNAKE &snake) {
        drawGameEnd();
        askReplay();
    }
};

int main(int argc, char* argv[]) {
    bool replay = false;
    bool simplearg = false;

    for(int ac = 1; ac < argc; ++ac) {
        if(strcmp(argv[ac], "-s") == 0 || strcmp(argv[ac], "--simple") == 0) {
            simplearg = true;
        }
    }

    do {
        SNAKE snake(simplearg);
        snake.initGame();
        snake.playGame();
        snake.endGame(snake);
        replay = snake.replay;
    } while (replay);
}