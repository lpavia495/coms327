#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#define WIDTH 80
#define HEIGHT 21
#define MAX_MAPS 400
#define PATH '#'
#define POKEMON_CENTER 'C'
#define POKEMART 'M'
#define TALL_GRASS ':'
#define WATER '~'
#define CLEARING '.'
#define BOULDER '%'
#define TREE '^'
#define PLAYER '@'
#define HIKER 'H'
#define RIVAL 'R'
#define PACER 'P'
#define WANDERER 'W'
#define SENTRY 'S'
#define EXPLORER 'E'
int pacerX = -1; // Initialize with invalid position
int pacerY = -1;
int pacerDirection = 1; // 1 for moving right, -1 for moving left
int explorerDirection = 1;
int wandererDirection = 1;
#define INF 9999
char prevPacerCell = CLEARING;
char prevExplorerCell = CLEARING; // Initialize with default terrain type
char prevHikerCell = CLEARING; // Initialize with default terrain type
char prevRivalCell = CLEARING; // Initialize with default terrain type


typedef struct {
    char cells[HEIGHT][WIDTH];
} Map;

typedef struct {
    int col, row; // Node position
    int dist; // Distance from source
    int visited; // Visited flag
} Node;

typedef struct {
    Node nodes[HEIGHT][WIDTH];
} DistanceMap;

// 2D array of pointers to Map
Map* maps[MAX_MAPS][MAX_MAPS]; 
// Start in the center of the map grid
int currentX = MAX_MAPS / 2; 
int currentY = MAX_MAPS / 2;

void handle_signal(int sig) {
    printf("\nCaught signal %d, exiting...\n", sig);
    exit(0);  // Replace 0 with your desired exit code
}

void initializeMap(char map[HEIGHT][WIDTH]) {
    // Fill borders with boulders and inside with clearings
    for (int row = 0; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            // Set boulders at borders
            if (row == 0 || row == HEIGHT - 1 || col == 0 || col == WIDTH - 1) {
                map[row][col] = BOULDER; 
            // Fill inside of map with clearings
            } else {
                map[row][col] = CLEARING; 
            }
        }
    }
}


