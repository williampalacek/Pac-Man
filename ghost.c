// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

#include "defines.h"
#include "ghost.h"

extern char *map, *dot_map;
extern int width, height;

char sees_pacman(int pacman_y, int pacman_x, int ghost_y, int ghost_x) {
    if (ghost_y == pacman_y && ghost_x == pacman_x) {
        return EATING_PACMAN;
    }
    // Check in all four directions
    int dy[] = {-1, 1, 0, 0}; // Up, Down
    int dx[] = {0, 0, -1, 1}; // Left, Right
    char directions[] = {UP, DOWN, LEFT, RIGHT};

    for (int i = 0; i < 4; i++) {
        int y = ghost_y + dy[i];
        int x = ghost_x + dx[i];
        while (x >= 0 && x < width && y >= 0 && y < height && map[y * width + x] != WALL) {
            if (y == pacman_y && x == pacman_x) {
                return directions[i];
            }
            y += dy[i];
            x += dx[i];
        }
    }

    return SEES_NOTHING;
}
