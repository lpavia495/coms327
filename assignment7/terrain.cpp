#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <ctime>
#include <cstring>
#include <cmath>
#include <unistd.h>
#include <ncurses.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <climits> 
#define TALL_GRASS_COLOR 1
#define CLEARING_COLOR 2
#define PATH_COLOR 3
#define WATER_COLOR 4
#define BOULDER_COLOR 5
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
#define INF 9999
int pacerX = -1;
int pacerY = -1;
int WandererX = -1;
int WandererY = -1;
int pacerDirection = 1; 
int explorerDirection = 1;
int wandererDirection = 1;
int rivalDefeated = 0;
int hikerDefeated = 0;
int pacerDefeated = 0;
int explorerDefeated = 0;
int wandererDefeated = 0;
int sentryDefeated = 0;
bool playerExistsOnMap = true;
int wandererfound = 0;
// Initialize with default terrain type
char prevPacerCell = CLEARING;
char prevExplorerCell = CLEARING; 
char prevHikerCell = CLEARING; 
char prevRivalCell = CLEARING; 
std::string finalpath;

//Hold information about each Pokemon
struct Pokemon {
    int id;
    std::string identifier;
    int species_id;
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
};

// Function to parse the pokemon.csv file and return a vector of Pokemon structures
std::vector<Pokemon> parsePokemonCsv(const std::string& filepath) {
    std::vector<Pokemon> pokemons;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return pokemons; 
    }

    std::string line;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;
        Pokemon pokemon;

        std::getline(ss, field, ',');
        pokemon.id = std::stoi(field);

        std::getline(ss, pokemon.identifier, ',');

        std::getline(ss, field, ',');
        pokemon.species_id = std::stoi(field);

        std::getline(ss, field, ',');
        pokemon.height = std::stoi(field);

        std::getline(ss, field, ',');
        pokemon.weight = std::stoi(field);

        std::getline(ss, field, ',');
        pokemon.base_experience = std::stoi(field);

        std::getline(ss, field, ',');
        pokemon.order = std::stoi(field);

        std::getline(ss, field, ',');
        pokemon.is_default = std::stoi(field);

        pokemons.push_back(pokemon);
    }

    return pokemons;
}

struct Move {
    int id;
    std::string identifier;
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance; // INT_MAX will represent an empty field here
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

    // Constructor that initializes a Move object from a CSV row.
    Move(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); id = safeStoi(field);
        std::getline(ss, identifier, ',');
        std::getline(ss, field, ','); generation_id = safeStoi(field);
        std::getline(ss, field, ','); type_id = safeStoi(field);
        std::getline(ss, field, ','); power = safeStoi(field);
        std::getline(ss, field, ','); pp = safeStoi(field);
        std::getline(ss, field, ','); accuracy = safeStoi(field);
        std::getline(ss, field, ','); priority = safeStoi(field);
        std::getline(ss, field, ','); target_id = safeStoi(field);
        std::getline(ss, field, ','); damage_class_id = safeStoi(field);
        std::getline(ss, field, ','); effect_id = safeStoi(field);

 
        std::getline(ss, field, ',');
        effect_chance = field.empty() ? INT_MAX : safeStoi(field);

        std::getline(ss, field, ','); contest_type_id = safeStoi(field);
        std::getline(ss, field, ','); contest_effect_id = safeStoi(field);
        std::getline(ss, field, ','); super_contest_effect_id = safeStoi(field);
    }

    static int safeStoi(const std::string& str) {
        // Convert string to integer and returns INT_MAX for empty strings
        try {
            return str.empty() ? INT_MAX : std::stoi(str);
        } catch (const std::invalid_argument& ia) {
            return INT_MAX;
        } catch (const std::out_of_range& oor) {
            return INT_MAX;
        }
    }

    void print() const {
        // Print the Move data except for INT_MAX values
        std::cout << "ID: " << id << ", Identifier: " << identifier << ", Generation ID: " << generation_id
                  << ", Type ID: " << type_id << ", Power: " << power << ", PP: " << pp
                  << ", Accuracy: " << accuracy << ", Priority: " << priority
                  << ", Target ID: " << target_id << ", Damage Class ID: " << damage_class_id
                  << ", Effect ID: " << effect_id;
        
        if (effect_chance != INT_MAX) {
            std::cout << ", Effect Chance: " << effect_chance;
        }

        std::cout << ", Contest Type ID: " << contest_type_id << ", Contest Effect ID: " << contest_effect_id
                  << ", Super Contest Effect ID: " << super_contest_effect_id << std::endl;
    }
};

struct PokemonMove {
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    // Constructor to initialize from a CSV row
    PokemonMove(const std::string& row) {
    std::stringstream ss(row);
    std::string field;

    std::getline(ss, field, ','); pokemon_id = safeStoi(field);
    std::getline(ss, field, ','); version_group_id = safeStoi(field);
    std::getline(ss, field, ','); move_id = safeStoi(field);
    std::getline(ss, field, ','); pokemon_move_method_id = safeStoi(field);
    std::getline(ss, field, ','); level = safeStoi(field);
    std::getline(ss, field, ','); order = safeStoi(field); // Corrected this line
    }

    static int safeStoi(const std::string& str) {
        try {
            return str.empty() ? INT_MAX : std::stoi(str);
        } catch (...) {
            return INT_MAX;
        }
    }
};

std::vector<PokemonMove> parsePokemonMovesCsv(const std::string& filepath) {
    std::vector<PokemonMove> pokemonMoves;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return pokemonMoves;
    }

    std::string line;
    std::getline(file, line); // Skip the header

    while (std::getline(file, line)) {
        pokemonMoves.emplace_back(line);
    }

    return pokemonMoves;
}