void createPath(char map[HEIGHT][WIDTH], char path, int connectNorth, int connectEast, int connectSouth, int connectWest) {
    // Initial position for North to South path
    int nsPathX = rand() % (WIDTH - 3) + 2;
    int prevNsPathX = nsPathX;
    int startX;
    int endX;
    int startY;
    int endY;
    int i;
    // Initial position for East to West path
    int ewPathY = rand() % (HEIGHT - 3) + 2;
    int prevEwPathY = ewPathY;

    // Adjust nsPathX or ewPathY if there's a connecting path from the adjacent map
    if (connectNorth >= 0) {
        nsPathX = connectNorth;
        //Makes connection at the north border
        map[0][nsPathX] = path; 
        // Create bendy N-S path
        for (int row = 0; row < HEIGHT; row++) {
            map[row][nsPathX] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevNsPathX = nsPathX;
                // -1 = Move path left, 1 = right, or 0 = stay the same
                nsPathX += rand() % 3 - 1;
                // Bounds checking
                if (nsPathX < 1) {
                    nsPathX = 1;
                }
                if (nsPathX >= WIDTH - 1) {
                    nsPathX = WIDTH - 2;
                }
                // Makes paths walkable horizontally when there is a bend
                startX = (prevNsPathX < nsPathX) ? prevNsPathX : nsPathX;
                endX = (prevNsPathX > nsPathX) ? prevNsPathX : nsPathX;
                for (int col = startX; col <= endX; col++) {
                    map[row][col] = path;
                }
            }
        }
    }
    if (connectSouth >= 0) {
        nsPathX = connectSouth;
        //Makes connection at the south border
        map[HEIGHT - 1][nsPathX] = path; 
        // Create bendy N-S path
        for (int row = HEIGHT; row >= 0; row--) {
            map[row][nsPathX] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevNsPathX = nsPathX;
                // -1 = Move path left, 1 = right, or 0 = stay the same
                nsPathX += rand() % 3 - 1;
                // Bounds checking
                if (nsPathX < 1) {
                    nsPathX = 1;
                }
                if (nsPathX >= WIDTH - 1) {
                    nsPathX = WIDTH - 2;
                }
                // Makes paths walkable horizontally when there is a bend
                startX = (prevNsPathX < nsPathX) ? prevNsPathX : nsPathX;
                endX = (prevNsPathX > nsPathX) ? prevNsPathX : nsPathX;
                for (int col = startX; col <= endX; col++) {
                    map[row][col] = path;
                }
            }
        }
    }
    if (connectWest >= 0) {
        ewPathY = connectWest;
        //Makes connection at the west border
        map[ewPathY][0] = path; 
        // Create bendy E-W path
        for (int x = 0; x < WIDTH; x++) {
            map[ewPathY][x] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevEwPathY = ewPathY;
                // -1 = Move path up, 1 = down, or 0 = stay the same
                ewPathY += rand() % 3 - 1;
                // Bounds checking
                if (ewPathY < 1) {
                    ewPathY = 1;
                }
                if (ewPathY >= HEIGHT - 1) {
                    ewPathY = HEIGHT - 2;
                }
                // Makes paths walkable vertically when there is a bend
                startY = (prevEwPathY < ewPathY) ? prevEwPathY : ewPathY;
                endY = (prevEwPathY > ewPathY) ? prevEwPathY : ewPathY;
                for (int y = startY; y <= endY; y++) {
                    map[y][x] = path;
                }
            }
        }
    }
    if (connectEast >= 0) {
        ewPathY = connectEast;
        //Makes connection at the east border
        map[ewPathY][WIDTH - 1] = path; 
        // Create bendy E-W path
        for (int x = WIDTH-1; x >= 0; x--) {
            map[ewPathY][x] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevEwPathY = ewPathY;
                // -1 = Move path up, 1 = down, or 0 = stay the same
                ewPathY += rand() % 3 - 1;
                // Bounds checking
                if (ewPathY < 1) {
                    ewPathY = 1;
                }
                if (ewPathY >= HEIGHT - 1) {
                    ewPathY = HEIGHT - 2;
                }
                // Makes paths walkable vertically when there is a bend
                startY = (prevEwPathY < ewPathY) ? prevEwPathY : ewPathY;
                endY = (prevEwPathY > ewPathY) ? prevEwPathY : ewPathY;
                for (int y = startY; y <= endY; y++) {
                    map[y][x] = path;
                }
            }
        }
    }

    if (connectNorth == -1 && connectSouth == -1 && connectWest == -1 && connectEast == -1){
        // Create bendy N-S path
        for (int row = 0; row < HEIGHT; row++) {
            map[row][nsPathX] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevNsPathX = nsPathX;
                // -1 = Move path left, 1 = right, or 0 = stay the same
                nsPathX += rand() % 3 - 1;
                // Bounds checking
                if (nsPathX < 1) {
                    nsPathX = 1;
                }
                if (nsPathX >= WIDTH - 1) {
                    nsPathX = WIDTH - 2;
                }
                // Makes paths walkable horizontally when there is a bend
                startX = (prevNsPathX < nsPathX) ? prevNsPathX : nsPathX;
                endX = (prevNsPathX > nsPathX) ? prevNsPathX : nsPathX;
                for (int col = startX; col <= endX; col++) {
                    map[row][col] = path;
                }
            }
        }
        // Create bendy E-W path
        for (int x = 0; x < WIDTH; x++) {
            map[ewPathY][x] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevEwPathY = ewPathY;
                // -1 = Move path up, 1 = down, or 0 = stay the same
                ewPathY += rand() % 3 - 1;
                // Bounds checking
                if (ewPathY < 1) {
                    ewPathY = 1;
                }
                if (ewPathY >= HEIGHT - 1) {
                    ewPathY = HEIGHT - 2;
                }
                // Makes paths walkable vertically when there is a bend
                startY = (prevEwPathY < ewPathY) ? prevEwPathY : ewPathY;
                endY = (prevEwPathY > ewPathY) ? prevEwPathY : ewPathY;
                for (int y = startY; y <= endY; y++) {
                    map[y][x] = path;
                }
            }
        }

    } 

    if (connectNorth == -1 && connectSouth == -1 && connectWest != -1 && connectEast == -1){
        // Create bendy N-S path
        for (int row = 0; row < HEIGHT; row++) {
            map[row][nsPathX] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevNsPathX = nsPathX;
                // -1 = Move path left, 1 = right, or 0 = stay the same
                nsPathX += rand() % 3 - 1;
                // Bounds checking
                if (nsPathX < 1) {
                    nsPathX = 1;
                }
                if (nsPathX >= WIDTH - 1) {
                    nsPathX = WIDTH - 2;
                }
                // Makes paths walkable horizontally when there is a bend
                startX = (prevNsPathX < nsPathX) ? prevNsPathX : nsPathX;
                endX = (prevNsPathX > nsPathX) ? prevNsPathX : nsPathX;
                for (int col = startX; col <= endX; col++) {
                    map[row][col] = path;
                }
            }
        }
        
    }

    if (connectNorth == -1 && connectSouth == -1 && connectWest == -1 && connectEast != -1){
        // Create bendy N-S path
        for (int row = HEIGHT; row >= 0; row--) {
            map[row][nsPathX] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevNsPathX = nsPathX;
                // -1 = Move path left, 1 = right, or 0 = stay the same
                nsPathX += rand() % 3 - 1;
                // Bounds checking
                if (nsPathX < 1) {
                    nsPathX = 1;
                }
                if (nsPathX >= WIDTH - 1) {
                    nsPathX = WIDTH - 2;
                }
                // Makes paths walkable horizontally when there is a bend
                startX = (prevNsPathX < nsPathX) ? prevNsPathX : nsPathX;
                endX = (prevNsPathX > nsPathX) ? prevNsPathX : nsPathX;
                for (int col = startX; col <= endX; col++) {
                    map[row][col] = path;
                }
            }
        }
        
    }

    if (connectNorth == -1 && connectSouth != -1 && connectWest == -1 && connectEast == -1){
        // Create bendy E-W path
        for (int x = WIDTH-1; x >= 0; x--) {
            map[ewPathY][x] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevEwPathY = ewPathY;
                // -1 = Move path up, 1 = down, or 0 = stay the same
                ewPathY += rand() % 3 - 1;
                // Bounds checking
                if (ewPathY < 1) {
                    ewPathY = 1;
                }
                if (ewPathY >= HEIGHT - 1) {
                    ewPathY = HEIGHT - 2;
                }
                // Makes paths walkable vertically when there is a bend
                startY = (prevEwPathY < ewPathY) ? prevEwPathY : ewPathY;
                endY = (prevEwPathY > ewPathY) ? prevEwPathY : ewPathY;
                for (int y = startY; y <= endY; y++) {
                    map[y][x] = path;
                }
            }
        }
        
    }

    if (connectNorth != -1 && connectSouth == -1 && connectWest == -1 && connectEast == -1){
        // Create bendy E-W path
        for (int x = 0; x < WIDTH; x++) {
            map[ewPathY][x] = path;
            // Randomly decide if the path bends
            if (rand() % 4 == 0) {
                prevEwPathY = ewPathY;
                // -1 = Move path up, 1 = down, or 0 = stay the same
                ewPathY += rand() % 3 - 1;
                // Bounds checking
                if (ewPathY < 1) {
                    ewPathY = 1;
                }
                if (ewPathY >= HEIGHT - 1) {
                    ewPathY = HEIGHT - 2;
                }
                // Makes paths walkable vertically when there is a bend
                startY = (prevEwPathY < ewPathY) ? prevEwPathY : ewPathY;
                endY = (prevEwPathY > ewPathY) ? prevEwPathY : ewPathY;
                for (int y = startY; y <= endY; y++) {
                    map[y][x] = path;
                }
            }
        }
        
    }

    // Overrides path with a boulder when it is a map at the border of the entire world
    if (currentY == 0) {
        for (i = 0; i < WIDTH; i++) {
            map[0][i] = BOULDER;
        }
    }
    if (currentX == 0) {
        for (i = 0; i < HEIGHT; i++) {
            map[i][0] = BOULDER;
        }
    }
    if (currentY == MAX_MAPS - 1) {
        for (i = 0; i < WIDTH; i++) {
            map[HEIGHT - 1][i] = BOULDER;
        }
    }
    if (currentX == MAX_MAPS - 1) {
        for (i = 0; i < HEIGHT; i++) {
            map[i][WIDTH - 1] = BOULDER;
        }
    }
}

