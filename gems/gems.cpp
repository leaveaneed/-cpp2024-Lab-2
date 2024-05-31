#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <ctime>
#include <cmath>

const int WINDOW_WIDTH = 384;
const int WINDOW_HEIGHT = 384;
const int BOARD_SIZE = 6;
const int GEM_SIZE = 64;
const float ANIMATION_DURATION = 0.3f;

struct Gem {
    sf::Sprite sprite;
    int row;
    int col;
    bool isMatched;
    sf::Vector2f startPosition;
    sf::Vector2f targetPosition;
    float animationProgress;
};

void initializeBoard(Gem board[][BOARD_SIZE], const sf::Texture& texture) {
    std::srand(static_cast<unsigned>(std::time(0)));
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            int textureIndex = std::rand() % 4;
            sf::IntRect textureRect(textureIndex * GEM_SIZE, 0, GEM_SIZE, GEM_SIZE);
            board[i][j].sprite.setTexture(texture);
            board[i][j].sprite.setTextureRect(textureRect);
            board[i][j].sprite.setPosition(j * GEM_SIZE, i * GEM_SIZE);
            board[i][j].row = i;
            board[i][j].col = j;
            board[i][j].isMatched = false;
            board[i][j].startPosition = board[i][j].sprite.getPosition();
            board[i][j].targetPosition = board[i][j].sprite.getPosition();
            board[i][j].animationProgress = 1.0f;
        }
    }
}

void drawBoard(sf::RenderWindow& window, Gem board[][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            window.draw(board[i][j].sprite);
        }
    }
}

bool isValidPosition(int row, int col) {
    return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

void swapGems(Gem& gem1, Gem& gem2) {
    gem1.startPosition = gem1.sprite.getPosition();
    gem2.startPosition = gem2.sprite.getPosition();
    gem1.targetPosition = gem2.startPosition;
    gem2.targetPosition = gem1.startPosition;

    gem1.animationProgress = 0.0f;
    gem2.animationProgress = 0.0f;

    int tempRow = gem1.row;
    int tempCol = gem1.col;
    gem1.row = gem2.row;
    gem1.col = gem2.col;
    gem2.row = tempRow;
    gem2.col = tempCol;
}

void checkMatches(Gem board[][BOARD_SIZE]) {
    // Reset all matches
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j].isMatched = false;
        }
    }

    // Check horizontal matches
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE - 2; ++j) {
            if (board[i][j].sprite.getTextureRect() == board[i][j + 1].sprite.getTextureRect() &&
                board[i][j].sprite.getTextureRect() == board[i][j + 2].sprite.getTextureRect()) {
                board[i][j].isMatched = true;
                board[i][j + 1].isMatched = true;
                board[i][j + 2].isMatched = true;
                std::cout << "Horizontal match found at: (" << i << ", " << j << ")\n";
            }
        }
    }

    // Check vertical matches
    for (int j = 0; j < BOARD_SIZE; ++j) {
        for (int i = 0; i < BOARD_SIZE - 2; ++i) {
            if (board[i][j].sprite.getTextureRect() == board[i + 1][j].sprite.getTextureRect() &&
                board[i][j].sprite.getTextureRect() == board[i + 2][j].sprite.getTextureRect()) {
                board[i][j].isMatched = true;
                board[i + 1][j].isMatched = true;
                board[i + 2][j].isMatched = true;
                std::cout << "Vertical match found at: (" << i << ", " << j << ")\n";
            }
        }
    }
}

void removeMatches(Gem board[][BOARD_SIZE], const sf::Texture& texture) {
    for (int j = 0; j < BOARD_SIZE; ++j) {
        int emptyCount = 0; // Счетчик пустых ячеек в столбце
        for (int i = BOARD_SIZE - 1; i >= 0; --i) {
            if (board[i][j].isMatched) {
                for (int k = i; k > 0; --k) {
                    board[k][j].sprite.setTextureRect(board[k - 1][j].sprite.getTextureRect());
                    board[k][j].isMatched = board[k - 1][j].isMatched;
                }
                emptyCount++;
            }
        }
        // Заполняем пустые ячейки новыми случайными гемами сверху
        for (int i = 0; i < emptyCount; ++i) {
            int textureIndex = std::rand() % 4;
            sf::IntRect textureRect(textureIndex * GEM_SIZE, 0, GEM_SIZE, GEM_SIZE);
            board[i][j].sprite.setTexture(texture);
            board[i][j].sprite.setTextureRect(textureRect);
            board[i][j].isMatched = false;
            board[i][j].animationProgress = 0.0f; // Сбрасываем анимацию для нового гема
            board[i][j].startPosition = sf::Vector2f(j * GEM_SIZE, -GEM_SIZE * (emptyCount - i));
            board[i][j].targetPosition = sf::Vector2f(j * GEM_SIZE, i * GEM_SIZE);
        }
    }
}


bool updateBoard(Gem board[][BOARD_SIZE], const sf::Texture& texture) {
    checkMatches(board);
    bool hasMatches = false;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j].isMatched) {
                hasMatches = true;
            }
        }
    }
    if (hasMatches) {
        removeMatches(board, texture);
    }
    return hasMatches;
}

void updateAnimations(Gem board[][BOARD_SIZE], float deltaTime) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j].animationProgress < 1.0f && board[i][j].targetPosition != board[i][j].startPosition) {
                board[i][j].animationProgress += deltaTime / ANIMATION_DURATION;
                if (board[i][j].animationProgress > 1.0f) {
                    board[i][j].animationProgress = 1.0f;
                }
                float t = board[i][j].animationProgress;
                board[i][j].sprite.setPosition(
                    board[i][j].startPosition * (1.0f - t) + board[i][j].targetPosition * t
                );
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "GEMS");

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:/Users/leoni/projects/labs/.vscode/gems/images/background.png")) {
        std::cerr << "Failed to load background texture." << std::endl;
        return -1;
    }
    sf::Sprite background(backgroundTexture);

    sf::Texture gemTexture;
    if (!gemTexture.loadFromFile("C:/Users/leoni/projects/labs/.vscode/gems/images/gems.png")) {
        std::cerr << "Failed to load gem texture." << std::endl;
        return -1;
    }

    Gem board[BOARD_SIZE][BOARD_SIZE];
    initializeBoard(board, gemTexture);

    Gem* selectedGem = nullptr;

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    int row = mousePosition.y / GEM_SIZE;
                    int col = mousePosition.x / GEM_SIZE;

                    if (isValidPosition(row, col)) {
                        if (selectedGem == nullptr) {
                            selectedGem = &board[row][col];
                        } else {
                            if ((abs(selectedGem->row - row) == 1 && selectedGem->col == col) ||
                                (abs(selectedGem->col - col) == 1 && selectedGem->row == row)) {
                                swapGems(*selectedGem, board[row][col]);
                            }
                            selectedGem = nullptr;
                        }
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();

        updateAnimations(board, deltaTime);

        // Проверка и обновление доски, если нет активной анимации
        bool isAnimating = false;
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j].animationProgress < 1.0f) {
                    isAnimating = true;
                    break;
                }
            }
            if (isAnimating) break;
        }

        if (!isAnimating) {
            if (updateBoard(board, gemTexture)) {
                while (updateBoard(board, gemTexture)) {}
            }
        }

        window.clear();
        window.draw(background);
        drawBoard(window, board);
        window.display();
    }

    return 0;
}
