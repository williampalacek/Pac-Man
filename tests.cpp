// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <stdio.h>

// make sure not to modify anything in this extern block
extern "C"{
#include "defines.h"
#include "map.h"
#include "ghost.h"
#include "game.h"
char *map = NULL, *dot_map = NULL;
int width, height;
extern int pacman_y, pacman_x;            // Pacman's position coordinates
extern int ghosts_y[NUM_GHOSTS], ghosts_x[NUM_GHOSTS]; // Arrays to store positions of ghosts
}

/**
 * This file is where you should put your tests for your code.
 * Your code must have tests that execute at least 85% of the code in
 * required functions for you to get full marks for the project.
 * Make sure to check out the course videos on automated testing for
 * more information about how to write tests.
 */

void setup(void) {
    map = load_map("map.txt", &height, &width);
    fill_initial_dot_map();
    REQUIRE(map != NULL); // Ensure map is loaded correctly for each test
}

void teardown(void) {
    free(map);
    free(dot_map);
    map = NULL;
    dot_map = NULL;
}

/* tests for map.c */
TEST_SUITE_BEGIN("Map tests");

// Combining load map and initial state checks to reduce redundancy
TEST_CASE("Test Load Map and Initial States") {
    setup();
    // Correct dimensions including walls and verifying key positions
    CHECK(width == 9);
    CHECK(height == 9);
    CHECK(map[4 * width + 4] == PACMAN); // Pacman's position
    CHECK(map[0 * width + 4] == WALL);
    CHECK(map[0 * width + 0] == GHOST); // Ghost's initial position
    CHECK(map[8 * width + 8] == GHOST); // Another ghost's position

    teardown();
}

TEST_CASE("File Opening Failure") {
    int map_height = 0, map_width = 0;
    char* result = load_map("non_existing_file.txt", &map_height, &map_width);
    CHECK(result == NULL);
}

// Optimized test for is_wall
TEST_CASE("Test Is Wall - Optimized") {
    setup();

    // Direct wall checks around a known internal wall
    CHECK(is_wall(0, 4) == YES_WALL); // An internal wall

    // Non-wall checks simplified
    CHECK(is_wall(4, 4) == NOT_WALL); // Open space and Pacman's position
    CHECK(is_wall(4, 3) == NOT_WALL);
    CHECK(is_wall(0, 0) == NOT_WALL); // Ghost's position

    teardown();
}

// Streamlined test for move_actor
TEST_CASE("Test Move Actor - Streamlined") {
    setup();

    pacman_y = 5;
    pacman_x = 5;
    dot_map[5 * width + 6] = DOT; // Ensure a dot to the right

    // Movement validation
    CHECK(move_actor(&pacman_y, &pacman_x, UP, EAT_DOTS) == MOVED_OKAY);
    CHECK(pacman_x == 5); // Moved right

    // Wall interaction and invalid direction
    CHECK(move_actor(&pacman_y, &pacman_x, UP, EAT_DOTS) == MOVED_WALL);
    CHECK(move_actor(&pacman_y, &pacman_x, 'X', EAT_DOTS) == MOVED_INVALID_DIRECTION);

    teardown();
}

TEST_CASE("Actor Moves Successfully - No Wall and Blocked by Wall Combined") {
    setup();

    int y = 1, x = 1;
    CHECK(move_actor(&y, &x, RIGHT, EAT_DOTS) == MOVED_WALL);
    CHECK(x == 1);

    dot_map[(y-1) * width + x] = WALL; // Adding a wall above for the next check
    CHECK(move_actor(&y, &x, UP, EAT_DOTS) == MOVED_OKAY);

    teardown();
}

TEST_CASE("Actor Eats Dot Optimized") {
    setup();

    int y = 1, x = 1;
    dot_map[y * width + x] = DOT; // Dot on Pacman
    dot_map[y * width + (x + 1)] = DOT; // Dot to the right
    CHECK(move_actor(&y, &x, RIGHT, EAT_DOTS) == MOVED_WALL);
    CHECK(dot_map[y * width + x] == DOT); // Dot eaten
    CHECK(move_actor(&y, &x, LEFT, EAT_DOTS) == MOVED_OKAY);
    CHECK(move_actor(&y, &x, UP, EAT_DOTS) == MOVED_OKAY);
    CHECK(move_actor(&y, &x, DOWN, EAT_DOTS) == MOVED_OKAY);


    teardown();
}