void placeBuilding(char map[HEIGHT][WIDTH], char buildingType) {
    int row, col;
    int placed = 0;
    int d = 0;

    // Calculate the distance from the center map (200, 200)
    int dx = abs(currentX - MAX_MAPS/2);
    int dy = abs(currentY - MAX_MAPS/2);

    if(dx > dy){
        d = dx;
    }
    else if (dy > dx){
        d = dy;
    }
    else if (dy == dx && dy != 200 && dx != 200){
        d = dy;
    }

    // Probability for generating Pokecenter and Pokemart
    double probability = (d == 0) ? 1.0 : (-45.0 * d / 200.0 + 50.0) / 100.0;
    
    // Randomly decide if the building should be placed based on the probability
    if (rand() % 100 < probability * 100 || d == 0) {
        while (!placed) {
            // Randomly choose a position on the map
            col = rand() % WIDTH;
            row = rand() % HEIGHT;

            // Check if the position is a path and has an adjacent clearing
            if (map[row][col] == PATH) {
                if (row > 0 && map[row-1][col] == CLEARING) {
                    map[row-1][col] = buildingType;
                    placed = 1;
                } else if (row < HEIGHT - 1 && map[row+1][col] == CLEARING) {
                    map[row+1][col] = buildingType;
                    placed = 1;
                } else if (col > 0 && map[row][col-1] == CLEARING) {
                    map[row][col-1] = buildingType;
                    placed = 1;
                } else if (col < WIDTH - 1 && map[row][col+1] == CLEARING) {
                    map[row][col+1] = buildingType;
                    placed = 1;
                }
            }
        }
    }
}

