// APSC 142 Engineering Programming Project Starter Code
// Copyright Sean Kauffman 2024

#include <stdlib.h>
#include "defines.h"
#include "colours.h"
#include "map.h"
#include <stdio.h>

extern char *map, *dot_map;
extern int width, height, pacman_y, pacman_x, ghosts_y[NUM_GHOSTS], ghosts_x[NUM_GHOSTS];


int move_actor(int * y, int * x, char direction, int eat_dots) {
    int newY = *y, newX = *x;
    int index = *y * width + *x;

// Determine the new position based on the direction
    switch (direction) {
        case UP:
            newY -= 1;
            break;
        case DOWN:
            newY += 1;
            break;
        case LEFT:
            newX -= 1;
            break;
        case RIGHT:
            newX += 1;
            break;
        default:
            return MOVED_INVALID_DIRECTION;
    }
    // Check if the new position is a wall
    if (is_wall(newY, newX) == YES_WALL) {
        return MOVED_WALL;
    }

    if (eat_dots == EAT_DOTS) {
        map[index] = EMPTY; // Leave an empty space where the actor was
        dot_map[index] = EMPTY;
    } else if (eat_dots == REPLACE_DOTS) {
        if (dot_map[index] == DOT) {
            // If it does contain a DOT, then replicate that DOT in the main map at the same position
            map[index] = DOT;
        } else {
            // If it does not contain a DOT, set the position in the main map to EMPTY
            map[index] = EMPTY;
            dot_map[index] = EMPTY;
        }
    }

    *y = newY;
    *x = newX;

    return MOVED_OKAY;
}

int update_map(void){
    // Copy the dot_map to the main map, resetting previous positions
    for(int i = 0; i < width * height; i++) {
        map[i] = dot_map[i];
    }

    // Place Pacman on the map
    int pacman_index = pacman_y * width + pacman_x;
    map[pacman_index] = PACMAN;

    // Place each ghost on the map
    for (int i = 0; i < NUM_GHOSTS; i++) {
        int ghost_index = ghosts_y[i] * width + ghosts_x[i];
        map[ghost_index] = GHOST;
    }

    return NO_ERROR;
}

int print_map(void) {
    update_map();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char mapChar = map[y * width + x];

            // Apply color based on the character
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

int is_wall(int y, int x) {
    // Ensure the coordinates are within the bounds of the map
    if (y < 0 || y >= height || x < 0 || x >= width) {
        return YES_WALL; // Out of bounds can be considered a wall or return an error; depends on game logic
    }

    int index = y * width + x; // Calculate the index in the flat array

    // Check if the character at the index is a wall ('W')
    if (map[index] == WALL) {
        return YES_WALL;
    } else {
        return NOT_WALL;
    }
}

int find_initial_positions(void) {
    int ghost_count = 0;
    pacman_x = pacman_y = 0;

    for(int i = 0; i < NUM_GHOSTS; i++) {
        ghosts_y[i] = 0;
        ghosts_x[i] = 0;
    }


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
    if(pacman_x == 0 || pacman_y == 0){
        return ERR_NO_PACMAN;
    }
    else if(ghost_count == 0){
        return ERR_NO_GHOSTS;
    }
    else{
        return NO_ERROR;
    }
}

char * load_map(char *filename, int *map_height, int *map_width) {
    *map_height = height = 1;
    *map_width = width = 0;

    int mapIndex, elementCount = 0;
    char ch, *tempMap = NULL, *tempDotMap = NULL;

    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open the file.\n");
        return (char *) ERR_NO_MAP;
    }

    //printf("Determining map dimensions...\n");

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

    //printf("Adjusted map dimensions with walls - Width: %d, Height: %d\n", width, height);

    tempMap = (char*)malloc(width * height * sizeof(char));
    tempDotMap = (char*)malloc(width * height * sizeof(char));

    if (!tempMap || !tempDotMap) {
        printf("Error: Memory allocation failed.\n");
        free(tempMap); // Ensure to free allocated memory to avoid leaks
        free(tempDotMap);
        fclose(file);
        return NULL;
    }

// Initialize dot map with walls
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

    // Assigning the populated maps to the global pointers
    map = tempMap;
    dot_map = tempDotMap;
    *map_height = height;
    *map_width = width;

    find_initial_positions();

    //printf("Map loaded successfully with walls. Height: %d, Width: %d\n", *map_height, *map_width);

    return map;
}