struct PokemonSpecies {
    int id;
    std::string identifier;
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    
    PokemonSpecies(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); id = safeStoi(field);
        std::getline(ss, identifier, ',');
        std::getline(ss, field, ','); generation_id = safeStoi(field);
        std::getline(ss, field, ','); evolves_from_species_id = safeStoi(field, true);
        std::getline(ss, field, ','); evolution_chain_id = safeStoi(field);
        std::getline(ss, field, ','); color_id = safeStoi(field);
        std::getline(ss, field, ','); shape_id = safeStoi(field);
        std::getline(ss, field, ','); habitat_id = safeStoi(field, true);
        std::getline(ss, field, ','); gender_rate = safeStoi(field);
        std::getline(ss, field, ','); capture_rate = safeStoi(field);
        std::getline(ss, field, ','); base_happiness = safeStoi(field);
        std::getline(ss, field, ','); is_baby = safeStoi(field);
        std::getline(ss, field, ','); hatch_counter = safeStoi(field);
        std::getline(ss, field, ','); has_gender_differences = safeStoi(field);
        std::getline(ss, field, ','); growth_rate_id = safeStoi(field);
        std::getline(ss, field, ','); forms_switchable = safeStoi(field);
        std::getline(ss, field, ','); is_legendary = safeStoi(field);
        std::getline(ss, field, ','); is_mythical = safeStoi(field);
        std::getline(ss, field, ','); order = safeStoi(field);
        std::getline(ss, field, ','); conquest_order = safeStoi(field, true);
    }

    static int safeStoi(const std::string& str, bool isOptional = false) {
        if (str.empty() && isOptional) return INT_MAX;
        try {
            return std::stoi(str);
        } catch (...) {
            return INT_MAX;
        }
    }
};

std::vector<PokemonSpecies> parsePokemonSpeciesCsv(const std::string& filepath) {
    std::vector<PokemonSpecies> pokemonSpecies;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return pokemonSpecies;
    }

    std::string line;
    std::getline(file, line); // Skip the header

    while (std::getline(file, line)) {
        pokemonSpecies.emplace_back(line);
    }

    return pokemonSpecies;
}

struct Experience {
    int growth_rate_id;
    int level;
    int experience;

    // Constructor that initializes an Experience object from a CSV row
    Experience(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); growth_rate_id = stoi(field);
        std::getline(ss, field, ','); level = stoi(field);
        std::getline(ss, field, ','); experience = stoi(field);
    }
};

std::vector<Experience> parseExperienceCsv(const std::string& filepath) {
    std::vector<Experience> experiences;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return experiences;
    }

    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        experiences.emplace_back(line);
    }

    return experiences;
}

struct TypeName {
    int type_id;
    int local_language_id;
    std::string name;

    // Constructor that initializes a TypeName object from a CSV row.
    TypeName(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); type_id = std::stoi(field);
        std::getline(ss, field, ','); local_language_id = std::stoi(field);
        std::getline(ss, name, ',');
    }
};

std::vector<TypeName> parseTypeNamesCsv(const std::string& filepath) {
    std::vector<TypeName> typeNames;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return typeNames; 
    }

    std::string line;
    std::getline(file, line); // Skip the header line.

    while (std::getline(file, line)) {
        TypeName typeName(line);
        // Filter for English names.
        if (typeName.local_language_id == 9) { 
            typeNames.push_back(typeName);
        }
    }

    return typeNames;
}

struct PokemonStat {
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

    // Constructor that initializes a PokemonStat object from a CSV row
    PokemonStat(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); pokemon_id = std::stoi(field);
        std::getline(ss, field, ','); stat_id = std::stoi(field);
        std::getline(ss, field, ','); base_stat = std::stoi(field);
        std::getline(ss, field, ','); effort = std::stoi(field);
    }
};

std::vector<PokemonStat> parsePokemonStatsCsv(const std::string& filepath) {
    std::vector<PokemonStat> pokemonStats;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return pokemonStats;
    }

    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        pokemonStats.emplace_back(line);
    }

    return pokemonStats;
}

struct Stat {
    int id;
    int damage_class_id; 
    std::string identifier;
    int is_battle_only;
    int game_index; 

    // Constructor that initializes a Stat object from a CSV row.
    Stat(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); id = safeStoi(field);
        std::getline(ss, field, ','); damage_class_id = safeStoi(field);
        std::getline(ss, identifier, ',');
        std::getline(ss, field, ','); is_battle_only = safeStoi(field);
        std::getline(ss, field, ','); game_index = safeStoi(field);
    }

    static int safeStoi(const std::string& str) {
        // Convert string to int, returning INT_MAX for empty strings
        try {
            return str.empty() ? INT_MAX : std::stoi(str);
        } catch (...) {
            return INT_MAX;
        }
    }
};

std::vector<Stat> parseStatsCsv(const std::string& filepath) {
    std::vector<Stat> stats;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return stats; 
    }

    std::string line;
    std::getline(file, line); // Skip the header line

    while (std::getline(file, line)) {
        stats.emplace_back(line);
    }

    return stats;
}

struct PokemonType {
    int pokemon_id;
    int type_id;
    int slot;

    // Constructor that initializes a PokemonType object from a CSV row.
    PokemonType(const std::string& row) {
        std::stringstream ss(row);
        std::string field;

        std::getline(ss, field, ','); pokemon_id = std::stoi(field);
        std::getline(ss, field, ','); type_id = std::stoi(field);
        std::getline(ss, field, ','); slot = std::stoi(field);
    }
};

std::vector<PokemonType> parsePokemonTypesCsv(const std::string& filepath) {
    std::vector<PokemonType> pokemonTypes;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return pokemonTypes; 
    }

    std::string line;
    std::getline(file, line); // Skip the header line.

    while (std::getline(file, line)) {
        pokemonTypes.emplace_back(line);
    }

    return pokemonTypes;
}

