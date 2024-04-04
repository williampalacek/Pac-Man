// Include necessary header files for definitions and access to external variables
#include "defines.h"
#include "ghost.h"

// External global variables for accessing the map and its dimensions
extern char *map, *dot_map;
extern int width, height;
int failed_attempts[NUM_GHOSTS] = {0};  // Initialize all to zero

/**
 * Determines if a ghost can see Pacman from its current position.
 * Ghosts have a line of sight in all four directions until a wall blocks their view.
 *
 * @param pacman_y Y-coordinate of Pacman
 * @param pacman_x X-coordinate of Pacman
 * @param ghost_y Y-coordinate of the ghostdaa
 * @param ghost_x X-coordinate of the ghost
 * @return char - Direction towards Pacman if seen, EATING_PACMAN if on same position, otherwise SEES_NOTHING
 */
char sees_pacman(int pacman_y, int pacman_x, int ghost_y, int ghost_x) {
    // Check if the ghost is on the same position as Pacman
    if (ghost_y == pacman_y && ghost_x == pacman_x) {
        return EATING_PACMAN; // Ghost catches Pacman
    }
    // Arrays to iterate through the four directions
    int dy[] = {-1, 1, 0, 0}; // Up, Down
    int dx[] = {0, 0, -1, 1}; // Left, Right
    char directions[] = {UP, DOWN, LEFT, RIGHT}; // Corresponding direction symbols

    // Check each direction from the ghost's position
    for (int i = 0; i < 4; i++) {
        int y = ghost_y + dy[i];
        int x = ghost_x + dx[i];
        // Continue in the direction until hitting a wall or the edge of the map
        while (x >= 0 && x < width && y >= 0 && y < height && map[y * width + x] != WALL) {
            if (y == pacman_y && x == pacman_x) {
                return directions[i]; // Pacman is visible in this direction
            }
            // Move further in the direction
            y += dy[i];
            x += dx[i];
        }
    }

    return SEES_NOTHING; // Pacman is not visible from the ghost's position
}
