// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

#include "defines.h" // Include the defines header for constants and macro definitions
#include "game.h"    // Include game-related function prototypes and definitions
#include <stdlib.h>  // Standard library for memory management functions

// Declare external global variables defined elsewhere for map and game state management
extern char *map, *dot_map; // Pointers to the game map and dot map
extern int height, width;   // Variables for the dimensions of the game map

/**
 * Checks whether the win condition is met by verifying if all dots have been collected.
 * Iterates over the dot_map to find any remaining dots.
 * @param pacman_y Pacman's current vertical position
 * @param pacman_x Pacman's current horizontal position
 * @param ghosts_y Array containing the vertical positions of all ghosts
 * @param ghosts_x Array containing the horizontal positions of all ghosts
 * @return YOU_WIN if no dots are left, KEEP_GOING otherwise
 */
int check_win(int pacman_y, int pacman_x, int ghosts_y[NUM_GHOSTS], int ghosts_x[NUM_GHOSTS]) {
    // Loop through each cell of the game map
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x; // Calculate the flat array index for the current cell
            // If a dot is found, the game is not won yet
            if (dot_map[index] == DOT) {
                return KEEP_GOING; // A dot is still present on the map
            }
        }
    }
    // If this point is reached, no dots are left, and Pacman wins
    return YOU_WIN;
}

/**
 * Checks whether the loss condition is met by verifying if Pacman is caught by any ghost.
 * Compares Pacman's position with each ghost's position.
 * @param pacman_y Pacman's current vertical position
 * @param pacman_x Pacman's current horizontal position
 * @param ghosts_y Array containing the vertical positions of all ghosts
 * @param ghosts_x Array containing the horizontal positions of all ghosts
 * @return YOU_LOSE if Pacman is caught by a ghost, KEEP_GOING otherwise
 */
int check_loss(int pacman_y, int pacman_x, int ghosts_y[NUM_GHOSTS], int ghosts_x[NUM_GHOSTS]) {
    // Loop through each ghost to check for collision with Pacman
    for (int i = 0; i < NUM_GHOSTS; i++) {
        // If Pacman's position matches any ghost's position, Pacman is caught
        if (pacman_y == ghosts_y[i] && pacman_x == ghosts_x[i]) {
            return YOU_LOSE; // Pacman is caught by a ghost
        }
    }
    // If no ghost has caught Pacman, the game continues
    return KEEP_GOING;
}

/**
 * Frees the dynamically allocated memory for the game and dot maps when the game ends.
 * Ensures no memory leaks occur by setting pointers to NULL after freeing.
 */
void cleanup_game_resources(void) {
    // Free the memory allocated for the game map and set the pointer to NULL
    if (map != NULL) {
        free(map);
        map = NULL;
    }
    // Free the memory allocated for the dot map and set the pointer to NULL
    if (dot_map != NULL) {
        free(dot_map);
        dot_map = NULL;
    }
}