std::ifstream openFile(const std::string& filename) {
    // First directory
    std::string firstDir = "/share/cs327/pokedex/pokedex/data/csv/";
    std::string firstPath = firstDir + filename;

    // Try opening the file in the first directory
    std::ifstream file(firstPath);
    finalpath = firstPath;
    if (!file.is_open()) {
        // If opening fails, use second path
        std::string homeDir = getenv("HOME");
        std::string secondDir = homeDir + "/.poke327/pokedex/pokedex/data/csv/";
        std::string secondPath = secondDir + filename;

        // Try opening the file in the second directory
        file.open(secondPath);
        finalpath = secondPath;
        if (!file.is_open()) {
            // Error message if both directories fail
            std::cerr << "Failed to open file: " << filename << " in both directories." << std::endl;
            exit(0);
        }
    }
    return file;
}

typedef struct {
    char cells[HEIGHT][WIDTH];
    int rivalDefeated;
    int hikerDefeated;
    int pacerDefeated;
    int explorerDefeated;
    int wandererDefeated;
    int sentryDefeated;
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
    endwin();
    exit(0);
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
        // Randomly choose a radius with my preffered range
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
        } while (map[row][col] != CLEARING); 

        // Place the tree at the chosen position
        map[row][col] = TREE;
    }
    for (int i = 0; i < totalBoulders; i++) {
        do {
            // Randomly choose a position on the map
            col = rand() % WIDTH;
            row = rand() % HEIGHT;
        } while (map[row][col] != CLEARING);  

        // Place the boulder at the chosen position
        map[row][col] = BOULDER;
    }
}

void placeAdversary(char map[HEIGHT][WIDTH], char adversaryType) {
    int placed = 0;
    while (!placed) {
        int row = rand() % HEIGHT;
        int col = rand() % WIDTH;

        
        if (adversaryType == HIKER && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = HIKER;
            placed = 1;
        }
        
        else if (adversaryType == RIVAL && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = RIVAL;
            placed = 1;
        }
        else if (adversaryType == PACER && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = PACER;
            pacerX = col; 
            pacerY = row;
            // Randomly sets initial direction to left or right
            pacerDirection = (rand() % 2) * 2 - 1; 
            placed = 1;
        }
        else if (adversaryType == WANDERER && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = WANDERER;
            placed = 1;
        }
        else if (adversaryType == SENTRY && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = SENTRY;
            placed = 1;
        }
        else if (adversaryType == EXPLORER && (map[row][col] == CLEARING || map[row][col] == TALL_GRASS)) {
            map[row][col] = EXPLORER;
            placed = 1;
        }
    }
}

void saveMap(Map* maps[MAX_MAPS][MAX_MAPS], char map[HEIGHT][WIDTH], int x, int y) {
    //Checks if the map exists already or not
    if (!maps[x][y]) {
        // Allocate memory for a new map
        maps[x][y] = new Map; 
    }
    // Copy the map so it can be reloaded in the future
    memcpy(maps[x][y]->cells, map, sizeof(map[0][0]) * HEIGHT * WIDTH); 
    maps[x][y]->rivalDefeated = rivalDefeated;
    maps[x][y]->hikerDefeated = hikerDefeated;
    maps[x][y]->pacerDefeated = pacerDefeated;
    maps[x][y]->explorerDefeated = explorerDefeated;
    maps[x][y]->wandererDefeated = wandererDefeated;
    maps[x][y]->sentryDefeated = sentryDefeated;
}

void loadMap(Map* maps[MAX_MAPS][MAX_MAPS], char map[HEIGHT][WIDTH], int x, int y) {
    int connectNorth = -1, connectEast = -1, connectSouth = -1, connectWest = -1;

    wandererfound = 0;

    // Check for connecting paths from adjacent maps
    if (y > 0 && maps[x][y - 1]) { 
        for (int col = 0; col < WIDTH; col++) {
            if (maps[x][y - 1]->cells[HEIGHT - 1][col] == PATH) {
                connectNorth = col;
                break;
            }
        }
    }
    if (y < MAX_MAPS - 1 && maps[x][y + 1]) { 
        for (int col = 0; col < WIDTH; col++) {
            if (maps[x][y + 1]->cells[0][col] == PATH) {
                connectSouth = col;
                break;
            }
        }
    }
    if (x > 0 && maps[x - 1][y]) { 
        for (int row = 0; row < HEIGHT; row++) {
            if (maps[x - 1][y]->cells[row][WIDTH - 1] == PATH) {
                connectWest = row;
                break;
            }
        }
    }
    if (x < MAX_MAPS - 1 && maps[x + 1][y]) { 
        for (int row = 0; row < HEIGHT; row++) {
            if (maps[x + 1][y]->cells[row][0] == PATH) {
                connectEast = row;
                break;
            }
        }
    }

    // If the map exists already, copy it back. Otherwise, create a new map.
    if (maps[x][y]) {
        memcpy(map, maps[x][y]->cells, sizeof(map[0][0]) * HEIGHT * WIDTH);

        rivalDefeated = maps[x][y]->rivalDefeated;
        hikerDefeated = maps[x][y]->hikerDefeated;
        pacerDefeated = maps[x][y]->pacerDefeated;
        explorerDefeated = maps[x][y]->explorerDefeated;
        wandererDefeated = maps[x][y]->wandererDefeated;
        sentryDefeated = maps[x][y]->sentryDefeated;
    } else {
        initializeMap(map);
        createPath(map, PATH, connectNorth, connectEast, connectSouth, connectWest);
        addTerrain(map, TALL_GRASS, 2);
        addTerrain(map, WATER, 1);
        addOtherFeatures(map);
        placeBuilding(map, POKEMON_CENTER);
        placeBuilding(map, POKEMART);
        // Save the newly generated map
        saveMap(maps, map, x, y); 

        // Place adversaries on the new map, as not defeated
        rivalDefeated = 0;
        hikerDefeated = 0;
        pacerDefeated = 0;
        explorerDefeated = 0;
        wandererDefeated = 0;
        sentryDefeated = 0;
        placeAdversary(map, RIVAL);
        placeAdversary(map, HIKER);
        placeAdversary(map, PACER);
        placeAdversary(map, WANDERER);
        placeAdversary(map, EXPLORER); 
        placeAdversary(map, SENTRY);   
    }

    bool foundPacer = false;
    for (int row = 0; row < HEIGHT && !foundPacer; row++) {
        for (int col = 0; col < WIDTH && !foundPacer; col++) {
            if (map[row][col] == PACER) {
                pacerX = col; 
                pacerY = row; 
                foundPacer = true;
                pacerDirection = (rand() % 2) * 2 - 1; 
                break;
            }
        }
    }
    if (!foundPacer) {
        pacerX = -1; 
        pacerY = -1;
    }
    if(!wandererfound){
        WandererX = -1;
        WandererY = -1;
    }
}


