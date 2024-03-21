// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

#include "defines.h"
#include "game.h"
#include <stdlib.h>

extern char * map, * dot_map;
extern int height, width;

int check_win(int pacman_y, int pacman_x, int ghosts_y[NUM_GHOSTS], int ghosts_x[NUM_GHOSTS]) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x; // Calculate index for flat array access
            if (dot_map[index] == DOT) {
                return KEEP_GOING; // A dot is found, game continues
            }
        }
    }
    return YOU_WIN; // No dots left, Pacman wins
}

int check_loss(int pacman_y, int pacman_x, int ghosts_y[NUM_GHOSTS], int ghosts_x[NUM_GHOSTS]) {
    for (int i = 0; i < NUM_GHOSTS; i++) {
        if (pacman_y == ghosts_y[i] && pacman_x == ghosts_x[i]) {
            return YOU_LOSE; // Pacman and a ghost occupy the same position.
        }
    }
    return KEEP_GOING; // No ghost is at the same position as Pacman.
}

void cleanup_game_resources(void) {
    if (map != NULL) {
        free(map);
        map = NULL;
    }
    if (dot_map != NULL) {
        free(dot_map);
        dot_map = NULL;
    }
}

