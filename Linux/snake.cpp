#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int CELL_SIZE = 20;
const int COLS = 30;
const int ROWS = 20;
const int WINDOW_WIDTH = COLS * CELL_SIZE;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { MAIN_MENU, PLAYING, PAUSED, GAME_OVER };

struct Point { int x, y; };

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<Point> snake;
    Point food;
    Direction dir;
    int score;
    sf::Font font;
    sf::Text scoreText;
    sf::Text menuText;
    sf::Text pauseText;
    sf::Text gameOverText;
    float moveDelay;
    sf::Clock clock;
    GameState state;

public:
    SnakeGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dakto INC Snake"),
                  dir(RIGHT), score(0), moveDelay(0.15f), state(MAIN_MENU) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {}
        
        scoreText.setFont(font);
        scoreText.setCharacterSize(16);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(5, 0);

        menuText.setFont(font);
        menuText.setCharacterSize(24);
        menuText.setFillColor(sf::Color::Cyan);
        menuText.setString("Welcome to Dakto Snake!\n\nPress ENTER to Play\nPress Q to Quit");
        menuText.setPosition(50, WINDOW_HEIGHT/3);

        pauseText.setFont(font);
        pauseText.setCharacterSize(24);
        pauseText.setFillColor(sf::Color::Yellow);
        pauseText.setString("Game Paused\n\nPress P to Resume\nPress Q to Quit");
        pauseText.setPosition(70, WINDOW_HEIGHT/3);

        gameOverText.setFont(font);
        gameOverText.setCharacterSize(24);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(50, WINDOW_HEIGHT/3);

        window.setFramerateLimit(60);
    }

    void resetGame() {
        snake.clear();
        snake.push_back({COLS/2, ROWS/2});
        dir = RIGHT;
        score = 0;
        placeFood();
        clock.restart();
    }

    void placeFood() {
        while (true) {
            food.x = rand() % COLS;
            food.y = rand() % ROWS;
            bool collision = false;
            for (auto &s : snake) if (s.x == food.x && s.y == food.y) collision = true;
            if (!collision) break;
        }
    }

    void processInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (state == MAIN_MENU) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        resetGame();
                        state = PLAYING;
                    }
                    if (event.key.code == sf::Keyboard::Q) window.close();
                }
            } 
            else if (state == PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W && dir != DOWN) dir = UP;
                    else if (event.key.code == sf::Keyboard::S && dir != UP) dir = DOWN;
                    else if (event.key.code == sf::Keyboard::A && dir != RIGHT) dir = LEFT;
                    else if (event.key.code == sf::Keyboard::D && dir != LEFT) dir = RIGHT;
                    else if (event.key.code == sf::Keyboard::Q) window.close();
                    else if (event.key.code == sf::Keyboard::P) state = PAUSED;
                }
            }
            else if (state == PAUSED) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::P) state = PLAYING;
                    else if (event.key.code == sf::Keyboard::Q) window.close();
                }
            }
            else if (state == GAME_OVER) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) state = MAIN_MENU;
                    else if (event.key.code == sf::Keyboard::Q) window.close();
                }
            }
        }
    }

    void moveSnake() {
        if (state != PLAYING) return;
        if (clock.getElapsedTime().asSeconds() < moveDelay) return;
        clock.restart();

        Point head = snake.front();
        Point newHead = head;

        switch(dir) {
            case UP: newHead.y--; break;
            case DOWN: newHead.y++; break;
            case LEFT: newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }

        if (newHead.x < 0 || newHead.x >= COLS || newHead.y < 0 || newHead.y >= ROWS) {
            gameOver();
            return;
        }

        for (auto &s : snake) if (s.x == newHead.x && s.y == newHead.y) {
            gameOver();
            return;
        }

        snake.insert(snake.begin(), newHead);

        if (newHead.x == food.x && newHead.y == food.y) {
            score++;
            placeFood();
        } else {
            snake.pop_back();
        }
    }

    void gameOver() {
        gameOverText.setString("Game Over! Score: " + std::to_string(score) +
                               "\n\nPress ENTER for Menu\nPress Q to Quit");
        state = GAME_OVER;
    }

    void render() {
        window.clear(sf::Color::Black);

        if (state == MAIN_MENU) {
            window.draw(menuText);
        } 
        else if (state == PLAYING) {
            sf::RectangleShape rect(sf::Vector2f(CELL_SIZE-1, CELL_SIZE-1));
            rect.setFillColor(sf::Color::Green);
            for (auto &s : snake) {
                rect.setPosition(s.x * CELL_SIZE, s.y * CELL_SIZE);
                window.draw(rect);
            }

            rect.setFillColor(sf::Color::Red);
            rect.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
            window.draw(rect);

            scoreText.setString("Score: " + std::to_string(score));
            window.draw(scoreText);
        }
        else if (state == PAUSED) {
            window.draw(pauseText);
        }
        else if (state == GAME_OVER) {
            window.draw(gameOverText);
        }

        window.display();
    }

    void run() {
        while (window.isOpen()) {
            processInput();
            moveSnake();
            render();
        }
    }
};

int main() {
    srand(time(nullptr));
    SnakeGame game;
    game.run();
    return 0;
}