TEST_SUITE_END();

/* tests for ghost.c */
TEST_SUITE_BEGIN("Ghost tests");

// Optimized sees_pacman tests
TEST_CASE("Pacman Visibility to Ghosts - Combined") {
    setup();

    pacman_y = 4; pacman_x = 4;
    ghosts_y[0] = 4; ghosts_x[0] = 3; // Left of Pacman
    CHECK(sees_pacman(pacman_y, pacman_x, ghosts_y[0], ghosts_x[0]) == RIGHT);

    teardown();
}

TEST_CASE("Pacman Blocked by Wall" * doctest::description("Ghosts cannot see Pacman if view is blocked by a wall")) {
    setup();
    // Assuming a wall is placed directly between Pacman and the ghost
    map[5 * width + 4] = WALL; // Wall between ghost and Pacman
    pacman_x = 5; pacman_y = 5;
    ghosts_x[0] = 5; ghosts_y[0] = 3; // Ghost's position above Pacman with a wall in between

    CHECK(sees_pacman(pacman_y, pacman_x, ghosts_y[0], ghosts_x[0]) == SEES_NOTHING); // Wall blocks the view

    teardown();
}


TEST_SUITE_END();

/* tests for game.c */
TEST_SUITE_BEGIN("Game tests");

// Condensed win and loss checks
TEST_CASE("Game Outcome Scenarios Combined") {
    setup();

    // Scenario 1: All dots collected, Pacman wins
    for (int i = 0; i < width * height; i++) {
        dot_map[i] = EMPTY;
    }
    pacman_y = 4; pacman_x = 4;
    ghosts_y[0] = 2; ghosts_x[0] = 2; // Ghost positioned away from Pacman
    CHECK(check_win(pacman_y, pacman_x, ghosts_y, ghosts_x) == YOU_WIN);

    // Scenario 2: Pacman caught by a ghost, results in loss
    ghosts_x[0] = pacman_x; ghosts_y[0] = pacman_y; // Position ghost on Pacman
    CHECK(check_loss(pacman_y, pacman_x, ghosts_y, ghosts_x) == YOU_LOSE);

    // Scenario 3: Ghost near but not on Pacman, game continues
    ghosts_x[0] = pacman_x + 1; ghosts_y[0] = pacman_y; // Position ghost right next to Pacman
    CHECK(check_loss(pacman_y, pacman_x, ghosts_y, ghosts_x) == KEEP_GOING);

    // Scenario 4: Multiple ghosts, one on Pacman, results in loss
    ghosts_y[1] = pacman_y; ghosts_x[1] = pacman_x; // Another ghost on Pacman
    CHECK(check_loss(pacman_y, pacman_x, ghosts_y, ghosts_x) == YOU_LOSE);


    teardown();
}

TEST_CASE("Not All Dots Collected") {
    setup();

    // Leave at least one dot
    dot_map[2 * width + 2] = DOT;

    CHECK(check_win(pacman_y, pacman_x, ghosts_y, ghosts_x) == KEEP_GOING);

    teardown();
}


TEST_SUITE_END();

/* Simplified tests for non-required functions */
TEST_SUITE_BEGIN("Non-required functions tests");

TEST_CASE("Map and Resource Management Optimized") {
    // Setup and validate map update and cleanup
    setup();
    pacman_x = 2; pacman_y = 2;
    int ghosts_y[NUM_GHOSTS] = {1, 3}, ghosts_x[NUM_GHOSTS] = {1, 3};
    CHECK(update_map() == NO_ERROR);

    cleanup_game_resources();
    CHECK(map == NULL && dot_map == NULL);
}

TEST_SUITE_END();
