// Including necessary libraries and header files for functionality and global variables
#include <stdlib.h>  // Standard library for dynamic memory allocation
#include "defines.h" // Header file for predefined constants and macros
#include "colours.h" // For colored text output
#include "map.h"     // Map manipulation functions
#include <stdio.h>   // Standard I/O operations
#include "game.h"    // Game-specific functions

// External variables declared in another file, used to maintain game state
extern char *map, *dot_map; // Pointers to the game map and dot map for tracking dots
extern int width, height; // Variables for storing the dimensions of the game map
int pacman_y, pacman_x; // Variables for storing Pacman's position
int ghosts_y[NUM_GHOSTS], ghosts_x[NUM_GHOSTS]; // Arrays for storing ghosts' positions

/**
 * Moves an actor (Pacman or a ghost) on the game map.
 * This function updates the actor's position based on the provided direction and optionally eats dots.
 *
 * Parameters:
 * - y: Pointer to the actor's current y-coordinate
 * - x: Pointer to the actor's current x-coordinate
 * - direction: The direction to move the actor
 * - eat_dots: A flag indicating whether to eat dots (applicable for Pacman)
 *
 * Returns:
 * - An integer indicating the result of the move operation (e.g., success, hit a wall, invalid direction)
 */
int move_actor(int *y, int *x, char direction, int eat_dots) {
    // Calculate potential new position
    int newY = *y, newX = *x, mapIndex, oldMapIndex = *y * width + *x;

    switch (direction) {
        case UP:    newY--; break;
        case DOWN:  newY++; break;
        case LEFT:  newX--; break;
        case RIGHT: newX++; break;
        default:    return MOVED_INVALID_DIRECTION;
    }

    // Check if new position is a wall
    if (is_wall(newY, newX) == YES_WALL) {
        return MOVED_WALL;
    }

    // For Pacman moving and eating dots
    if (eat_dots == EAT_DOTS) {
        mapIndex = newY * width + newX;
        if (dot_map[mapIndex] == DOT) {
            dot_map[mapIndex] = EMPTY; // Pacman eats the dot
        }
    }

    // Update Pacman's or Ghost's previous position on the main map
    mapIndex = *y * width + *x;
    if (map[oldMapIndex] == PACMAN) {
        if (dot_map[mapIndex] == DOT) {
            map[mapIndex] = DOT;
        } else {
            map[mapIndex] = EMPTY;
        }
    } else {
        if(map[oldMapIndex] == GHOST){
            map[oldMapIndex] = DOT; // Update previous position of Ghost to dot
        }
        else{
            map[mapIndex] = EMPTY;
        }

    }

    // Update the actor's position
    *y = newY;
    *x = newX;

    // Update Pacman's or Ghost's new position on the main map immediately
    mapIndex = newY * width + newX;
    map[mapIndex] = (eat_dots == EAT_DOTS) ? PACMAN : GHOST;

    return MOVED_OKAY;
}

/**
 * Updates the main game map based on the dot map and current positions of Pacman and the ghosts.
 *
 * This function copies the dot map to the main map and then places Pacman and the ghosts on the map.
 *
 * Returns:
 * - NO_ERROR indicating the operation was successful
 */
int update_map(void){
    // Copy dot map to main map, preparing for re-drawing actors
    for(int i = 0; i < width * height; i++) {
        map[i] = dot_map[i];
    }

    // Place Pacman on the map
    int pacman_index = pacman_y * width + pacman_x;
    map[pacman_index] = PACMAN;

    // Place ghosts on the map
    for (int i = 0; i < NUM_GHOSTS; i++) {
        int ghost_index = ghosts_y[i] * width + ghosts_x[i];
        map[ghost_index] = GHOST;
    }

    return NO_ERROR; // Operation completed successfully
}

/**
 * Prints the game map with colored symbols for different entities (Pacman, ghosts, walls).
 *
 * This function first updates the map to reflect the current state, then prints each character
 * of the map with appropriate color coding.
 *
 * Returns:
 * - NO_ERROR indicating the operation was successful
 */
int print_map(void) {
    update_map(); // Ensure map reflects current state

    // Print top outer wall
    for (int x = 0; x < width + 2; x++) {
        change_text_colour(BLUE);
        printf("W ");
    }
    printf("\n");

    for (int y = 0; y < height; y++) {
        change_text_colour(BLUE);
        printf("W "); // Left outer wall
        for (int x = 0; x < width; x++) {
            char mapChar = map[y * width + x];
            switch(mapChar) {
                case PACMAN: change_text_colour(YELLOW); break;
                case GHOST:  change_text_colour(PINK); break;
                case WALL:   change_text_colour(BLUE); break;
                default:     change_text_colour(WHITE);
            }
            printf("%c ", mapChar);
            change_text_colour(WHITE);
        }
        change_text_colour(BLUE);
        printf("W\n"); // Right outer wall
    }

    // Print bottom outer wall
    for (int x = 0; x < width + 2; x++) {
        change_text_colour(BLUE);
        printf("W ");
    }
    printf("\n");

    change_text_colour(WHITE); // Reset color for next print
    return NO_ERROR; // Indicating successful operation
}

