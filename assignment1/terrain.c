#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
int WIDTH = 80;
int HEIGHT = 21;
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


void createPath(char map[HEIGHT][WIDTH], char path) {
    // Initial position for North to South path
    int nsPathX = rand() % (WIDTH - 3) + 2;
    int prevNsPathX = nsPathX;
    int startX;
    int endX;
    int startY;
    int endY;

    // Initial position for East to West path
    int ewPathY = rand() % (HEIGHT - 3) + 2;
    int prevEwPathY = ewPathY;

    // Create bendy N-S path
    for (int row = 0; row < HEIGHT; row++) {
        map[row][nsPathX] = path;
        // Randomly decide if the path bends
        if (rand() % 4 == 0) { 
            prevNsPathX = nsPathX;
            // -1 = Move path left, 1 = right, or 0 = stay the same
            nsPathX += rand() % 3 - 1; 
            // Make sure path doesn't go out of bounds if nsPathX < 1 it resets it to 1.
            if (nsPathX < 1) {
                nsPathX = 1;  
            }
            // Make sure path doesn't go out of bounds if greater or equal to WIDTH - 1 then resets to WIDTH - 2
            if (nsPathX >= WIDTH - 1) {
                nsPathX = WIDTH - 2;  
            } 
            
            // Makes paths walkable horizontally when there is a bend
            if (prevNsPathX < nsPathX) {
                startX = prevNsPathX;
            } else {
                startX = nsPathX;
            }       
            if (prevNsPathX > nsPathX) {
                endX = prevNsPathX;
            } else {
                endX = nsPathX;
            }
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
            // If ewPathY < 1 it resets it to 1 so it doesn't go out of bounds
            if (ewPathY < 1) {
                ewPathY = 1;  
            }
            // If ewPathY >= HEIGHT - 1 it resets it to HEIGHT - 2 so it doesn't go out of bounds
            if (ewPathY >= HEIGHT - 1) {
                ewPathY = HEIGHT - 2;  
            }
            
            // Makes paths walkable vertically when there is a bend
            if (prevEwPathY < ewPathY) {
                startY = prevEwPathY;
            } else {
                startY = ewPathY;
            }
            if (prevEwPathY > ewPathY) {
                endY = prevEwPathY;
            } else {
                endY = ewPathY;
            }
            for (int y = startY; y <= endY; y++) {
                map[y][x] = path;
    }

        }
    }
}

void placeBuilding(char map[HEIGHT][WIDTH], char buildingType) {
    int row, col; 
    int placed = 0;
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

void addTerrain(char map[HEIGHT][WIDTH], char terrainType, int count) {
    int centerRow, centerCol, radius;
    for (int i = 0; i < count; i++) {
        // Randomly choose a center position for the terrain
        centerCol = rand() % WIDTH;
        centerRow = rand() % HEIGHT;
        // Randomly choose a radius with my preffered range of 3-9
        radius = rand() % 7 + 3;

        //Iterate through grid to get position
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                // Distance from the center
                double distance = sqrt(pow(x - centerCol, 2) + pow(y - centerRow, 2));

                // Check if within radius and not overwriting paths or borders then place terrain accordingly
                if (distance <= radius && map[y][x] != PATH && map[y][x] != BOULDER && map[y][x] != TALL_GRASS) {
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

void placePlayer(char map[HEIGHT][WIDTH]) {
    int placed = 0;
    while (!placed) {
        // Make sure player can't be placed on a gate
        int row = rand() % (HEIGHT - 2) + 1; // From 1 to HEIGHT-2
        int col = rand() % (WIDTH - 2) + 1;  // From 1 to WIDTH-2
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
        if (adversaryType == HIKER && map[row][col] != WATER && map[row][col] != BOULDER) {
            map[row][col] = HIKER;
            placed = 1;
        }
        // Rival cannot move through rocks, trees, or water
        else if (adversaryType == RIVAL && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER) {
            map[row][col] = RIVAL;
            placed = 1;
        }
        else if (adversaryType == PACER && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER) {
            map[row][col] = PACER;
            pacerX = col; // Set initial position of the Pacer
            pacerY = row;
            pacerDirection = (rand() % 2) * 2 - 1; // Randomly sets initial direction to left or right
            placed = 1;
        }
        else if (adversaryType == WANDERER && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER) {
            map[row][col] = WANDERER;
            placed = 1;
        }
        else if (adversaryType == SENTRY && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER) {
            map[row][col] = SENTRY;
            placed = 1;
        }
        else if (adversaryType == EXPLORER && map[row][col] != WATER && map[row][col] != TREE && map[row][col] != BOULDER) {
            map[row][col] = EXPLORER;
            placed = 1;
        }
    }
}

void movePacer(char map[HEIGHT][WIDTH], int *pacerX, int *pacerY, int *direction) {
    int newX = *pacerX + *direction; // Assume direction is either 1 (right) or -1 (left) for simplicity

    // Check if the new position is within bounds and not impassable terrain
    if (newX >= 1 && newX < WIDTH - 1 && map[*pacerY][newX] != WATER && map[*pacerY][newX] != TREE && map[*pacerY][newX] != BOULDER) {
        // Move pacer but first restore the previous cell based on a simple assumption
        if (map[*pacerY][*pacerX] == PACER) {
            map[*pacerY][*pacerX] = map[*pacerY][*pacerX + *direction]; // Assume the Pacer was on a clearing before
        }
        *pacerX = newX; // Update position
        map[*pacerY][*pacerX] = PACER; // Place pacer at new position
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


void printMap(char map[HEIGHT][WIDTH]) {
    // Print the map
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", map[y][x]); 
        }
        printf("\n"); 
    }
}


int main(){
srand(time(NULL));

// Initialize the map
char map[HEIGHT][WIDTH];
initializeMap(map);

// Create paths
createPath(map, PATH);


// Add terrains
addTerrain(map, TALL_GRASS, 2); 
addTerrain(map, WATER, 1); 

// Add boulders and trees
addOtherFeatures(map);

// Place pokemon center/mart
placeBuilding(map, POKEMON_CENTER);
placeBuilding(map, POKEMART);

placePlayer(map); // Place the player character on the map
placeAdversary(map, HIKER); // Place a Hiker on the map
placeAdversary(map, RIVAL); // Place a Rival on the map
placeAdversary(map, PACER);
placeAdversary(map, WANDERER);
placeAdversary(map, SENTRY);
placeAdversary(map, EXPLORER);



// Print the map
printMap(map);

// Game loop
    char command;
    do {
        printf("Enter 'n' to move to the next turn, or 'q' to quit: ");
        scanf(" %c", &command); // Read a single character command from the user

        if(command == 'n') { // 'n' for next turn
            // Update NPC positions here, for now, just the Pacer
            updatePacerPosition(map);

            // Print the updated map
            printMap(map);
        }
    } while(command != 'q'); // 'q' for quit

    return 0;
}