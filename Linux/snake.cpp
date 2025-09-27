#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int CELL_SIZE = 20;
enum Direction { UP, DOWN, LEFT, RIGHT };

class SnakeGame {
    sf::RenderWindow window;
    std::vector<sf::Vector2i> snake;
    sf::Vector2i food;
    Direction dir;
    float moveDelay;
    float moveTimer;
    int score;
    sf::Font font;
    sf::Text scoreText;

public:
    SnakeGame() : window(sf::VideoMode(sf::Vector2u{WINDOW_WIDTH, WINDOW_HEIGHT}), "Dakto INC Snake"), dir(RIGHT), score(0), moveDelay(0.15f), moveTimer(0.f) {
        snake.push_back(sf::Vector2i(WINDOW_WIDTH / (2 * CELL_SIZE), WINDOW_HEIGHT / (2 * CELL_SIZE)));
        placeFood();
        font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
        scoreText = sf::Text("Score: 0", font, 24);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(sf::Vector2f(5.f, 0.f));
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
    }

    void placeFood() {
        while (true) {
            food.x = std::rand() % (WINDOW_WIDTH / CELL_SIZE);
            food.y = std::rand() % (WINDOW_HEIGHT / CELL_SIZE);
            bool collision = false;
            for (auto& s : snake) if (s == food) collision = true;
            if (!collision) break;
        }
    }

    void processInput() {
        while (auto eventOpt = window.pollEvent()) {
            if (!eventOpt.has_value()) break;
            auto& event = *eventOpt;
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::W && dir != DOWN) dir = UP;
                else if (event.key.code == sf::Keyboard::S && dir != UP) dir = DOWN;
                else if (event.key.code == sf::Keyboard::A && dir != RIGHT) dir = LEFT;
                else if (event.key.code == sf::Keyboard::D && dir != LEFT) dir = RIGHT;
                else if (event.key.code == sf::Keyboard::Q) window.close();
            }
        }
    }

    void update(float dt) {
        moveTimer += dt;
        if (moveTimer < moveDelay) return;
        moveTimer = 0.f;

        sf::Vector2i newHead = snake.front();
        switch (dir) {
            case UP: newHead.y -= 1; break;
            case DOWN: newHead.y += 1; break;
            case LEFT: newHead.x -= 1; break;
            case RIGHT: newHead.x += 1; break;
        }

        if (newHead.x < 0 || newHead.x >= WINDOW_WIDTH / CELL_SIZE || newHead.y < 0 || newHead.y >= WINDOW_HEIGHT / CELL_SIZE) {
            window.close();
            return;
        }

        for (auto& s : snake) if (s == newHead) { window.close(); return; }

        snake.insert(snake.begin(), newHead);

        if (newHead == food) {
            score++;
            placeFood();
        } else {
            snake.pop_back();
        }

        scoreText.setString("Score: " + std::to_string(score));
    }

    void render() {
        window.clear(sf::Color::Black);
        sf::RectangleShape rect(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        rect.setFillColor(sf::Color::Red);
        rect.setPosition(sf::Vector2f(food.x * CELL_SIZE, food.y * CELL_SIZE));
        window.draw(rect);

        rect.setFillColor(sf::Color::Green);
        for (auto& s : snake) {
            rect.setPosition(sf::Vector2f(s.x * CELL_SIZE, s.y * CELL_SIZE));
            window.draw(rect);
        }

        window.draw(scoreText);
        window.display();
    }

    void run() {
        sf::Clock clock;
        while (window.isOpen()) {
            float dt = clock.restart().asSeconds();
            processInput();
            update(dt);
            render();
        }
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}
