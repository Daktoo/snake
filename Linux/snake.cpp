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
    float moveDelay;
    sf::Clock clock;

public:
    SnakeGame() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Dakto INC Snake"), dir(RIGHT), score(0), moveDelay(0.15f) {
        snake.push_back({COLS / 2, ROWS / 2});
        placeFood();

        if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
            // fallback: ignore
        }
        scoreText.setFont(font);
        scoreText.setCharacterSize(16);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(5, 0);

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

    void processInput() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W && dir != DOWN) dir = UP;
                else if (event.key.code == sf::Keyboard::S && dir != UP) dir = DOWN;
                else if (event.key.code == sf::Keyboard::A && dir != RIGHT) dir = LEFT;
                else if (event.key.code == sf::Keyboard::D && dir != LEFT) dir = RIGHT;
                else if (event.key.code == sf::Keyboard::Q) window.close();
            }
        }
    }

    void moveSnake() {
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
        }

        for (auto &s : snake) if (s.x == newHead.x && s.y == newHead.y) gameOver();

        snake.insert(snake.begin(), newHead);

        if (newHead.x == food.x && newHead.y == food.y) {
            score++;
            placeFood();
        } else {
            snake.pop_back();
        }
    }

    void gameOver() {
        sf::Text msg("Game Over! Score: " + std::to_string(score), font, 24);
        msg.setFillColor(sf::Color::Red);
        msg.setPosition(50, WINDOW_HEIGHT / 2 - 20);

        window.clear();
        window.draw(msg);
        window.display();
        sf::sleep(sf::seconds(3));
        window.close();
    }

    void render() {
        window.clear(sf::Color::Black);

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