/**
 * Checks if a specified map location is a wall.
 *
 * Parameters:
 * - y: The y-coordinate of the location to check
 * - x: The x-coordinate of the location to check
 *
 * Returns:
 * - YES_WALL if the location is a wall, NOT_WALL otherwise
 */
int is_wall(int y, int x) {
    // Check bounds to consider out-of-bounds as walls
    if (y < 0 || y >= height || x < 0 || x >= width) {
        return YES_WALL; // Treat out-of-bounds as walls
    }

    int index = y * width + x; // Calculate index in the map array

    // Determine if the location is a wall
    return (map[index] == WALL) ? YES_WALL : NOT_WALL;
}

// Finds initial positions of Pacman and ghosts from the map.
int find_initial_positions(char* tempMap) {
    int ghost_count = 0; // Counter for found ghosts
    pacman_x = pacman_y = -1; // Reset positions to impossible values

    // Initialize ghost positions
    for(int i = 0; i < NUM_GHOSTS; i++) ghosts_y[i] = ghosts_x[i] = 0;

    // Scan map for Pacman and ghosts, updating positions
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x; // Calculate index for map character

            if (tempMap[index] == PACMAN) {
                pacman_y = y;
                pacman_x = x;
            } else if (tempMap[index] == GHOST && ghost_count < NUM_GHOSTS) {
                ghosts_y[ghost_count] = y;
                ghosts_x[ghost_count] = x;
                ghost_count++;
            }
        }
    }

    // Return error codes if Pacman or ghosts are not found
    if(pacman_x == -1 || pacman_y == -1){
        //cleanup_game_resources();
        return ERR_NO_PACMAN;
    }
    else if(ghost_count == 0){
        //cleanup_game_resources();
        return ERR_NO_GHOSTS;
    }
    else{
        return NO_ERROR;
    }
}

/**
 * Loads the game map from a specified file and initializes the game state.
 * This function allocates memory for the map and dot map, reads the file to populate the map,
 * and then initializes the game state based on the map contents.
 *
 * Parameters:
 * - filename: Name of the file to load the map from
 * - map_height: Pointer to store the loaded map's height
 * - map_width: Pointer to store the loaded map's width
 *
 * Returns:
 * - A pointer to the loaded map, or an error code cast to a char* if an error occurred
 */
char * load_map(char *filename, int *map_height, int *map_width) {
    // Initialize map dimensions.
    height = 1; // Start with a height of 1
    width = 0; // Start with no width

    // Declare variables for processing the file and map.
    int mapIndex, elementCount = 0, errorKey;
    char ch, *tempMap = NULL;

    // Open the file for reading. If it fails, print an error and exit the function.
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open the file.\n");
        return NULL; // Return an error code for "no map found".
    }

    // Read the file character by character to determine the map's width and height.
    while((ch = fgetc(file)) != EOF){
        if(ch == '\n'){
            height++; // Increment height for each new line.
            elementCount = 0; // Reset element count for the new line.
        }
        else if(ch != EMPTY && height == 1) {
            width++; // Increment width for each character in the first line.
            elementCount++;
        }
    }
    // Adjust height if there are trailing characters in the last line.
    if(elementCount > 0) height++;

    // Allocate memory for the map and a secondary "dot" map.
    tempMap = (char*)malloc(width * height * sizeof(char));


    // Check for memory allocation failure.
    if (!tempMap) {
        printf("Error: Memory allocation failed.\n");
        free(tempMap);

        fclose(file);
        return NULL;
    }

    // Reset file pointer to start for loading the map
    rewind(file);
    int y = 0, x = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            y++;
            x = -1;
        } else if (x%3 ==0 || x==0) {
            tempMap[y * width + x/3] = ch;
        }
        x++;
    }
    // Close the file as it is no longer needed.
    fclose(file);

    // Update global map dimensions.
    *map_height = height;
    *map_width = width;
    // Validate initial positions of Pacman and ghosts, return error codes if not found.
    errorKey = find_initial_positions(tempMap);

    if (errorKey == ERR_NO_PACMAN){
        cleanup_game_resources();
        return (char *) ERR_NO_PACMAN; // Pacman missing.
    }
    else if (errorKey == ERR_NO_GHOSTS){
        cleanup_game_resources();
        return (char *) ERR_NO_GHOSTS; // Ghosts missing.
    }


    // Return the pointer to the populated map.
    return tempMap;
}

void fill_initial_dot_map(void) {
    dot_map = (char*)malloc(width * height * sizeof(char));
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;

            if(map[index] == PACMAN ||map[index] == GHOST){
                dot_map[index] = EMPTY;
            }
            else{
                dot_map[index] = map[index];
            }

        }
    }
}