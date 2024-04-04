// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

// Reminder to update your project configuration to use "Emulate terminal in the output console"

// Including standard and necessary libraries
#include <stdio.h>   // For input/output functions
#include <stdlib.h>  // For general utilities like memory allocation, program control, etc.
#include <time.h>    // For dealing with time, necessary for random number generation

// Including custom header files for additional functionality and organization
#include "colours.h"  // Contains functions for changing text color and character input handling
#include "defines.h"  // Contains definitions for improved code readability
#include "map.h"      // Function prototypes for map-related operations
#include "game.h"     // Function prototypes for game logic
#include "ghost.h"    // Function prototypes for ghost behavior

// Declaration of global variables to store game state information
char *map = NULL, *dot_map = NULL; // Pointers to dynamic arrays for the game map and dot tracking
int width, height;                 // Variables to store map dimensions

extern int pacman_y, pacman_x;            // Variables to store Pacman's position
extern int ghosts_y[NUM_GHOSTS], ghosts_x[NUM_GHOSTS]; // Arrays to store the positions of multiple ghosts

/**
 * The main function acts as the entry point of the program.
 *
 * It initializes game resources, handles the main game loop, and cleans up resources upon exit.
 * The function returns different status codes based on the game's end condition or errors encountered.
 *
 * Status codes:
 *   NO_ERROR - Indicates the game exited without errors.
 *   ERR_NO_MAP - Returned if the map file could not be found.
 *   ERR_NO_PACMAN - Returned if Pacman could not be found on the map.
 *   ERR_NO_GHOSTS - Returned if less than the required number of ghosts are found.
 *
 * Before exiting, the function ensures all dynamically allocated memory is freed.
 */
int main(void) {
    setbuf(stdout, NULL);
    srand(time(NULL)); // Seeds the random number generator with current time for unpredictability

    char* filename = MAP_NAME; // Assigns the map file name to a variable

    int gameStatus, counter = 0; // Variable to track the game's win/lose status
    char input = ' ', ghostStatus; // Variables for player input and ghost AI decision-making

    // Loading the game map from a file
    map = load_map(filename, &height, &width);

    // Handling possible errors during map loading
    if((int) map == NULL) {
        return ERR_NO_MAP; // Map file not found
    }
    else if ((int) map == ERR_NO_GHOSTS) {
        return ERR_NO_GHOSTS; // Not enough ghosts on the map
    }
    else if ((int) map == ERR_NO_PACMAN) {
        return ERR_NO_PACMAN; // Pacman not found on the map
    }

    // New call to initialize dot_map based on the loaded map
    fill_initial_dot_map();

    // Main game loop starts, continues until 'q' is pressed
    print_map(); // Initial display of the game map
    while (input != 'q' ){
        input = getch(); // Reads user input without requiring Enter

        // Moving Pacman based on the player's input
        move_actor(&pacman_y, &pacman_x, input, EAT_DOTS);

        // Processing each ghost's movement based on their AI
        for (int i = 0; i < NUM_GHOSTS; i++) {
            ghostStatus = sees_pacman(pacman_y, pacman_x, ghosts_y[i], ghosts_x[i]);
            if (ghostStatus == SEES_NOTHING) {
                // Random movement logic when Pacman is not visible
                char possibleDirections[] = {UP, DOWN, LEFT, RIGHT};
                int randomIndex;
                char direction;
                int moveResult;
                do {
                    randomIndex = rand() % 4; // Select a random direction
                    direction = possibleDirections[randomIndex];
                    moveResult = move_actor(&ghosts_y[i], &ghosts_x[i], direction, REPLACE_DOTS);
                    if(counter >= 10){
                        break;
                    }
                    counter++;
                } while (moveResult == MOVED_WALL); // Ensure the selected direction is not a wall
                counter = 0;
            } else if (ghostStatus != EATING_PACMAN) {
                move_actor(&ghosts_y[i], &ghosts_x[i], ghostStatus, REPLACE_DOTS);
            }
        }

        // Update the display after each turn
        print_map();

        // Checking for loss condition
        gameStatus = check_loss(pacman_y, pacman_x, ghosts_y, ghosts_x);
        if (gameStatus == YOU_LOSE) {
            printf("Sorry, you lose.\n");
            break; // Game ends on loss
        }

        // Checking for win condition
        gameStatus = check_win(pacman_y, pacman_x, ghosts_y, ghosts_x);
        if (gameStatus == YOU_WIN) {
            printf("Congratulations! You win!\n");
            break; // Game ends on win
        }
    }

    // Cleanup before program exit
    cleanup_game_resources();
    return NO_ERROR; // Indicating the game concluded without any errors
}