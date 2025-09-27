#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
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
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake"),
      dir(RIGHT), score(0), moveDelay(0.15f), moveTimer(0.f) {

    snake.push_back({WINDOW_WIDTH / (2 * CELL_SIZE), WINDOW_HEIGHT / (2 * CELL_SIZE)});

    font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");

    scoreText = sf::Text("Score: 0", font, 24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({5.f, 0.f});

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
    sf::Event event;
    while (window.pollEvent(event)) {
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
    if (dir == UP) newHead.y--;
    else if (dir == DOWN) newHead.y++;
    else if (dir == LEFT) newHead.x--;
    else if (dir == RIGHT) newHead.x++;

    if (newHead.x < 0 || newHead.x >= WINDOW_WIDTH / CELL_SIZE ||
        newHead.y < 0 || newHead.y >= WINDOW_HEIGHT / CELL_SIZE ||
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

    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
    cell.setFillColor(sf::Color::Green);
    for (auto &s : snake) {
        cell.setPosition(s.x * CELL_SIZE, s.y * CELL_SIZE);
        window.draw(cell);
    }

    cell.setFillColor(sf::Color::Red);
    cell.setPosition(food.x * CELL_SIZE, food.y * CELL_SIZE);
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
    SnakeGame game;
    game.run();
    return 0;
}