void addTerrain(char map[HEIGHT][WIDTH], char terrainType, int count) {
    int centerRow, centerCol, radius;
    for (int i = 0; i < count; i++) {
        // Randomly choose a center position for the terrain
        centerCol = rand() % WIDTH;
        centerRow = rand() % HEIGHT;
        // Randomly choose a radius with my preffered range of 3-12
        radius = rand() % 10 + 3;

        //Iterate through grid to get position
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                // Distance from the center
                double distance = sqrt(pow(x - centerCol, 2) + pow(y - centerRow, 2));

                // Check if within radius and not overwriting paths or borders then place terrain accordingly
                if (distance <= radius && map[y][x] != PATH && map[y][x] != BOULDER) {
                    map[y][x] = terrainType;
                }
            }
        }
    }
}

void addOtherFeatures(char map[HEIGHT][WIDTH]) {
    int col, row;
    // Gets total number of trees and boulders between 10 and 20 to be generated.
    int totalTrees = rand() % 11 + 10;  
    int totalBoulders = rand() % 11 + 10;

    for (int i = 0; i < totalTrees; i++) {
        do {
            // Randomly choose a position on the map
            col = rand() % WIDTH;
            row = rand() % HEIGHT;
        } while (map[row][col] != CLEARING);  // Ensure it's placed on a clearing

        // Place the tree at the chosen position
        map[row][col] = TREE;
    }
    for (int i = 0; i < totalBoulders; i++) {
        do {
            // Randomly choose a position on the map
            col = rand() % WIDTH;
            row = rand() % HEIGHT;
        } while (map[row][col] != CLEARING);  // Ensure it's placed on a clearing

        // Place the boulder at the chosen position
        map[row][col] = BOULDER;
    }
}

void saveMap(Map* maps[MAX_MAPS][MAX_MAPS], char map[HEIGHT][WIDTH], int x, int y) {
    //Checks if the map exists already or not
    if (!maps[x][y]) {
        // Allocate memory for a new map
        maps[x][y] = malloc(sizeof(Map)); 
    }
    // Copy the map so it can be reloaded in the future
    memcpy(maps[x][y]->cells, map, sizeof(map[0][0]) * HEIGHT * WIDTH); 
}

