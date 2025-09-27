#include <SFML/Graphics.hpp>
#include <vector>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int CELL_SIZE = 20;

enum Direction { UP, DOWN, LEFT, RIGHT };

struct SnakeSegment {
    int x, y;
};

class SnakeGame {
private:
    sf::RenderWindow window;
    std::vector<SnakeSegment> snake;
    Direction dir;
    SnakeSegment food;
    int score;
    float moveDelay;
    float moveTimer;

    sf::Font font;
    sf::Text scoreText;

public:
    SnakeGame()
        : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Snake"),
          dir(RIGHT), score(0), moveDelay(0.15f), moveTimer(0.f)
    {
        if (!font.loadFromFile("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf")) {
            // handle font error
        }

        scoreText = sf::Text("Score: 0", font, 24);
        scoreText.setFillColor(sf::Color::White);

        // Initialize snake
        snake.push_back({WINDOW_WIDTH / (2 * CELL_SIZE), WINDOW_HEIGHT / (2 * CELL_SIZE)});
    }

    void processInput() {
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

    void render() {
        window.clear();

        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
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

    void run() {
        sf::Clock clock;
        while (window.isOpen()) {
            float delta = clock.restart().asSeconds();
            moveTimer += delta;

            processInput();


            render();
        }
    }
};

int main() {
    SnakeGame game;
    game.run();
    return 0;
}
