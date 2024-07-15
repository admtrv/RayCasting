/*
    Command Line RayCasting Visualization
*/

#define _USE_MATH_DEFINES           // Specify math constants 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <curses.h>                 // Library for comfortable i/o using the terminal

#define ESC 27					    // ESC key code
#define W 119					    // W key code
#define A 97					    // A key code
#define S 115					    // S key code
#define D 100					    // D key code
#define screenWidth 120             // Console screen size X (columns)
#define screenHeight 30             // Console screen size Y (rows)
#define mapWidth 16                 // World size X (columns)
#define mapHeight 16                // World size Y (rows)
char map[mapHeight][mapWidth] = {   // Create map of the world: '#' = wall / '.' = space    
    {"################"},
    {"#..#...........#"},
    {"#..#....########"},
    {"#..#...........#"},
    {"#..#...#.......#"},
    {"#......#.......#"},
    {"#..............#"},
    {"###............#"},
    {"#..............#"},
    {"#......####..###"},
    {"#......#.......#"},
    {"#......#.......#"},
    {"#..............#"},
    {"#.....########.#"},
    {"#..............#"},
    {"################"}
};
float frameTime = 0.0f;             // Time of one frame

float playerX = 5.0f;               // Player start position coordinate X 
float playerY = 14.0f;              // Player start position coordinate Y 
float playerDir = M_PI;             // Players dirrection
float playerSpeed = 5.0f;		    // Walking speed
float playerFOV = M_PI / 3.0f;      // Players field of view
float playersMaxDepth = 13.0f;      // Maximum viewing distance

#define gradientSize 10                         // Size of gradient range
char gradient1[gradientSize]="@%#+=*:-. ";      // Gradient of ASCII symbols for wall
char gradient2[gradientSize]="&Oi?+~>:. ";      // Another gradient of ASCII symbols 

void checkKeyState()
{
    int key = getch();

    switch (key)
    {
        case W: case KEY_UP:                                            // Processing of forward movement
            playerX += sinf(playerDir) * playerSpeed * frameTime;
            playerY += cosf(playerDir) * playerSpeed * frameTime;

            if (map[(int)playerY][(int)playerX] == '#')                 // Processing wall collision
            {
                playerX -= sinf(playerDir) * playerSpeed * frameTime;
                playerY -= cosf(playerDir) * playerSpeed * frameTime;
            }
            break;

        case S: case KEY_DOWN:                                          // Processing of backward movement
            playerX -= sinf(playerDir) * playerSpeed * frameTime;
            playerY -= cosf(playerDir) * playerSpeed * frameTime;

            if (map[(int)playerY][(int)playerX] == '#')                 // Processing wall collision 
            {
                playerX += sinf(playerDir) * playerSpeed * frameTime;
                playerY += cosf(playerDir) * playerSpeed * frameTime;
            }
            break;

        case A: case KEY_LEFT:                                          // Processing a left turn
            playerDir += (playerSpeed * 0.75f) * frameTime;
            break;

        case D: case KEY_RIGHT:                                         // Processing a right turn
            playerDir -= (playerSpeed * 0.75f) * frameTime;
            break;

        case ESC:                                                       // Processing of program exit
            clear();                                                    // Clear the console
            refresh();                                                  // Refresh the screen
            endwin();                                                   // Exiting ncurses mode
            exit(1);
            break;

        default:
            break;
    }
}

void displayStats()
{
    mvprintw(0,0,"x = %3.2f, y = %3.2f, direction = %3.2f, fps = %d ",playerX, playerY, playerDir, (int) (1.0f / frameTime));

    for (int i = 0; i < mapHeight; i++)
    {
        for (int j = 0; j < mapWidth; j++)
        {
            mvprintw(screenHeight - mapHeight + i,j,"%c",map[i][j]);
        }
    }
}