void loadMap(Map* maps[MAX_MAPS][MAX_MAPS], char map[HEIGHT][WIDTH], int x, int y) {
    int connectNorth = -1, connectEast = -1, connectSouth = -1, connectWest = -1;

    // Check for connecting paths from adjacent maps
    // North
    if (y > 0 && maps[x][y - 1]) { 
        for (int col = 0; col < WIDTH; col++) {
            if (maps[x][y - 1]->cells[HEIGHT - 1][col] == PATH) {
                connectNorth = col;
                break;
            }
        }
    }
    // South
    if (y < MAX_MAPS - 1 && maps[x][y + 1]) { 
        for (int col = 0; col < WIDTH; col++) {
            if (maps[x][y + 1]->cells[0][col] == PATH) {
                connectSouth = col;
                break;
            }
        }
    }
    // West
    if (x > 0 && maps[x - 1][y]) { 
        for (int row = 0; row < HEIGHT; row++) {
            if (maps[x - 1][y]->cells[row][WIDTH - 1] == PATH) {
                connectWest = row;
                break;
            }
        }
    }
    // East
    if (x < MAX_MAPS - 1 && maps[x + 1][y]) { 
        for (int row = 0; row < HEIGHT; row++) {
            if (maps[x + 1][y]->cells[row][0] == PATH) {
                connectEast = row;
                break;
            }
        }
    }

    //If the map exists already
    if (maps[x][y]) {
        // Copy the map back
        memcpy(map, maps[x][y]->cells, sizeof(map[0][0]) * HEIGHT * WIDTH); 
    } else {
        // If the map at this location was never visited, generate a new map
        initializeMap(map);
        
        // Pass the connecting points to createPath
        createPath(map, PATH, connectNorth, connectEast, connectSouth, connectWest);
        
        addTerrain(map, TALL_GRASS, 2);
        addTerrain(map, WATER, 1);
        addOtherFeatures(map);
        placeBuilding(map, POKEMON_CENTER);
        placeBuilding(map, POKEMART);
        saveMap(maps, map, x, y); // Save the newly generated map
    }
}

void placePlayer(char map[HEIGHT][WIDTH]) {
    int placed = 0;
    while (!placed) {
        int row = rand() % (HEIGHT - 2) + 1; // From 1 to HEIGHT-2
        int col = rand() % (WIDTH - 2) + 1;  // From 1 to WIDTH-2
        //PC can only be placed on path
        if (map[row][col] == PATH) { 
            map[row][col] = PLAYER;
            placed = 1;
        }
    }
}

void placeAdversary(char map[HEIGHT][WIDTH], char adversaryType) {
    int placed = 0;
    while (!placed) {
        int row = rand() % HEIGHT;
        int col = rand() % WIDTH;

        // Hiker can move almost anywhere except water
        if (adversaryType == HIKER && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = HIKER;
            placed = 1;
        }
        // Rival cannot move through rocks, trees, or water
        else if (adversaryType == RIVAL && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = RIVAL;
            placed = 1;
        }
        else if (adversaryType == PACER && (map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER)) {
            map[row][col] = PACER;
            pacerX = col; // Set initial position of the Pacer
            pacerY = row;
            pacerDirection = (rand() % 2) * 2 - 1; // Randomly sets initial direction to left or right
            placed = 1;
        }
        else if (adversaryType == WANDERER && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = WANDERER;
            placed = 1;
        }
        else if (adversaryType == SENTRY && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER && map[row][col] != PATH) {
            map[row][col] = SENTRY;
            placed = 1;
        }
        else if (adversaryType == EXPLORER && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER && map[row][col] != PATH) {
            map[row][col] = EXPLORER;
            placed = 1;
        }
    }
}

