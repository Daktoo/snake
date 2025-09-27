#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>

const int CELL_SIZE = 20;
const int COLS = 30;
const int ROWS = 20;
const int WINDOW_WIDTH = COLS * CELL_SIZE;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { MAIN_MENU, PLAYING, PAUSED, GAME_OVER };

struct Point { int x, y; };

class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;

    Button(const sf::Font& font, const std::string& label, float x, float y, float w=200, float h=50) {
        rect.setSize({w, h});
        rect.setFillColor(sf::Color(50, 50, 50));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2);
        rect.setPosition(x, y);

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);

        sf::FloatRect tb = text.getLocalBounds();
        text.setOrigin(tb.width/2, tb.height/2);
        text.setPosition(x + w/2, y + h/2 - 5);
    }

    bool contains(sf::Vector2f pos) const {
        return rect.getGlobalBounds().contains(pos);
    }

    void draw(sf::RenderWindow& win) {
        win.draw(rect);
        win.draw(text);
    }
};

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<Point> snake;
    Point food;
    Direction dir;
    int score;
    int highScore;
    sf::Font font;
    sf::Text scoreText;
    float moveDelay;
    sf::Clock clock;
    GameState state;

    Button* playBtn;
    Button* quitBtn;
    Button* resumeBtn;
    Button* menuBtn;

public:
    SnakeGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dakto INC Snake"),
                  dir(RIGHT), score(0), moveDelay(0.15f), state(MAIN_MENU),
                  playBtn(nullptr), quitBtn(nullptr), resumeBtn(nullptr), menuBtn(nullptr) {
        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {}

        scoreText.setFont(font);
        scoreText.setCharacterSize(16);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(5, 0);

        playBtn   = new Button(font, "Play", 100, 150);
        quitBtn   = new Button(font, "Quit", 100, 220);
        resumeBtn = new Button(font, "Resume", 100, 150);
        menuBtn   = new Button(font, "Main Menu", 100, 220);

        loadHighScore();
        window.setFramerateLimit(60);
    }

    ~SnakeGame() {
        delete playBtn;
        delete quitBtn;
        delete resumeBtn;
        delete menuBtn;
    }

    void resetGame() {
        snake.clear();
        snake.push_back({COLS/2, ROWS/2});
        dir = RIGHT;
        score = 0;
        placeFood();
        clock.restart();
    }

    void loadHighScore() {
        std::ifstream f("highscore.txt");
        if (f) f >> highScore;
        else highScore = 0;
    }

    void saveHighScore() {
        if (score > highScore) {
            highScore = score;
            std::ofstream f("highscore.txt");
            f << highScore;
        }
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
                if (event.type == sf::Event::MouseButtonPressed) {
                    auto mp = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                    if (playBtn->contains(mp)) { resetGame(); state = PLAYING; }
                    if (quitBtn->contains(mp)) { window.close(); }
                }
            }
            else if (state == PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W && dir != DOWN) dir = UP;
                    else if (event.key.code == sf::Keyboard::S && dir != UP) dir = DOWN;
                    else if (event.key.code == sf::Keyboard::A && dir != RIGHT) dir = LEFT;
                    else if (event.key.code == sf::Keyboard::D && dir != LEFT) dir = RIGHT;
                    else if (event.key.code == sf::Keyboard::P) state = PAUSED;
                }
            }
            else if (state == PAUSED) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    auto mp = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                    if (resumeBtn->contains(mp)) { state = PLAYING; }
                    if (quitBtn->contains(mp)) { window.close(); }
                }
            }
            else if (state == GAME_OVER) {
                if (!playAgainBtn) {
                    playAgainBtn = new Button(font, "Play Again", {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2}, {200, 50});}
                }
                if (!menuBtn) {
                    menuBtn = new Button(font, "Main Menu", {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 70}, {200, 50});
                }
                if (!quitBtn) {
                    quitBtn = new Button(font, "Quit", {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 140}, {200, 50});
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    auto mp = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});
                    if (playAgainBtn->contains(mp)) {
                        resetGame();
                        state = PLAYING;
                    }
                    if (menuBtn->contains(mp)) {
                        state = MAIN_MENU;
                    }
                    if (quitBtn->contains(mp)) {
                        window.close();
                    }
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
            gameOver(); return;
        }

        for (auto &s : snake) if (s.x == newHead.x && s.y == newHead.y) {
            gameOver(); return;
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
        saveHighScore();
        state = GAME_OVER;
    }

    void render() {
        window.clear(sf::Color::Black);

        if (state == MAIN_MENU) {
            sf::Text title("Dakto Snake", font, 32);
            title.setFillColor(sf::Color::Cyan);
            title.setPosition(80, 50);
            window.draw(title);
            playBtn->draw(window);
            quitBtn->draw(window);

            sf::Text hs("High Score: " + std::to_string(highScore), font, 18);
            hs.setPosition(100, 300);
            window.draw(hs);
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
            sf::Text paused("Game Paused", font, 28);
            paused.setFillColor(sf::Color::Yellow);
            paused.setPosition(80, 80);
            window.draw(paused);

            resumeBtn->draw(window);
            quitBtn->draw(window);
        }
        else if (state == GAME_OVER) {
            sf::Text msg("Game Over. Score: " + std::to_string(score), font, 24);
            msg.setFillColor(sf::Color::Red);
            msg.setPosition(WINDOW_WIDTH/2 - msg.getLocalBounds().width/2, WINDOW_HEIGHT/2 - 100);

            window.draw(msg);
            playAgainBtn->draw(window);
            menuBtn->draw(window);
            quitBtn->draw(window);
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
