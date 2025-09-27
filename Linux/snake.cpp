#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <algorithm>

constexpr unsigned int WINDOW_WIDTH = 800;
constexpr unsigned int WINDOW_HEIGHT = 600;
constexpr int CELL_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

class SnakeGame {
public:
    SnakeGame();
    void run();

private:
    void processInput();
    void update(float dt);
    void render();
    void placeFood();

    sf::RenderWindow window;
    std::vector<sf::Vector2i> snake;
    Direction dir;
    sf::Vector2i food;
    int score;
    float moveDelay;
    float moveTimer;

    sf::Font font;
    sf::Text scoreText;
};

SnakeGame::SnakeGame()
    : window(sf::VideoMode(sf::Vector2u{WINDOW_WIDTH, WINDOW_HEIGHT}), "Snake"),
      dir(RIGHT), score(0), moveDelay(0.15f), moveTimer(0.f) {

    snake.push_back({static_cast<int>(WINDOW_WIDTH / (2 * CELL_SIZE)), static_cast<int>(WINDOW_HEIGHT / (2 * CELL_SIZE))});

    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    scoreText = sf::Text("", font, 24);  // Construct AFTER font is loaded
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: 0");
    scoreText.setPosition(sf::Vector2f(5.f, 0.f));

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    placeFood();
}

void SnakeGame::placeFood() {
    do {
        food.x = std::rand() % (WINDOW_WIDTH / CELL_SIZE);
        food.y = std::rand() % (WINDOW_HEIGHT / CELL_SIZE);
    } while (std::find(snake.begin(), snake.end(), food) != snake.end());
}

void SnakeGame::processInput() {
    while (auto eventOpt = window.pollEvent()) {
        sf::Event event = *eventOpt;

        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::KeyPressed) {
            auto key = event.key.code;
            if (key == sf::Keyboard::W && dir != DOWN) dir = UP;
            else if (key == sf::Keyboard::S && dir != UP) dir = DOWN;
            else if (key == sf::Keyboard::A && dir != RIGHT) dir = LEFT;
            else if (key == sf::Keyboard::D && dir != LEFT) dir = RIGHT;
            else if (key == sf::Keyboard::Q) window.close();
        }
    }
}

void SnakeGame::update(float dt) {
    moveTimer += dt;
    if (moveTimer < moveDelay) return;
    moveTimer = 0.f;

    sf::Vector2i newHead = snake.front();
    switch (dir) {
        case UP:    newHead.y--; break;
        case DOWN:  newHead.y++; break;
        case LEFT:  newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }

    if (newHead.x < 0 || newHead.x >= static_cast<int>(WINDOW_WIDTH / CELL_SIZE) ||
        newHead.y < 0 || newHead.y >= static_cast<int>(WINDOW_HEIGHT / CELL_SIZE) ||
        std::find(snake.begin(), snake.end(), newHead) != snake.end()) {
        window.close();
        return;
    }

    snake.insert(snake.begin(), newHead);

    if (newHead == food) {
        score++;
        scoreText.setString("Score: " + std::to_string(score));
        placeFood();
    } else {
        snake.pop_back();
    }
}

void SnakeGame::render() {
    window.clear();

    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1.f, CELL_SIZE - 1.f));
    cell.setFillColor(sf::Color::Green);
    for (auto &s : snake) {
        cell.setPosition(sf::Vector2f(s.x * CELL_SIZE, s.y * CELL_SIZE));
        window.draw(cell);
    }

    cell.setFillColor(sf::Color::Red);
    cell.setPosition(sf::Vector2f(food.x * CELL_SIZE, food.y * CELL_SIZE));
    window.draw(cell);

    window.draw(scoreText);
    window.display();
}

void SnakeGame::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processInput();
        update(dt);
        render();
    }
}

int main() {
    try {
        SnakeGame game;
        game.run();
    } catch (const std::exception &e) {
        printf("Error: %s\n", e.what());
        return 1;
    }
    return 0;
}