void moveHiker(char map[HEIGHT][WIDTH], DistanceMap *distMap) {
    int x, y, minDist = INF, nextX = 0, nextY = 0;
    // Find the Hiker's current position
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == HIKER) {
                // Check all adjacent cells to find the one with the lowest distance value
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int newX = x + dx, newY = y + dy;
                        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT &&
                            (map[newY][newX] == CLEARING || map[newY][newX] == PATH || map[newY][newX] == TALL_GRASS)) {
                            if (distMap->nodes[newY][newX].dist < minDist) {
                                minDist = distMap->nodes[newY][newX].dist;
                                nextX = newX;
                                nextY = newY;
                            }
                        }
                    }
                }
                // Move the Hiker to the cell with the lowest distance
                if (minDist != INF) {
                    // Restore the old position to its previous state
                    map[y][x] = prevHikerCell;  // Use the saved 'prevHikerCell' state
                    // Save the current state before moving
                    prevHikerCell = map[nextY][nextX]; 
                    // Place the Hiker in the new position
                    map[nextY][nextX] = HIKER;
                }
                return; // Exit the function once the Hiker is moved
            }
        }
    }
}

void moveRival(char map[HEIGHT][WIDTH], DistanceMap *distMap) {
    int x, y, minDist = INF, nextX = 0, nextY = 0;
    // Find the Rival's current position
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == RIVAL) {
                // Check all adjacent cells to find the one with the lowest distance value
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int newX = x + dx, newY = y + dy;
                        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT &&
                            (map[newY][newX] == CLEARING || map[newY][newX] == PATH || map[newY][newX] == TALL_GRASS)) {
                            if (distMap->nodes[newY][newX].dist < minDist) {
                                minDist = distMap->nodes[newY][newX].dist;
                                nextX = newX;
                                nextY = newY;
                            }
                        }
                    }
                }
                // Move the Rival to the cell with the lowest distance
                if (minDist != INF) {
                    // Restore the old position to its previous state
                    map[y][x] = prevRivalCell;  // Use the saved 'prevRivalCell' state
                    // Save the current state before moving
                    prevRivalCell = map[nextY][nextX]; 
                    // Place the Rival in the new position
                    map[nextY][nextX] = RIVAL;
                }
                return; // Exit the function once the Rival is moved
            }
        }
    }
}
void moveExplorer(char map[HEIGHT][WIDTH]) {
    int explorerX, explorerY, found = 0;

    // Find the Explorer's current position
    for (int y = 0; y < HEIGHT && !found; y++) {
        for (int x = 0; x < WIDTH && !found; x++) {
            if (map[y][x] == EXPLORER) {
                explorerX = x;
                explorerY = y;
                found = 1; // Mark as found to break out of loops
            }
        }
    }

    if (found) {
        int attempts = 0;
        while (attempts < 8) { // Limit the number of attempts to avoid infinite loops
            // Define possible movements (8 directions including diagonals)
            int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1};
            int dy[8] = {0, 0, 1, -1, 1, 1, -1, -1};

            int newX = explorerX + dx[explorerDirection];
            int newY = explorerY + dy[explorerDirection];

            // Check if the new position is valid
            if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
                char newPosType = map[newY][newX];
                // Check if the new position is not an obstacle and different than current
                if (newPosType != WATER && newPosType != BOULDER && newPosType != TREE && newPosType != EXPLORER) {
                    // Restore the old position to its previous state
                    map[explorerY][explorerX] = prevExplorerCell; 

                    // Save the current state before moving
                    prevExplorerCell = newPosType; 

                    // Move the Explorer to the new position
                    map[newY][newX] = EXPLORER; 
                    break; // Exit the loop after moving
                } else {
                    explorerDirection = rand() % 8; // Choose a new random direction if blocked
                }
            }
            attempts++; // Increment attempts after each try
        }
    }
}