void renderFrame()
{
    for (int x = 0; x < screenWidth; x++)
    {
        float rayDir = (playerDir + playerFOV / 2.0f) - ((float)x / (float)screenWidth) * playerFOV;            // Direction of each iteration ray
        float wallDistance = 0.0f;                                                                              // Distance to obstacle in ray direction
        
        int hitWallFlag = 0;                                                                                    // Flag, did the ray reach the wall
        int hitEdgeFlag = 0;                                                                                    // Flag, did the ray hit the edge of the wall

        float rayX = sinf(rayDir);                                                                              // Coordinates of the ray direction by X
        float rayY = cosf(rayDir);                                                                              // Coordinates of the ray direction by Y
 
        while (!hitWallFlag && wallDistance < playersMaxDepth)                                                  // As long as we don't collide with a wall or go out of visual depth
        {
            wallDistance += 0.1f;                                                                               // Slowly moving in the direction of the ray
 
            int distanceX = (int)(playerX + rayX * wallDistance);                                               // Coordinates of the ray end by X
            int distanceY = (int)(playerY + rayY * wallDistance);                                               // Coordinates of the ray end by Y
 
            if (distanceX < 0 || distanceX >= mapWidth || distanceY < 0 || distanceY >= mapHeight)              // If we're off the map, there's no point in looking further
            { 
                hitWallFlag = 1;                                                                                // Fixing the collision within vision depth
                wallDistance = playersMaxDepth;
            }
            else if ( map[distanceY][distanceX] == '#')                                                         // If we hit a wall
            { 
                hitWallFlag = 1;                                                                                // Fixing the collision at a distance from the wall

                float edgeDistance[4]; 
                float dotProduct[4];

                for (int i = 0; i < 4; i++)                                                                     // Calculate the distance and dot product for each edge
                {
                    int edgeX = i % 2;
                    int edgeY = i / 2;

                    float vectorX = (float)distanceX + edgeX - playerX;
                    float vectorY = (float)distanceY + edgeY - playerY;

                    edgeDistance[i] = sqrtf(vectorX * vectorX + vectorY * vectorY);
                    dotProduct[i] = (rayX * vectorX / edgeDistance[i]) + (rayY * vectorY / edgeDistance[i]);
                }
                
                for (int i = 0; i < 4; i++)                                                                     // Sorted in order from nearest to farthest
                {
                    for (int j = i + 1; j < 4; j++) 
                    {
                        if (edgeDistance[j] < edgeDistance[i]) 
                        {
                            float temp = edgeDistance[j];
                            edgeDistance[j] = edgeDistance[i];
                            edgeDistance[i] = temp;

                            temp = dotProduct[j];
                            dotProduct[j] = dotProduct[i];
                            dotProduct[i] = temp;
                        }
                    }
                }

                float proximity = 0.004f;                                                                       // Threshold for edge proximity detection

                for (int i = 0; i < 2; i++)                                                                     // Checking the first three/two edges for proximity, we never can't see all four
                {
                    if (acosf(dotProduct[i]) < proximity) 
                    {
                        hitEdgeFlag = 1;
                        break;
                    }
                }
            }
        }

        int ceilingHeight = (float)(screenHeight/2.0) - screenHeight / ((float)wallDistance);                   // Ceiling height
        int floorHeight = (float)(screenHeight/2.0) + screenHeight / ((float)wallDistance);                     // Floor height

        for (int y = 0; y < screenHeight; y++)
        {
            if (y < ceilingHeight)                                                                              // Rendering of the ceiling
            {
                float d = 1.0f + ((float)y - screenHeight / 2.0) / ((float)screenHeight / 2.0);
                int gradientIndex = (int)(d * gradientSize);
                if (gradientIndex < 0) gradientIndex = 0;
                if (gradientIndex >= gradientSize) gradientIndex = gradientSize - 1;

                char ceilingShader = gradient2[gradientIndex];

                attron(COLOR_PAIR(2)); 
                mvprintw(y,x,"%c",ceilingShader);
                attroff(COLOR_PAIR(2));
            } 
            else if (y >= ceilingHeight && y <= floorHeight)                                                    // Rendering of walls
            {
                int gradientIndex = (int)((wallDistance / playersMaxDepth) * gradientSize);
                if (gradientIndex < 0) gradientIndex = 0;
                if (gradientIndex >= gradientSize) gradientIndex = gradientSize - 1;

                char wallShader = gradient1[gradientIndex];
                if (hitEdgeFlag) wallShader = ' ';

                attron(COLOR_PAIR(1));
                mvprintw(y,x,"%c",wallShader);
                attroff(COLOR_PAIR(1));
            } 
            else                                                                                                // Rendering of the floor
            {
                float d = 1.0f - ((float)y - screenHeight / 2.0) / ((float)screenHeight / 2.0);
                int gradientIndex = (int)(d * gradientSize);
                if (gradientIndex < 0) gradientIndex = 0;
                if (gradientIndex >= gradientSize) gradientIndex = gradientSize - 1;

                char floorShader = gradient2[gradientIndex];
                
                attron(COLOR_PAIR(3));
                mvprintw(y,x,"%c",floorShader);
                attroff(COLOR_PAIR(3));
            } 
        }
    }
}

int main() 
{
    clock_t timeBefore, timeAfter;              // Initialization of time variables 
    timeBefore = clock();                       // Storing the current time
    timeAfter = clock();

    map[(int)playerY][(int)playerX] = 'o';      // Initialization of a player on the map

    initscr();                                  // Initialization of ncurses mode
    cbreak();                                   // Disable string buffering
    noecho();                                   // Do not display input characters
    nodelay(stdscr, TRUE);                      // Enable non-blocking input
    start_color();                              // Initialization of the color mode
    init_pair(1, COLOR_RED, COLOR_BLACK);       // For the walls - red
    init_pair(2, COLOR_BLUE, COLOR_BLACK);      // For the ceiling - blue
    init_pair(3, COLOR_BLACK + 8, COLOR_BLACK); // For the floor - gray

    while(1)    // Game cycle
    {
        timeAfter = clock();                                            // Updating the end of frame time
        frameTime = (float)(timeAfter - timeBefore) / CLOCKS_PER_SEC;   // Frame time calculation
        timeBefore = timeAfter;                                         // Updating the frame start time
        
        map[(int)playerY][(int)playerX] = '.';                          // When a player starts a move, he is removed from the map

        checkKeyState();                                                // Check the players movement depends on key  

        map[(int)playerY][(int)playerX] = 'o';                          // When a player has finished moving, he is added to the map at the new coordinates

        renderFrame();                                                  // Rendering a new frame depends on new coordinates

        displayStats();                                                 // Display stats and mini-map on the screen

        refresh();                                                      // Updating the frame
    }

    endwin();   // Exiting ncurses mode

    return 0;
}

