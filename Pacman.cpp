#include <conio.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <vector>

const int numRows = 15;
const int numCols = 20;

class Berry {
public:
	Berry() : x(0), y(0), collected(false) {}

	void spawn(char(&maze)[numRows][numCols]) {
		do {
			x = rand() % numCols;
			y = rand() % numRows;
		} while (maze[y][x] != 0);  

		collected = false;
	}

	void collect() {
		collected = true;
	}

	bool isCollected() const {
		return collected;
	}

	int getX() const {
		return x;
	}

	int getY() const {
		return y;
	}

	void draw() const {
		if (!collected) {
			std::cout << "B";
		}
	}

private:
	int x;
	int y;
	bool collected;
};

class MazeGenerator {
public:
	void generateMaze(char(&maze)[numRows][numCols]) {
		initializeMaze(maze);
		generateRecursiveBacktracker(maze, 3, 3);
	}

private:
	void initializeMaze(char(&maze)[numRows][numCols]) {
		for (int i = 0; i < numRows; ++i) {
			for (int j = 0; j < numCols; ++j) {
				maze[i][j] = 1;
			}
		}
	}

	void generateRecursiveBacktracker(char(&maze)[numRows][numCols], int x, int y) {
		maze[y][x] = 0;

		int directions[4][2] = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };

		for (int i = 0; i < 4; ++i) {
			int randIndex = rand() % 4;
			std::swap(directions[i][0], directions[randIndex][0]);
			std::swap(directions[i][1], directions[randIndex][1]);
		}

		for (int i = 0; i < 4; ++i) {
			int newX = x + directions[i][0];
			int newY = y + directions[i][1];
			if (newX > 0 && newX < numCols - 1 && newY > 0 && newY < numRows - 1) {
				if (maze[newY][newX] == 1) {
					maze[newY][newX] = 0;
					maze[y + directions[i][1] / 2][x + directions[i][0] / 2] = 0;
					generateRecursiveBacktracker(maze, newX, newY);
				}
			}
		}
	}
};

class Ghost {
public:
	Ghost() : spawned(false) {}

	void setMaze(char maze[numRows][numCols]) {
		this->maze = maze;
	}

	void spawn(char(&maze)[numRows][numCols], int pacmanX, int pacmanY) {
		do {
			if (spawned) {
				x = rand() % numCols;
				y = rand() % numRows;
			}
			else {
				spawned = true;
			}
		} while (maze[y][x] != 0 || isTooCloseToPacman(pacmanX, pacmanY));
	}

	int getX() const {
		return x;
	}

	int getY() const {
		return y;
	}

	void move(int dx, int dy) {
		if (maze) {
			int newX = x + dx;
			int newY = y + dy;

			if (isValidPosition(newX, newY)) {
				x = newX;
				y = newY;
			}
		}
	}

private:
	int x;
	int y;
	char(*maze)[numCols];
	bool spawned;

	bool isValidPosition(int newX, int newY) const {
		return newX >= 0 && newX < numCols && newY >= 0 && newY < numRows && maze[newY][newX] == 0;
	}

	bool isTooCloseToPacman(int pacmanX, int pacmanY) const {
		return std::abs(x - pacmanX) < 3 && std::abs(y - pacmanY) < 3;
	}
};

class Pacman {
public: 
	Pacman() : x(1), y(1), maze(nullptr) {}

	void setInitialPosition(int startX, int startY) {

		for (int i = -1; i <= 1; ++i) {
			for (int j = -1; j <= 1; ++j) {
				int checkX = startX + i;
				int checkY = startY + j;

				if (checkX >= 0 && checkX < numCols && checkY >= 0 && checkY < numRows && maze[checkY][checkX] == 0) {
					x = checkX;
					y = checkY;
					return;
				}
			}
		}
	}

	void collectBerries(Berry& berry) {
		if (!berry.isCollected() && x == berry.getX() && y == berry.getY()) {
			berry.collect();
		}
	}

	void setMaze(char(*maze)[numCols]) {
		this->maze = maze;
	}
	void move(int dx, int dy) {
		if (maze) {
			int newX = x + dx;
			int newY = y + dy;

			if (newX >= 0 && newX < numCols && newY >= 0 && newY < numRows && maze[newY][newX] == 0) {
				x = newX;
				y = newY;
				ensureValidPosition();
			}
		}
	}

	int getX() const{
		return x;
	}
	int getY() const{
		return y;
	}
	void setX(int dx) {
		x = dx;
	}
	void setY(int dy) {
		y = dy;
	}

private : 
	int x;
	int y;
	char(*maze)[numCols];

