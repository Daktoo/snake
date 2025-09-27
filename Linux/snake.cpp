#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

const int CELL_SIZE = 20;
const int COLS = 30;
const int ROWS = 20;
const int WINDOW_WIDTH = COLS * CELL_SIZE;
const int WINDOW_HEIGHT = ROWS * CELL_SIZE;

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { MAIN_MENU, PLAYING, PAUSED, GAME_OVER };

struct Point { int x, y; };

class Button {
private:
    sf::RectangleShape rect;
    sf::Text text;
public:
    Button(const sf::Font& font, const std::string& label, float x, float y, float w=200, float h=50) {
        rect.setPosition(x, y);
        rect.setSize(sf::Vector2f(w, h));
        rect.setFillColor(sf::Color(100, 100, 100));
        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::White);
        sf::FloatRect tBounds = text.getLocalBounds();
        text.setOrigin(tBounds.left + tBounds.width / 2, tBounds.top + tBounds.height / 2);
        text.setPosition(x + w / 2, y + h / 2);
    }
    void draw(sf::RenderWindow& window) {
        window.draw(rect);
        window.draw(text);
    }
    bool contains(sf::Vector2f point) {
        return rect.getGlobalBounds().contains(point);
    }
};

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<Point> snake;
    Point food;
    Direction dir;
    int score;
    sf::Font font;
    sf::Text scoreText;
    sf::Clock moveClock;
    float moveDelay;
    GameState state;
    Button* playBtn;
    Button* quitBtn;
    Button* resumeBtn;
    Button* playAgainBtn;
    Button* menuBtn;

public:
    SnakeGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dakto INC Snake"), dir(RIGHT), score(0), moveDelay(0.15f), state(MAIN_MENU) {
        snake.push_back({COLS / 2, ROWS / 2});
        placeFood();
        font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
        scoreText.setFont(font);
        scoreText.setCharacterSize(16);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10, 10);

        playBtn = new Button(font, "Play", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 70);
        quitBtn = new Button(font, "Quit", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 10);
        resumeBtn = new Button(font, "Resume", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 70);
        playAgainBtn = new Button(font, "Play Again", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 60);
        menuBtn = new Button(font, "Main Menu", WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 + 20);

        window.setFramerateLimit(60);
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

    void resetGame() {
        snake.clear();
        snake.push_back({COLS / 2, ROWS / 2});
        dir = RIGHT;
        score = 0;
        placeFood();
        moveClock.restart();
    }

    void processInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (state == PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::W && dir != DOWN) dir = UP;
                    else if (event.key.code == sf::Keyboard::S && dir != UP) dir = DOWN;
                    else if (event.key.code == sf::Keyboard::A && dir != RIGHT) dir = LEFT;
                    else if (event.key.code == sf::Keyboard::D && dir != LEFT) dir = RIGHT;
                    else if (event.key.code == sf::Keyboard::P) state = PAUSED;
                }
            } else if (state == PAUSED) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) state = PLAYING;
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mp(event.mouseButton.x, event.mouseButton.y);
                    if (resumeBtn->contains(mp)) state = PLAYING;
                    else if (menuBtn->contains(mp)) state = MAIN_MENU;
                }
            } else if (state == MAIN_MENU || state == GAME_OVER) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mp(event.mouseButton.x, event.mouseButton.y);
                    if (state == MAIN_MENU) {
                        if (playBtn->contains(mp)) { resetGame(); state = PLAYING; }
                        else if (quitBtn->contains(mp)) window.close();
                    } else if (state == GAME_OVER) {
                        if (playAgainBtn->contains(mp)) { resetGame(); state = PLAYING; }
                        else if (menuBtn->contains(mp)) state = MAIN_MENU;
                    }
                }
            }
        }
    }

    void moveSnake() {
        if (state != PLAYING) return;
        if (moveClock.getElapsedTime().asSeconds() < moveDelay) return;
        moveClock.restart();
        Point head = snake.front();
        Point newHead = head;
        switch(dir) {
            case UP: newHead.y--; break;
            case DOWN: newHead.y++; break;
            case LEFT: newHead.x--; break;
            case RIGHT: newHead.x++; break;
        }
        if (newHead.x < 0 || newHead.x >= COLS || newHead.y < 0 || newHead.y >= ROWS) { state = GAME_OVER; return; }
        for (auto &s : snake) if (s.x == newHead.x && s.y == newHead.y) { state = GAME_OVER; return; }
        snake.insert(snake.begin(), newHead);
        if (newHead.x == food.x && newHead.y == food.y) { score++; placeFood(); }
        else snake.pop_back();
    }

    void render() {
        window.clear(sf::Color::Black);
        if (state == MAIN_MENU) {
            sf::Text title("Dakto INC Snake", font, 48);
            title.setFillColor(sf::Color::White);
            sf::FloatRect tBounds = title.getLocalBounds();
            title.setOrigin(tBounds.left + tBounds.width/2, tBounds.top + tBounds.height/2);
            title.setPosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 150);
            window.draw(title);
            playBtn->draw(window);
            quitBtn->draw(window);
        } else if (state == PLAYING || state == PAUSED) {
            sf::RectangleShape rect(sf::Vector2f(CELL_SIZE-1, CELL_SIZE-1));
            rect.setFillColor(sf::Color::Green);
            for (auto &s : snake) {
                rect.setPosition(s.x * CELL_SIZE + (WINDOW_WIDTH - COLS*CELL_SIZE)/2, s.y * CELL_SIZE + (WINDOW_HEIGHT - ROWS*CELL_SIZE)/2);
                window.draw(rect);
            }
            rect.setFillColor(sf::Color::Red);
            rect.setPosition(food.x * CELL_SIZE + (WINDOW_WIDTH - COLS*CELL_SIZE)/2, food.y * CELL_SIZE + (WINDOW_HEIGHT - ROWS*CELL_SIZE)/2);
            window.draw(rect);
            scoreText.setString("Score: " + std::to_string(score));
            sf::FloatRect sBounds = scoreText.getLocalBounds();
            scoreText.setOrigin(0,0);
            scoreText.setPosition(WINDOW_WIDTH/2 - sBounds.width/2, 10);
            window.draw(scoreText);
            if (state == PAUSED) {
                sf::Text paused("Game Paused", font, 32);
                paused.setFillColor(sf::Color::Yellow);
                sf::FloatRect tBounds = paused.getLocalBounds();
                paused.setOrigin(tBounds.left + tBounds.width/2, tBounds.top + tBounds.height/2);
                paused.setPosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 90);
                window.draw(paused);
                resumeBtn->draw(window);
            }
        } else if (state == GAME_OVER) {
            sf::Text msg("Game Over. Score: " + std::to_string(score), font, 32);
            msg.setFillColor(sf::Color::Red);
            sf::FloatRect tBounds = msg.getLocalBounds();
            msg.setOrigin(tBounds.left + tBounds.width/2, tBounds.top + tBounds.height/2);
            msg.setPosition(WINDOW_WIDTH/2, WINDOW_HEIGHT/2 - 120);
            window.draw(msg);
            playAgainBtn->draw(window);
            menuBtn->draw(window);
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
