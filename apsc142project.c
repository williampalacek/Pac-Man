// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

// don't forget to update your project configuration to select "Emulate terminal in the output console"

// Make sure to include all relevant libraries
#include <stdio.h>
#include <stdlib.h>

// colours.h contains functions to change text colour and read single characters without requiring an enter
#include "colours.h"
// defines.h contains useful definitions to keep your code readable
#include "defines.h"
// map.h, game.h, and ghost.h contain prototypes of functions you must implement
#include "map.h"
#include "game.h"
#include "ghost.h"

// These global variables must be used to store map information.
// Almost every function needs these variables, so keeping them as globals helps keep things organized.
// map is a pointer to a dynamically allocated map for displaying to the user
// dot_map is a pointer to a dynamically allocated map for keeping track of what dots are left


// Global variables for storing the game state. Using globals facilitates access across multiple functions.
char *map = NULL, *dot_map = NULL; // Pointers to dynamically allocated maps for display and dot tracking
int width, height;                 // Dimensions of the map, excluding outer walls
int pacman_y, pacman_x;            // Pacman's position coordinates
int ghosts_y[NUM_GHOSTS], ghosts_x[NUM_GHOSTS]; // Arrays to store positions of ghosts



/**
 * Main entry point into your program.
 * Make sure that main returns appropriate status codes depending on what
 * happens.  The codes you must use are:
 *   NO_ERROR when no error occurs
 *   ERR_NO_MAP when no map file is found
 *   ERR_NO_PACMAN when no pacman is found on the map
 *   ERR_NO_GHOSTS when fewer than 2 ghosts are found on the map
 *
 * Make sure that any allocated memory is freed before returning.
 * @return a status code
 */
int main(void) {
    setbuf(stdout, NULL); // Disable buffering for stdout to allow for real-time display updates

    char* filename = MAP_NAME; // The name of the map file to load

    int gameStatus; // Variable to hold the current status of the game (win/lose)
    char input = ' ', ghostStatus; // Variables for storing player input and ghost behavior

    // Attempt to load the map from the specified file
    map = load_map(filename, &height, &width);

    // Check for errors in map loading and return corresponding error codes
    if((int) map == ERR_NO_MAP) {
        return ERR_NO_MAP;
    }
    else if ((int) map == ERR_NO_GHOSTS) {
        return ERR_NO_GHOSTS;
    }
    else if ((int) map == ERR_NO_PACMAN) {
        return ERR_NO_PACMAN;
    }

    // Main game loop: continues until 'q' is pressed
    print_map(); // Display the initial map state
    while (input != 'q' ){
        input = getch(); // Read a character from the user without waiting for Enter

        // Move Pacman based on user input, potentially eating dots
        move_actor(&pacman_y, &pacman_x, input, EAT_DOTS);

        // Process ghost movement based on their vision of Pacman
        for (int i = 0; i < NUM_GHOSTS; i++) {
            ghostStatus = sees_pacman(pacman_y, pacman_x, ghosts_y[i], ghosts_x[i]);
            if (ghostStatus != SEES_NOTHING) {
                move_actor(&ghosts_y[i], &ghosts_x[i], ghostStatus, REPLACE_DOTS);
            }
        }

        // Re-display the map after movements
        print_map();

        // Check for win/lose conditions after each turn
        gameStatus = check_win(pacman_y, pacman_x, ghosts_y, ghosts_x);
        if (gameStatus == YOU_WIN) {
            printf("Congratulations! You win!\n");
            break; // Exit the game loop on win
        }

        gameStatus = check_loss(pacman_y, pacman_x, ghosts_y, ghosts_x);
        if (gameStatus == YOU_LOSE) {
            printf("Sorry, you lose.\n");
            break; // Exit the game loop on loss
        }
    }

    // Clean up allocated resources before exiting
    cleanup_game_resources();
    return NO_ERROR; // Return with a status code indicating no errors occurred
}