	void ensureValidPosition() {
		x = std::max(0, std::min(x, numCols - 1));
		y = std::max(0, std::min(y, numRows - 1));
	}
};

class GameLevel {
public:
	GameLevel(char(&maze)[numRows][numCols]) : berriesCollected(0), maze(maze) {}

	void generateBerries(int numBerries) {
		for (int i = 0; i < numBerries; ++i) {
			Berry berry;
			berry.spawn(maze);
			berries.push_back(berry);
		}
	}

	void drawLevel(const Pacman& pacman, const Ghost& ghost1, const Ghost& ghost2) const {
		for (int i = 0; i < numRows; ++i) {
			for (int j = 0; j < numCols; ++j) {
				bool berryDrawn = false;
				for (const auto& berry : berries) {
					if (!berry.isCollected() && berry.getX() == j && berry.getY() == i) {
						berry.draw();
						berryDrawn = true;
						break;
					}
				}
				if (!berryDrawn) {
					if (i == pacman.getY() && j == pacman.getX()) {
						std::cout << 'P';
					}
					else if ((i == ghost1.getY() && j == ghost1.getX()) || (i == ghost2.getY() && j == ghost2.getX())) {
						std::cout << 'G';
					}
					else if (maze[i][j] == 1) {
						std::cout << '#';
					}
					else {
						std::cout << '.';
					}
				}
				std::cout << ' ';
			}
			std::cout << '\n';
		}
	}

	int getNumRemainingBerries() const {
		return berries.size();
	}
	void collectBerries(Pacman& pacman) {
		auto it = berries.begin();
		while (it != berries.end()) {
			pacman.collectBerries(*it);
			if (it->isCollected()) {
				berriesCollected++;
				it = berries.erase(it); 
			}
			else {
				++it;
			}
		}
	}

	bool isGameWon() const {
		return berries.empty();
	}

private:
	std::vector<Berry> berries; 
	int berriesCollected;
	char(&maze)[numRows][numCols];
};


class Game {
public:
	Game() : pacman(), ghost1(), ghost2(), mazeGenerator(), gameLevel(maze) {
		srand(static_cast<unsigned>(time(nullptr)));
		mazeGenerator.generateMaze(maze);
		pacman.setMaze(maze);
		gameLevel.generateBerries(10);
	}

	void run() {
		char key;
		initializeGhosts(); 
		while (true) {
			gameLevel.drawLevel(pacman, ghost1, ghost2);
			key = _getch();
			handleKeyPress(key);
			update();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			system("cls");
		}
	}

private:
	int berriesCollected;
	char maze[numRows][numCols];

	MazeGenerator mazeGenerator;

	Pacman pacman;
	Ghost ghost1;
	Ghost ghost2;
	GameLevel gameLevel;


	void spawnGhosts() {
		ghost1.spawn(maze, pacman.getX(), pacman.getY());
		ghost2.spawn(maze, pacman.getX(), pacman.getY());
	}

	void initializeGhosts() {
		ghost1.setMaze(maze);
		ghost2.setMaze(maze);
		spawnGhosts();
	}
	void handleKeyPress(char key) {
		switch (key) {
		case 'w':
			pacman.move(0, -1);
			break;
		case 's':
			pacman.move(0, 1);
			break;
		case 'a':
			pacman.move(-1, 0);
			break;
		case 'd':
			pacman.move(1, 0);
			break;
		default:
			break;
		}
	}

	void update() {
		int moveX1 = rand() % 3 - 1;
		int moveY1 = rand() % 3 - 1;
		int moveX2 = rand() % 3 - 1;
		int moveY2 = rand() % 3 - 1;
		ghost1.move(moveX1, moveY1);
		ghost2.move(moveX2, moveY2);

		gameLevel.collectBerries(pacman);

		if (gameLevel.isGameWon()) {
			std::cout << "You won the game!\n";
			displayScore();
			exit(0);
		}

		if (checkCollision()) {
			std::cout << "Game Over! You collided with a ghost.\n";
			displayScore();
			exit(0);
		}
	}

	bool checkCollision() const {
		return (pacman.getX() == ghost1.getX() && pacman.getY() == ghost1.getY()) ||
			(pacman.getX() == ghost2.getX() && pacman.getY() == ghost2.getY());
	}

	void displayScore() const {
		std::cout << "Berries collected: " << berriesCollected << "\n";
		std::cout << "Berries remaining: " << gameLevel.getNumRemainingBerries() << "\n";
	}
};

int main()
{
	Game game;
	game.run();

	return 0;
}