void moveWanderer(char map[HEIGHT][WIDTH]) {
    static int wandererX, wandererY, found = 0, direction = -1;
    static char spawnTerrain;

    // Initialize Wanderer's position and direction if not found previously
    if (!found) {
        for (int y = 0; y < HEIGHT && !found; y++) {
            for (int x = 0; x < WIDTH && !found; x++) {
                if (map[y][x] == WANDERER) {
                    wandererX = x;
                    wandererY = y;
                    found = 1; // Mark as found to break out of loops

                    // Count adjacent terrain types
                    int tallGrassCount = 0, clearingCount = 0;
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            if (dx == 0 && dy == 0) continue; // Skip the Wanderer's own cell
                            int checkX = wandererX + dx, checkY = wandererY + dy;
                            // Ensure we're not checking outside the map boundaries
                            if (checkX >= 0 && checkX < WIDTH && checkY >= 0 && checkY < HEIGHT) {
                                if (map[checkY][checkX] == TALL_GRASS) tallGrassCount++;
                                if (map[checkY][checkX] == CLEARING) clearingCount++;
                            }
                        }
                    }

                    // Set spawn terrain based on majority of adjacent cells
                    spawnTerrain = (tallGrassCount > clearingCount) ? TALL_GRASS : CLEARING;

                    direction = rand() % 8; // Set initial random direction for eight possible movements
                }
            }
        }
    }

    // Define possible movements including diagonals (8 directions)
    int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1};
    int dy[8] = {0, 0, 1, -1, 1, 1, -1, -1};

    // Attempt to move Wanderer if found
    if (found) {
        int newX = wandererX + dx[direction];
        int newY = wandererY + dy[direction];

        // Check if the new position is within bounds and matches the Wanderer's spawn terrain
        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT && map[newY][newX] == spawnTerrain) {
            // Move the Wanderer to the new position
            map[wandererY][wandererX] = spawnTerrain; // Restore the old position to its terrain type
            map[newY][newX] = WANDERER; // Place the Wanderer in the new position
            wandererX = newX;
            wandererY = newY;
        } else {
            // Change direction if the Wanderer cannot move further in the current direction
            direction = rand() % 8; // Choose a new random direction from eight possibilities
        }
    }
}

void movePacer(char map[HEIGHT][WIDTH], int *pacerX, int *pacerY, int *direction) {
    int newX = *pacerX + *direction;
    if (newX >= 0 && newX < WIDTH && (map[*pacerY][newX] == PATH || map[*pacerY][newX] == TALL_GRASS || map[*pacerY][newX] == CLEARING)) {
        // Restore the cell the pacer was previously on
        map[*pacerY][*pacerX] = prevPacerCell; 
        
        // Save the content of the new cell before moving the pacer there
        prevPacerCell = map[*pacerY][newX];
        
        // Update pacer's position
        *pacerX = newX;
        // Place pacer at new position
        map[*pacerY][*pacerX] = PACER;
    } else {
        // Change direction
        *direction *= -1;
    }
}

void updatePacerPosition(char map[HEIGHT][WIDTH]) {
    if (pacerX != -1 && pacerY != -1) { // Check if Pacer has been initialized
        movePacer(map, &pacerX, &pacerY, &pacerDirection);
    }
}



// Gets the weight of moving into a cell for a hiker or rival
int getWeight(char cell, int isRival) {
    switch(cell) {
        case PATH:
        case CLEARING:
            return 10;
        case TALL_GRASS:
        // 20 for rival, 15 for hiker
            return isRival ? 20 : 15; 
        case POKEMON_CENTER:
        case POKEMART:
            return 50;
        case WATER:
        case BOULDER:
        case TREE:
            return INF; 
        default:
            return INF;
    }
}