void placePlayer(char map[HEIGHT][WIDTH]) {
    int placed = 0;
    while (!placed) {
        int row = rand() % (HEIGHT - 2) + 1; 
        int col = rand() % (WIDTH - 2) + 1;  
        //PC can only be placed on path
        if (map[row][col] == PATH) { 
            map[row][col] = PLAYER;
            placed = 1;
        }
    }
}



void pokemonBattleInterface(char map[HEIGHT][WIDTH], int x, int y) {
    int inputKey;
    bool inBattle = true;

    while (inBattle) {
        clear(); // Clear the screen to show the battle interface
        mvprintw(0, 0, "A wild Pokemon appears! Press escape to flee or in this case defeat.");
        refresh(); // Refresh the screen to show the message

        inputKey = getch();
        switch (inputKey) {
            case 27: // Escape key
                inBattle = false;
                if (map[y][x] == HIKER) {
                    hikerDefeated = 1; 
                } else if (map[y][x] == RIVAL) {
                    rivalDefeated = 1; 
                }
                else if (map[y][x] == EXPLORER) {
                    explorerDefeated = 1; 
                }
                 else if (map[y][x] == PACER) {
                    pacerDefeated = 1; 
                }
                else if (map[y][x] == WANDERER) {
                    wandererDefeated = 1; 
                }
                else if (map[y][x] == SENTRY) {
                sentryDefeated = 1; 
                }
            break;
            
        }
    }

    clear();
}

void moveHiker(char map[HEIGHT][WIDTH], DistanceMap *distMap) {
    if (hikerDefeated) return;
    int x, y, minDist = INF, nextX = 0, nextY = 0;
    // Find the Hiker's current position
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == HIKER) {
                // Check all adjacent cells to find the one with the lowest distance value
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int newX = x + dx, newY = y + dy;
                        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
                            if (map[newY][newX] == PLAYER) {
                                // Trigger battle interface if Hiker moves onto player
                                pokemonBattleInterface(map, x, y);
                                return;
                            } else if (distMap->nodes[newY][newX].dist < minDist && 
                                       (map[newY][newX] == CLEARING || map[newY][newX] == PATH || map[newY][newX] == TALL_GRASS)) {
                                minDist = distMap->nodes[newY][newX].dist;
                                nextX = newX;
                                nextY = newY;
                            }
                        }
                    }
                }
                // Move the Hiker to a valid cell with the lowest distance
                if (minDist != INF) {
                    // Additional check to prevent moving onto a border path
                    if (!(nextX == 0 || nextX == WIDTH - 1 || nextY == 0 || nextY == HEIGHT - 1) || map[nextY][nextX] != PATH) {
                        // Restore the old position to its previous state
                        map[y][x] = prevHikerCell;
                        // Save the current state before moving
                        prevHikerCell = map[nextY][nextX];
                        // Place the Hiker in the new position
                        map[nextY][nextX] = HIKER;
                    }
                }
                return; 
            }
        }
    }
}

void moveRival(char map[HEIGHT][WIDTH], DistanceMap *distMap) {
    if (rivalDefeated) return;
    int x, y, minDist = INF, nextX = 0, nextY = 0;
    // Find the Rival's current position
    for (y = 0; y < HEIGHT; y++) {
        for (x = 0; x < WIDTH; x++) {
            if (map[y][x] == RIVAL) {
                // Check all adjacent cells to find the one with the lowest distance value
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int newX = x + dx, newY = y + dy;
                        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
                            if (map[newY][newX] == PLAYER) {
                                // Trigger battle interface if Rival moves onto player
                                pokemonBattleInterface(map, x, y);
                                return; // Exit after battle
                            } else if (distMap->nodes[newY][newX].dist < minDist &&
                                       (map[newY][newX] == CLEARING || map[newY][newX] == PATH || map[newY][newX] == TALL_GRASS)) {
                                minDist = distMap->nodes[newY][newX].dist;
                                nextX = newX;
                                nextY = newY;
                            }
                        }
                    }
                }
                // Move the Rival to a valid cell with the lowest distance
                if (minDist != INF) {
                    // Additional check to prevent moving onto a border path
                    if (!(nextX == 0 || nextX == WIDTH - 1 || nextY == 0 || nextY == HEIGHT - 1) || map[nextY][nextX] != PATH) {
                        // Restore the old position to its previous state
                        map[y][x] = prevRivalCell;
                        // Save the current state before moving
                        prevRivalCell = map[nextY][nextX];
                        // Place the Rival in the new position
                        map[nextY][nextX] = RIVAL;
                    }
                }
                return;
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
                found = 1; 
            }
        }
    }

    if (found) {
        int attempts = 0;
        while (attempts < 8) { 
            
            int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1};
            int dy[8] = {0, 0, 1, -1, 1, 1, -1, -1};

            int newX = explorerX + dx[explorerDirection];
            int newY = explorerY + dy[explorerDirection];

            // Check if the new position is valid
            if (newX >= 1 && newX < WIDTH-1 && newY >= 1 && newY < HEIGHT-1) {
                char newPosType = map[newY][newX];
                // Trigger battle interface when the cell contains the PC
                if (newPosType == PLAYER && !explorerDefeated) {
                    pokemonBattleInterface(map, explorerX, explorerY);
                    break; 
                } else if (newPosType != WATER && newPosType != BOULDER && newPosType != TREE && newPosType != WANDERER && newPosType != PLAYER && newPosType != PACER) {
                    // Restore the old position to its previous state
                    map[explorerY][explorerX] = prevExplorerCell; 

                    // Save the current state before moving
                    prevExplorerCell = newPosType; 

                    // Move the Explorer to the new position
                    map[newY][newX] = EXPLORER; 
                    break;
                } else {
                    explorerDirection = rand() % 8;
                }
            }
            else {   
                explorerDirection = rand() % 8;
                }

            attempts++; 
        }
    }
}

