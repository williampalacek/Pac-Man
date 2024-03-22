// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

#include <stdlib.h>
#include "defines.h"
#include "colours.h"
#include "map.h"
#include <stdio.h>

extern char *map, *dot_map;
extern int width, height, pacman_y, pacman_x, ghosts_y[NUM_GHOSTS], ghosts_x[NUM_GHOSTS];

// Moves an actor (Pacman or ghosts) on the map based on direction, with the option to eat dots.
int move_actor(int * y, int * x, char direction, int eat_dots) {
    int newY = *y, newX = *x;
    int index = newY * width + newX;

    // Update position based on direction
    switch (direction) {
        case UP:    newY -= 1; break;
        case DOWN:  newY += 1; break;
        case LEFT:  newX -= 1; break;
        case RIGHT: newX += 1; break;
        default:    return MOVED_INVALID_DIRECTION;
    }

    // Abort if new position is a wall
    if (is_wall(newY, newX) == YES_WALL) {
        return MOVED_WALL;
    }

    // Eat dot at new position if applicable
    if (eat_dots == EAT_DOTS && dot_map[index] == DOT) {
        dot_map[index] = EMPTY; // Pacman eats the dot
    }

    // Update actor's position only if moving to a non-wall
    *y = newY;
    *x = newX;


    return MOVED_OKAY;
}

// Updates the main map from dot_map, placing Pacman and ghosts.
int update_map(void){
    // Copy the dot_map to the main map, resetting previous positions
    for(int i = 0; i < width * height; i++) {
        map[i] = dot_map[i];
    }

    // Place Pacman on the map and mark his position
    int pacman_index = pacman_y * width + pacman_x;
    map[pacman_index] = PACMAN;

    // Place each ghost on the map and mark there position
    for (int i = 0; i < NUM_GHOSTS; i++) {
        int ghost_index = ghosts_y[i] * width + ghosts_x[i];
        map[ghost_index] = GHOST;
    }

    return NO_ERROR;
}

// Prints the game map with colored symbols for different actors.
int print_map(void) {
    update_map();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char mapChar = map[y * width + x];

            // Color-code output
            if (mapChar == PACMAN) {
                change_text_colour(YELLOW);
            } else if (mapChar == GHOST) {
                change_text_colour(PINK);
            } else if (mapChar == WALL) {
                change_text_colour(BLUE);
            } else {
                change_text_colour(WHITE); // Default for dots and empty spaces
            }

            printf("%c ", mapChar); // Print the character with a space for readability
        }
        change_text_colour(WHITE); // Reset to default color after each line
        printf("\n");
    }

    return NO_ERROR;
}

// Determines if a given map location is a wall.
int is_wall(int y, int x) {
    // Ensure the coordinates are within the bounds of the map
    if (y < 0 || y >= height || x < 0 || x >= width) {
        return YES_WALL; // Out of bounds can be considered a wall or return an error
    }

    int index = y * width + x; // Calculate the index in the flat array

    // Check if the character at the index is a wall ('W')
    if (map[index] == WALL) {
        return YES_WALL;
    } else {
        return NOT_WALL;
    }
}

// Finds initial positions of Pacman and ghosts from the map.
int find_initial_positions(void) {
    int ghost_count = 0;
    pacman_x = pacman_y = -1; // Reset positions to impossible values

    // Initialize ghost positions
    for(int i = 0; i < NUM_GHOSTS; i++) ghosts_y[i] = ghosts_x[i] = 0;

    // Scan map for Pacman and ghosts
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x; // Calculate the index in the flat map array.

            if (map[index] == PACMAN) {
                pacman_y = y;
                pacman_x = x;
            } else if (map[index] == GHOST && ghost_count < NUM_GHOSTS) {
                ghosts_y[ghost_count] = y;
                ghosts_x[ghost_count] = x;
                ghost_count++;
            }
        }
    }

    // Return error codes if Pacman or ghosts are not found
    if(pacman_x == -1 || pacman_y == -1){
        return ERR_NO_PACMAN;
    }
    else if(ghost_count == 0){
        return ERR_NO_GHOSTS;
    }
    else{
        return NO_ERROR;
    }
}

// Loads and initializes the game map from a file.
char * load_map(char *filename, int *map_height, int *map_width) {
    *map_height = height = 1; // Initial size
    *map_width = width = 0;

    int mapIndex, elementCount = 0, errorKey;
    char ch, *tempMap = NULL, *tempDotMap = NULL;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open the file.\n");
        return (char *) ERR_NO_MAP; // File read error
    }

    while((ch= fgetc(file)) != EOF){
        if(ch == '\n'){
            height++;
            elementCount = 0;
        }
        else if(ch != EMPTY && height == 1) {
            width++;
            elementCount++;
        }
    }
    if(elementCount > 0) height++;

    // Adjust width and height for walls
    width += 2; // Adding walls on both sides
    //width *=2;
    height += 2; // Adding walls on the top and bottom

    // Allocate memory for maps
    tempMap = (char*)malloc(width * height * sizeof(char));
    tempDotMap = (char*)malloc(width * height * sizeof(char));

    if (!tempMap || !tempDotMap) {
        printf("Error: Memory allocation failed.\n");
        free(tempMap); // Ensure to free allocated memory to avoid leaks
        free(tempDotMap);
        fclose(file);
        return NULL; // Allocation failure
    }

    // Initialize maps with walls and empty spaces
    for(int i = 0; i < width * height; i++) {
        // Top row or sides or Bottom row
        if(i < width|| i % width == 0 || (i+1) % width == 0 || i >= width * (height - 1)) {
            tempDotMap[i] = WALL;
            tempMap[i] = WALL;
        } else {
            tempDotMap[i] = EMPTY; // Initialize with space
            tempMap[i] = EMPTY;
        }
    }

    // Reload file and populate maps
    rewind(file); // Go back to the start of the file to read the map content
    mapIndex = width + 1; // Start just after the top wall and first side wall
    while((ch = fgetc(file)) != EOF) {
        if(ch == '\n') {
            mapIndex += 2; // Skip the last side wall and first side wall of next line
        } else if(ch != EMPTY) {
            tempMap[mapIndex] = ch;
            if(ch != GHOST && ch != PACMAN){
                tempDotMap[mapIndex++] = ch;
            }
            else{
                tempDotMap[mapIndex++] = ' ';
            }
        }
    }

    fclose(file);

    // Assign global pointers and sizes
    map = tempMap;
    dot_map = tempDotMap;
    *map_height = height;
    *map_width = width;

    // Find and validate initial positions
    errorKey = find_initial_positions();

    if (errorKey == ERR_NO_PACMAN){
        return (char *) ERR_NO_PACMAN;
    }
    else if (errorKey == ERR_NO_GHOSTS){
        return (char *) ERR_NO_GHOSTS;
    }

    return map;
}