void dijkstra(char map[HEIGHT][WIDTH], DistanceMap *distMap, int startX, int startY, int isRival) {
    // Initialize distance map
    for (int row = 0; row < HEIGHT; ++row) {
        for (int col = 0; col < WIDTH; ++col) {
            distMap->nodes[row][col].col = col;
            distMap->nodes[row][col].row = row;
            distMap->nodes[row][col].dist = INF;
            distMap->nodes[row][col].visited = 0;
        }
    }

    // Set start position distance to 0
    distMap->nodes[startY][startX].dist = 0;

    while (1) {
        int minDist = INF;
        int minX = -1, minY = -1;
        for (int y = 0; y < HEIGHT; ++y) {
            for (int x = 0; x < WIDTH; ++x) {
                if (!distMap->nodes[y][x].visited && distMap->nodes[y][x].dist < minDist) {
                    minDist = distMap->nodes[y][x].dist;
                    minX = x;
                    minY = y;
                }
            }
        }

        if (minDist == INF) break;

        distMap->nodes[minY][minX].visited = 1;

        // All 8 movements incuding diagonal
        int neighbors[8][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        for (int i = 0; i < 8; ++i) {
            int neighborX = minX + neighbors[i][0];
            int neighborY = minY + neighbors[i][1];
            if (neighborX >= 0 && neighborX < WIDTH && neighborY >= 0 && neighborY < HEIGHT) {
                int weight = getWeight(map[neighborY][neighborX], isRival);
                if (distMap->nodes[minY][minX].dist + weight < distMap->nodes[neighborY][neighborX].dist) {
                    distMap->nodes[neighborY][neighborX].dist = distMap->nodes[minY][minX].dist + weight;
                }
            }
        }
    }
}

// Modify the printMap function to display the map with the player character
void printMap(char map[HEIGHT][WIDTH]) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", map[y][x]);
        }
        printf("\n");
    }
    //printf("(%d, %d)\n", currentX - MAX_MAPS / 2, currentY - MAX_MAPS / 2);
}


int main(int argc, char *argv[]) {
    int numTrainers = 6;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0 && i + 1 < argc) {
            numTrainers = atoi(argv[i + 1]);
            i++; // Skip the next argument since we already used it
        }
    }

    signal(SIGINT, handle_signal);
    unsigned int seed = time(NULL);
    srand(seed);
    srand(time(NULL));
    char map[HEIGHT][WIDTH];

    // Initialize all map pointers to NULL
    for (int i = 0; i < MAX_MAPS; i++) {
        for (int j = 0; j < MAX_MAPS; j++) {
            maps[i][j] = NULL;
        }
    }

    // Initialize and generate the initial map
    int playerX, playerY;
    initializeMap(map);
    createPath(map, PATH, -1, -1, -1, -1); // No connecting paths for the first map
    addTerrain(map, TALL_GRASS, 2);
    addTerrain(map, WATER, 1);
    addOtherFeatures(map);
    placeBuilding(map, POKEMON_CENTER);
    placeBuilding(map, POKEMART);
    placePlayer(map);
    placeAdversary(map, HIKER);
    placeAdversary(map, RIVAL);

    if (numTrainers > 0) {
        placeAdversary(map, RIVAL); 
    }
    if (numTrainers > 1) {
        placeAdversary(map, HIKER); 
    }
    if (numTrainers > 2) {
        placeAdversary(map, PACER); // Place a Pacer if numTrainers > 2
    }
    if (numTrainers > 3) {
        placeAdversary(map, WANDERER); // Place a Wanderer if numTrainers > 3
    }
    if (numTrainers > 4) {
        placeAdversary(map, EXPLORER); // Place a Explorer if numTrainers > 4
    }
    if (numTrainers > 5) {
        placeAdversary(map, SENTRY); // Place an Sentry if numTrainers > 5
    }

    int extraSentries = numTrainers - 6; // Calculate how many extra Sentries to add
        for (int i = 0; i < extraSentries; i++) {
        placeAdversary(map, SENTRY); // Place an additional Sentry for each number above 6
}

    // Get the coordinates of the player character
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (map[y][x] == PLAYER) {
                playerX = x;
                playerY = y;
                break;
            }
        }
    }

    printMap(map); // Print the initial map with the player character placed

    DistanceMap distMapHiker, distMapRival;
    dijkstra(map, &distMapHiker, playerX, playerY, 0); // For Hiker
    dijkstra(map, &distMapRival, playerX, playerY, 1); // For Rival

    // Game loop
    while (1) { // Remove manual 'n' command for next turn

        // Update NPC positions
        moveHiker(map, &distMapHiker);
        moveRival(map, &distMapRival);
        updatePacerPosition(map);
        moveExplorer(map);
        moveWanderer(map);

        // Print the updated map
        printMap(map);

        // Pause between each update to make it human-viewable
        usleep(250000); // Sleep for 250 milliseconds (Adjust this value as needed)


        // Check if game over or player wants to quit, implement your condition here
        // For example, if(game_over) { break; }
    }

    return 0;
}