void moveWanderer(char map[HEIGHT][WIDTH]) {
    static int wandererX, wandererY, direction = -1;
    static char spawnTerrain;

    // Initialize Wanderer's position and direction if not found previously
    if (!wandererfound) {
        for (int y = 0; y < HEIGHT && !wandererfound; y++) {
            for (int x = 0; x < WIDTH && !wandererfound; x++) {
                if (map[y][x] == WANDERER) {
                    wandererX = x;
                    wandererY = y;
                    wandererfound = 1; 

                    // Determine spawn terrain based on adjacent cells
                    int tallGrassCount = 0, clearingCount = 0;
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            // Skip the Wanderer's own cell
                            if (dx == 0 && dy == 0) continue; 
                            int checkX = wandererX + dx, checkY = wandererY + dy;
                            
                            if (checkX >= 0 && checkX < WIDTH && checkY >= 0 && checkY < HEIGHT) {
                                if (map[checkY][checkX] == TALL_GRASS) tallGrassCount++;
                                if (map[checkY][checkX] == CLEARING) clearingCount++;
                            }
                        }
                    }

                    // Set spawn terrain based on majority of adjacent cells
                    spawnTerrain = (tallGrassCount > clearingCount) ? TALL_GRASS : CLEARING;
                    
                    direction = rand() % 8; 
                }
            }
        }
    }

    int dx[8] = {1, -1, 0, 0, 1, -1, 1, -1};
    int dy[8] = {0, 0, 1, -1, 1, 1, -1, -1};

    // Attempt to move Wanderer if found
    if (wandererfound) {
        int newX = wandererX + dx[direction];
        int newY = wandererY + dy[direction];

        // Check if new position matches the Wanderer's spawn terrain
        if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT) {
            if (map[newY][newX] == PLAYER && !wandererDefeated) {
                pokemonBattleInterface(map, newX, newY);

                
                // If defeated, Wanderer's previous location to its spawnTerrain.
                if (!wandererDefeated) {
                    map[wandererY][wandererX] = spawnTerrain;
                    wandererX = newX; 
                    wandererY = newY;
                    map[newY][newX] = WANDERER;
                } else {
                    map[wandererY][wandererX] = spawnTerrain; 
                }
            } else if (map[newY][newX] == spawnTerrain && !wandererDefeated) {
                // Move the Wanderer to the new position if the new position matches the spawn terrain
                map[wandererY][wandererX] = spawnTerrain; 
                wandererX = newX; 
                wandererY = newY; 
                map[newY][newX] = WANDERER; 
            } else {
            
                direction = rand() % 8;
            }
        }
    }
}

void movePacer(char map[HEIGHT][WIDTH], int *pacerX, int *pacerY, int *direction) {
    int newX = *pacerX + *direction;
    if (newX >= 1 && newX < WIDTH-1) {
        char newPosType = map[*pacerY][newX];
        if (newPosType == PLAYER) {
            if (pacerDefeated == 0) {
                // Trigger the battle interface if Pacer not defeated
                pokemonBattleInterface(map, *pacerX, *pacerY);
                
            }
            //If defeated Pacer will still continue
        } else if ((newPosType == PATH || newPosType == TALL_GRASS || newPosType == CLEARING)) {
            map[*pacerY][*pacerX] = prevPacerCell; 

            // Save the new cell before moving
            prevPacerCell = map[*pacerY][newX];

            // Update Pacer position and place Pacer
            *pacerX = newX;
            map[*pacerY][*pacerX] = PACER;
        } else {
           
            *direction *= -1;
        }
    } else {
        
        *direction *= -1;
    }
}



