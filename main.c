/*
    Command Line Ray Casting Engine (FPS)
    
    BEFORE RUNNING
    ~~~~~~~~~~~~~~
    This program expects the console dimensions to be set to 120 Columns by 30 Rows. 
    I recommend an usual font "Consolas" at size 16. You can do this
    by running the program, right clicking on the console title bar, and specifying 
    the properties. I forced them in program code by using the Windows API.

    To compile the project:
            In command line:
                > gcc -o main main.c -lncurses
                > main

    Controls: 'A' = Turn Left / 'D' = Turn Right / 'W' = Walk Forwards / 'S' = Walk Backwards / 'Esc' = Exit 
*/

#define _USE_MATH_DEFINES   // Math Macro Definition 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <curses.h>         // Comfortable Input / Output with Terminal
#include <windows.h>        // Windows API

#define screenWidth 120             // Console Screen Size X (Columns)
#define screenHeight 30             // Console Screen Size Y (Rows)
#define mapWidth 16                 // World Size X (Columns)
#define mapHeight 16                // World Size Y (Rows)
char map[mapHeight][mapWidth] = {   // Create Map of the World: '#' = wall / '.' = space    
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
float frameTime = 0.0f;             // Time of One Frame

float playerX = 1.0f;			// Player Start Position X (Columns)
float playerY = 1.00f;          // Player Start Position Y (Columns)
float playerDir = 0.0f;         // Players Dirrection
float playerSpeed = 5.0f;		// Walking Speed
float playerFOV = M_PI / 3;     // Players Field of View
float playersMaxDepth = 30.0f;  // Maximum Viewing Distance

char gradient[10] = " .:~=+*#%@";

void setWindowSize()
{
    HANDLE handleConsole = GetStdHandle(STD_OUTPUT_HANDLE);             // Получаем дескриптор текущего окна консоли

    COORD bufferSize = {screenWidth, screenHeight};                     // Устанавливаем размер буфера экрана
    SetConsoleScreenBufferSize(handleConsole, bufferSize);              // (необходимо, чтобы размер окна не превышал размер буфера)

    SMALL_RECT windowSize = {0, 0, screenWidth - 1, screenHeight - 1};  // Создаем структуру с информацией о размерах и позиции окна консоли

    SetConsoleWindowInfo(handleConsole, TRUE, &windowSize);             // Устанавливаем размер и позицию окна консоли

    HWND consoleWindow = GetConsoleWindow();                            // Получаем дескриптор текущего окна консоли для изменения его стилей

    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);               // Получаем текущие стили окна

    style &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);                            // Убираем стили, позволяющие изменять размер окна

    SetWindowLong(consoleWindow, GWL_STYLE, style);                     // Устанавливаем модифицированные стили обратно
}

void checkKeyState()
{
    if (GetAsyncKeyState((unsigned short)'A') & 0x8000)             // Обработка поворота влево
    {
        playerDir -= (playerSpeed * 0.75f) * frameTime;
    }

    if (GetAsyncKeyState((unsigned short)'D') & 0x8000)             // Обработка поворота вправо 
    {
        playerDir += (playerSpeed * 0.75f) * frameTime;
    }

    if (GetAsyncKeyState((unsigned short)'W') & 0x8000)             // Обработка движения вперед 
    {
	    playerX += sinf(playerDir) * playerSpeed * frameTime;
	    playerY += cosf(playerDir) * playerSpeed * frameTime;

	    if (map[(int)playerY][(int)playerX] == '#')                 // Обработка столкновения со стеной 
	    {
		    playerX -= sinf(playerDir) * playerSpeed * frameTime;
		    playerY -= cosf(playerDir) * playerSpeed * frameTime;
	    }			
    }
        
    if (GetAsyncKeyState((unsigned short)'S') & 0x8000)             // Обработка движения назад 
    {
	    playerX -= sinf(playerDir) * playerSpeed * frameTime;
	    playerY -= cosf(playerDir) * playerSpeed * frameTime;

	    if (map[(int)playerY][(int)playerX] == '#')                 // Обработка столкновения со стеной 
	    {
		    playerX += sinf(playerDir) * playerSpeed * frameTime;
		    playerY += cosf(playerDir) * playerSpeed * frameTime;
	    }			
    }

    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)                       // Обработка выхода из программы
    {
        exit(1);
    }
}

void displayStats()
{
    mvprintw(0,0,"x = %3.2f, y = %3.2f, direction = %3.2f, fps = %3.2f",playerX, playerY, playerDir, 1.0f / frameTime);

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

}

int main() 
{
    setWindowSize();                        // Initialization of Window Size

    clock_t timeBefore, timeAfter;          // Инициализируем временные пременные 
    timeBefore = clock();                   // Запоминаем текущее время 
    timeAfter = clock();

    map[(int)playerY][(int)playerX] = 'P';  // Инициализация игрока на карте

    initscr();                              // Инициализируем режим curses
    cbreak();                               // Отключаем буферизацию строк
    noecho();                               // Не отображаем вводимые символы
    nodelay(stdscr, TRUE);                  // Включаем неблокирующий режим для getch()
    keypad(stdscr, TRUE);                   // Получаем специальные клавиши, такие как стрелки

    while(1)    // Game cycle
    {
        timeAfter = clock();                                            // Обновляем время конца кадра
        frameTime = (float)(timeAfter - timeBefore) / CLOCKS_PER_SEC;   // Подсчет времени кадра
        timeBefore = timeAfter;                                         // Обновляем время начала кадра
        
        map[(int)playerY][(int)playerX] = '.';  // Когда игрок начинает движение, он удаляется с карты

        checkKeyState();                        // Check the Players Movement Depends on Key  

        map[(int)playerY][(int)playerX] = 'P';  // Когда игрок закончил движение, он добавляется на карту по новым координатам

        displayStats();                         // Display of Statistics and Minicart
    
        refresh();                              // Обновляем кадр

    }

    endwin();   // Выходим из curses

    return 0;
}