void updatePacerPosition(char map[HEIGHT][WIDTH]) {
    // Check if Pacer has been initialized
    if (pacerX != -1 && pacerY != -1) { 
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


void printMap(char map[HEIGHT][WIDTH]) {
    mvprintw(0, 0, "Welcome to the Pokémon game!");

    // Adjust map printing to start below the welcome message
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            switch(map[y][x]) {
                case TALL_GRASS:
                    attron(COLOR_PAIR(TALL_GRASS_COLOR));
                    break;
                case CLEARING:
                    attron(COLOR_PAIR(CLEARING_COLOR));
                    break;
                case PATH:
                    attron(COLOR_PAIR(PATH_COLOR));
                    break;
                case WATER:
                    attron(COLOR_PAIR(WATER_COLOR));
                    break;
                case BOULDER:
                    attron(COLOR_PAIR(BOULDER_COLOR));
                    break;
                default:
                    attroff(COLOR_PAIR(TALL_GRASS_COLOR));
                    attroff(COLOR_PAIR(CLEARING_COLOR));
                    attroff(COLOR_PAIR(PATH_COLOR));
                    attroff(COLOR_PAIR(WATER_COLOR));
                    attroff(COLOR_PAIR(BOULDER_COLOR));
            }
            // Adjust the y position by 1 to because the welcome message
            mvaddch(y + 1, x, map[y][x]);
            attroff(COLOR_PAIR(TALL_GRASS_COLOR));
            attroff(COLOR_PAIR(CLEARING_COLOR));
            attroff(COLOR_PAIR(PATH_COLOR));
            attroff(COLOR_PAIR(WATER_COLOR));
            attroff(COLOR_PAIR(BOULDER_COLOR));
        }
    }

    refresh();
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << "[pokemon|moves|pokemon_moves|pokemon_species|experience|type_names|pokemon_stats|stats|pokemon_types]" << std::endl;
        return 1;
    }

    // Construct the filename from the command-line argument
    std::string filename = std::string(argv[1]) + ".csv";

    std::ifstream file = openFile(filename);

    // Continue with file processing if file opened successfully
    if (file.is_open()) {
        // Process the file
        std::cout << "File opened successfully: " << filename << std::endl;

        if (filename == "pokemon.csv") {
            auto pokemons = parsePokemonCsv(finalpath);

            for (const auto& pokemon : pokemons) {
                std::cout << "ID: " << pokemon.id
                        << ", Identifier: " << pokemon.identifier
                        << ", Species ID: " << pokemon.species_id
                        << ", Height: " << pokemon.height
                        << ", Weight: " << pokemon.weight
                        << ", Base Experience: " << pokemon.base_experience
                        << ", Order: " << pokemon.order
                        << ", Is Default: " << pokemon.is_default
                        << std::endl;
            }
        exit(0);
        } else if (filename == "moves.csv") {
            std::string line;
            std::vector<Move> moves;
            bool firstLine = true;

            while(std::getline(file, line)) {
                if(firstLine) { // Skip the header row
                    firstLine = false;
                    continue;
                }
                moves.push_back(Move(line));
            }

            for(const auto& move : moves) {
                move.print();
            }
            exit(0);
        } else if (filename == "pokemon_moves.csv") {
            auto pokemonMoves = parsePokemonMovesCsv(finalpath);

            for (const auto& move : pokemonMoves) {
                std::cout << "Pokemon ID: " << move.pokemon_id
                        << ", Version Group ID: " << move.version_group_id
                        << ", Move ID: " << move.move_id
                        << ", Pokemon Move Method ID: " << move.pokemon_move_method_id
                        << ", Level: " << move.level;
                if (move.order != INT_MAX) {
                    std::cout << ", Order: " << move.order;
                }
                std::cout << std::endl;
            }
            exit(0);
        } else if (filename == "pokemon_species.csv") {
            auto species = parsePokemonSpeciesCsv(finalpath);

            for (const auto& specie : species) {
                std::cout << "ID: " << specie.id
                        << ", Identifier: " << specie.identifier
                        << ", Generation ID: " << specie.generation_id
                        << std::endl;
            }
            exit(0);
        }else if (filename == "experience.csv") {
        auto experiences = parseExperienceCsv(finalpath);

        for (const auto& exp : experiences) {
            std::cout << "Growth Rate ID: " << exp.growth_rate_id
                    << ", Level: " << exp.level
                    << ", Experience: " << exp.experience << std::endl;
        }
        exit(0);
        }else if (filename == "type_names.csv") {
            auto typeNames = parseTypeNamesCsv(finalpath);

            for (const auto& typeName : typeNames) {
                std::cout << "Type ID: " << typeName.type_id
                        << ", Local Language ID: " << typeName.local_language_id
                        << ", Name: " << typeName.name << std::endl;
            }
            exit(0);
        }else if(filename == "pokemon_stats.csv"){
            auto pokemonStats = parsePokemonStatsCsv(finalpath);

            for (const auto& stat : pokemonStats) {
                std::cout << "Pokemon ID: " << stat.pokemon_id
                        << ", Stat ID: " << stat.stat_id
                        << ", Base Stat: " << stat.base_stat
                        << ", Effort: " << stat.effort << std::endl;
            }
            exit(0);
        }else if(filename == "stats.csv"){
            auto stats = parseStatsCsv(finalpath);

            for (const auto& stat : stats) {
                std::cout << "ID: " << stat.id
                        << ", Damage Class ID: " << (stat.damage_class_id != INT_MAX ? std::to_string(stat.damage_class_id) : "N/A")
                        << ", Identifier: " << stat.identifier
                        << ", Is Battle Only: " << stat.is_battle_only
                        << ", Game Index: " << (stat.game_index != INT_MAX ? std::to_string(stat.game_index) : "N/A") << std::endl;
            }
            exit(0);
        }else if (filename == "pokemon_types.csv") {
            auto pokemonTypes = parsePokemonTypesCsv(finalpath);

            for (const auto& pokemonType : pokemonTypes) {
                std::cout << "Pokemon ID: " << pokemonType.pokemon_id
                        << ", Type ID: " << pokemonType.type_id
                        << ", Slot: " << pokemonType.slot << std::endl;
            }
            exit(0);
        }

    
    }

    initscr();     // Start curses mode
    start_color(); // Start the color functionality
    cbreak();            
    keypad(stdscr, TRUE); 
    noecho();           

    // Initialize color pairs
    init_pair(TALL_GRASS_COLOR, COLOR_GREEN, COLOR_BLACK);
    init_pair(CLEARING_COLOR, COLOR_YELLOW, COLOR_BLACK); 
    init_pair(PATH_COLOR, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(WATER_COLOR, COLOR_BLUE, COLOR_BLACK);
    init_pair(BOULDER_COLOR, COLOR_WHITE, COLOR_BLACK);

    char prevPlayerCell = PATH; 
    int numTrainers = 6;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--numtrainers") == 0 && i + 1 < argc) {
            numTrainers = atoi(argv[i + 1]);
            
            i++; 
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


    if (numTrainers > 0) {
        placeAdversary(map, RIVAL); 
    }
    if (numTrainers > 1) {
        placeAdversary(map, HIKER); 
    }
    if (numTrainers > 2) {
        placeAdversary(map, PACER); 
    }
    if (numTrainers > 3) {
        placeAdversary(map, WANDERER); 
    }
    if (numTrainers > 4) {
        placeAdversary(map, EXPLORER); 
    }
    if (numTrainers > 5) {
        placeAdversary(map, SENTRY); 
    }

    int extraSentries = numTrainers - 6; 
        for (int i = 0; i < extraSentries; i++) {
            // Place an additional Sentry for each number above 6
            placeAdversary(map, SENTRY); 
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

    printMap(map); 

    DistanceMap distMapHiker, distMapRival;
    dijkstra(map, &distMapHiker, playerX, playerY, 0); 
    dijkstra(map, &distMapRival, playerX, playerY, 1); 

    // Game loop
    while (1) {
        // Get user input
        int ch = getch(); 

        int newX = playerX;
        int newY = playerY;

        // Handle movement based on key press
        switch(ch) {
            
            case '7': case 'y': // Move up-left
                if (playerExistsOnMap) {
                    newX--; newY--;
                }
                break;
            case '8': case 'k': // Move up
                if (playerExistsOnMap) {
                    newY--;
                }
                
                break;
            case '9': case 'u': // Move up-right
                if (playerExistsOnMap) {
                    newX++; newY--;
                }
                
                break;
            case '6': case 'l': // Move right
                if (playerExistsOnMap) {
                    newX++;
                }
                break;
            case '3': case 'n': // Move down-right
            if (playerExistsOnMap) {
                newX++; newY++;
            }
                break;
            case '2': case 'j': // Move down
            if (playerExistsOnMap) {
                newY++;
            }
                break;
            case '1': case 'b': // Move down-left
            if (playerExistsOnMap) {
                newX--; newY++;
            }
                break;
            case '4': case 'h': // Move left
            if (playerExistsOnMap) {
                newX--;
            }
                break;
            case '5': case ' ': case '.': // Rest - do not move
            break;
            
            case 'f':; // Start fly logic    
                echo();
                mvprintw(HEIGHT + 2, 0, "Enter new x and y coordinates (format: x y): ");
                int newPlayerX, newPlayerY;
                //Input coordinates to fly to
                scanw("%d %d", &newPlayerX, &newPlayerY); 
                noecho();
                if (newPlayerX >= -200 && newPlayerX < 200 && newPlayerY >= -200 && newPlayerY < 200) {
                    saveMap(maps, map, currentX, currentY); // Save the current map
                    currentX = newPlayerX + MAX_MAPS / 2; 
                    currentY = newPlayerY + MAX_MAPS / 2;
                    loadMap(maps, map, currentX, currentY); // Load or generate the new map

                    // Check if the player already exists on the map
                    playerExistsOnMap = false;
                    for (int y = 0; y < HEIGHT && !playerExistsOnMap; y++) {
                        for (int x = 0; x < WIDTH && !playerExistsOnMap; x++) {
                            if (map[y][x] == PLAYER) {
                                playerY = y; // Update player's position to existing one
                                playerX = x;
                                playerExistsOnMap = true;
                            }
                        }
                    }

                    //Find a CLEARING and place the player there
                    if (!playerExistsOnMap) {
                        for (int y = 0; y < HEIGHT && !playerExistsOnMap; y++) {
                            for (int x = 0; x < WIDTH && !playerExistsOnMap; x++) {
                                if (map[y][x] == CLEARING) {
                                    map[y][x] = PLAYER;
                                    playerX = x;
                                    playerY = y;
                                    playerExistsOnMap = true;
                                }
                            }
                        }
                    }

                    // Update dijkstra maps since player's position might be different
                    dijkstra(map, &distMapHiker, playerX, playerY, 0);
                    dijkstra(map, &distMapRival, playerX, playerY, 1);
                } else {
                    mvprintw(HEIGHT + 3, 0, "Invalid coordinates. Please enter values between -200 and 199.");
                }
                clear(); 
                break;

            case '>':
                if (prevPlayerCell == POKEMON_CENTER || prevPlayerCell == POKEMART) {
                int inputKey;
                clear(); 
                // Display the appropriate message based on prevPlayerCell
                const char *location = (prevPlayerCell == POKEMON_CENTER) ? "Pokémon Center" : "PokéMart";
                mvprintw(0, 0, "Welcome to the %s! Press < to leave.", location);
                refresh(); 
                
                do {
                    inputKey = getch(); 
                } while (inputKey != '<'); 
                clear(); 
                }
            break;    
            case 'Q': // Quit the game
                endwin();
                return 0;
            
        }

        // Handle 't' key press for displaying adversary distances
        if (ch == 't') {
            int startLine = 0; // Start displaying from the first line
            int totalLines = 0; 
            int maxLine = HEIGHT - 3; 
            bool done = false;

            // Calculate totalLines based on number of adversaries
            for (int y = 0; y < HEIGHT; y++) {
                for (int x = 0; x < WIDTH; x++) {
                    if (map[y][x] == RIVAL || map[y][x] == SENTRY || map[y][x] == PACER || map[y][x] == WANDERER || map[y][x] == EXPLORER || map[y][x] == HIKER) {
                        totalLines++;
                    }
                }
            }

            while (!done) {
                clear(); // Clear the screen to hide the map
                mvprintw(0, 0, "Press the escape key to go back to the map. Use arrow keys to scroll.");
                
                int line = 0; // Counter for lines printed
                int printed = 0; // Counter for lines printed on screen

                // Print part of list that fits on screen starting from the top
                for (int y = 0; y < HEIGHT && printed < maxLine; y++) {
                    for (int x = 0; x < WIDTH && printed < maxLine; x++) {
                        if ((map[y][x] == RIVAL || map[y][x] == SENTRY || map[y][x] == PACER || map[y][x] == WANDERER || map[y][x] == EXPLORER || map[y][x] == HIKER) && line >= startLine) {
                            int deltaX = x - playerX; 
                            int deltaY = playerY - y;
                            const char* horizontal = (deltaX > 0) ? "east" : "west";
                            const char* vertical = (deltaY > 0) ? "north" : "south";
                            deltaX = abs(deltaX);
                            deltaY = abs(deltaY);
                            mvprintw(2 + printed, 0, "%s: %d %s and %d %s", 
                                map[y][x] == RIVAL ? "Rival (R)" : 
                                map[y][x] == SENTRY ? "Sentry (S)" : 
                                map[y][x] == PACER ? "Pacer (P)" : 
                                map[y][x] == WANDERER ? "Wanderer (W)" : 
                                map[y][x] == EXPLORER ? "Explorer (E)" : 
                                "Hiker (H)", 
                                deltaX, horizontal, deltaY, vertical);
                            printed++;
                        }
                        if (map[y][x] == RIVAL || map[y][x] == SENTRY || map[y][x] == PACER || map[y][x] == WANDERER || map[y][x] == EXPLORER || map[y][x] == HIKER) {
                            line++;
                        }
                    }
                }
                
                refresh(); // Refresh the screen to show the output
                int inputKey = getch(); 


                switch (inputKey) {
                    case 27: // ESC key
                        done = true;
                        break;
                    case KEY_UP:
                        startLine--;
                        break;
                    case KEY_DOWN:
                        startLine++;
                        break;
                }
            }

            clear(); 
            }
            char newPosType = map[newY][newX];
            // Check if new position is within bounds and not a blocked cell
            if (newX >= 1 && newX < WIDTH-1 && newY >= 1 && newY < HEIGHT-1) {
                char newPosType = map[newY][newX];

                
                // Check if the new position is occupied by a defeated NPC
                if ((newPosType == HIKER && hikerDefeated) || (newPosType == RIVAL && rivalDefeated) || (newPosType == PACER && pacerDefeated) || (newPosType == EXPLORER && explorerDefeated) || (newPosType == WANDERER && wandererDefeated) || (newPosType == SENTRY && sentryDefeated)) {
                    // If the Hiker or Rival is defeated, player cant move into cell
                }
                else if (newPosType != BOULDER && newPosType != WATER && newPosType != TREE && newPosType != PACER && newPosType != WANDERER) {
                    //Starts battle if NPC isn't defeated
                    if (newPosType == HIKER || newPosType == RIVAL || newPosType == SENTRY || newPosType == PACER || newPosType == WANDERER || newPosType == EXPLORER) {
                        pokemonBattleInterface(map, newX, newY);
                        // After returning from the battle, check if the cell is still occupied
                        // If NPC is not defeated (still there), revert player's move
                        if ((newPosType == HIKER && !hikerDefeated) || (newPosType == RIVAL && !rivalDefeated) || (newPosType == WANDERER&& !wandererDefeated)) {
                            newX = playerX;
                            newY = playerY;
                        }
                    }

                    //let's player move if possible
                    if (newX != playerX || newY != playerY) {  // Check if the move is allowed
                        map[playerY][playerX] = prevPlayerCell; // Restore the old position
                        prevPlayerCell = map[newY][newX]; // Save the new cell's state
                        playerX = newX;
                        playerY = newY;
                        map[playerY][playerX] = PLAYER; // Place the player in the new cell

                        dijkstra(map, &distMapHiker, playerX, playerY, 0); 
                        dijkstra(map, &distMapRival, playerX, playerY, 1); 
                    }
                }
            }

            if ((newX == 0 || newX == WIDTH - 1 || newY == 0 || newY == HEIGHT - 1) && newPosType == PATH) {
                // Save the current cell the player is on before moving to a new map
                map[playerY][playerX] = prevPlayerCell; 

                // Calculate new map coordinates based on the direction of movement
                int newMapX = currentX + (newX == 0 ? -1 : (newX == WIDTH - 1 ? 1 : 0));
                int newMapY = currentY + (newY == 0 ? -1 : (newY == HEIGHT - 1 ? 1 : 0));

                // Save the current map before transitioning
                saveMap(maps, map, currentX, currentY);

                // Update current map coordinates to the new map
                currentX = newMapX;
                currentY = newMapY;

                // Load or create the new map
                loadMap(maps, map, currentX, currentY);

                // Update player position on the new map corresponding to the direction of entry
                playerX = newX == 0 ? WIDTH - 2 : (newX == WIDTH - 1 ? 1 : playerX);
                playerY = newY == 0 ? HEIGHT - 2 : (newY == HEIGHT - 1 ? 1 : playerY);

                // Set the player's previous cell type for the new map
                prevPlayerCell = PATH;

                // Place the player on the new map
                map[playerY][playerX] = PLAYER;

                // Refresh distance maps due to the new player location
                dijkstra(map, &distMapHiker, playerX, playerY, 0);
                dijkstra(map, &distMapRival, playerX, playerY, 1);
            }

        // Update positions of all adversaries after player's move
        moveHiker(map, &distMapHiker);
        moveRival(map, &distMapRival);
        updatePacerPosition(map);
        moveExplorer(map);
        moveWanderer(map);

        // Update and print the map
        printMap(map);

        
    }

    return 0;
}