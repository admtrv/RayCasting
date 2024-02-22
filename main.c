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
                > gcc -o main main.c -lpdcurses
                > main

    Controls: 'A' = Turn Left / 'D' = Turn Right / 'W' = Walk Forwards / 'S' = Walk Backwards / 'Esc' = Exit 
*/

#define _USE_MATH_DEFINES           // Specify Math Constants 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <curses.h>                 // Library for Сomfortable I/O using the Terminal
#include <windows.h>                // Windows API

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

float playerX = 5.0f;               // Player Start Position coordinate X 
float playerY = 14.0f;              // Player Start Position coordinate Y 
float playerDir = M_PI;             // Players Dirrection
float playerSpeed = 5.0f;		    // Walking Speed
float playerFOV = M_PI / 3.0f;      // Players Field of View
float playersMaxDepth = 12.0f;      // Maximum Viewing Distance

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
        playerDir += (playerSpeed * 0.75f) * frameTime;
    }

    if (GetAsyncKeyState((unsigned short)'D') & 0x8000)             // Обработка поворота вправо 
    {
        playerDir -= (playerSpeed * 0.75f) * frameTime;
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
        system("cls");
        exit(1);
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
        float rayDir = (playerDir + playerFOV / 2.0f) - ((float)x / (float)screenWidth) * playerFOV;    // Направление каждого луча итерации
        float wallDistance = 0.0f;                                                                      // Расстояние до препятствия в направлении луча
        int hitWallFlag = 0;                                                                            //   Флаг, достигнул ли луч стену

        float rayX = sinf(rayDir);                                                                      // Координаты вектора луча по Х
        float rayY = cosf(rayDir);                                                                      // Координаты вектора луча по У
 
        while (!hitWallFlag && wallDistance < playersMaxDepth)          // Пока не столкнулись со стеной или не вышли за радиус видимости
        {
            wallDistance += 0.1f;                                       // Постепенно идем в направлении луча
 
            int tX = (int)(playerX + rayX * wallDistance);              // Координаты конца луча по X
            int tY = (int)(playerY + rayY * wallDistance);              // Координаты конца луча по У
 
            if (tX < 0 || tX >= mapWidth || tY < 0 || tY >= mapHeight)  // Если мы вышли за карту, то дальше смотреть нет смысла
            { 
                hitWallFlag = 1;                                        // Фиксируем удар на расстоянии видимости
                wallDistance = playersMaxDepth;
            }
            else if ( map[tY][tX] == '#')                               // Если встретили стену
            { 
                hitWallFlag = 1;                                        // Фиксируем удар на расстоянии до стены
            }
        }

        int ceilingHeight = (float)(screenHeight/2.0) - screenHeight / ((float)wallDistance);   // Высота потолка
        int floorHeight = (float)(screenHeight/2.0) + screenHeight / ((float)wallDistance);     // Высота пола

        for (int y = 0; y < screenHeight; y++)
        {
            if (y <= ceilingHeight)
            {
                float d = 1.0f + ((float)y - screenHeight / 2.0) / ((float)screenHeight / 2.0);
                int randomNumber = rand() % 2;

                char ceilingShader = ' ';
                if (d < 0.25f)          {ceilingShader = (randomNumber == 0?'x':'&');}
                else if (d < 0.5f)      {ceilingShader = (randomNumber == 0?'+':'*');}
                else if (d < 0.75f)     {ceilingShader = (randomNumber == 0?'~':',');}
                else if (d < 0.9f)      {ceilingShader = (randomNumber == 0?'.':'-');}
                else                    {ceilingShader = ' ';}

                attron(COLOR_PAIR(2)); 
                mvprintw(y,x,"%c",ceilingShader);
                attroff(COLOR_PAIR(2));
            } 
            else if (y > ceilingHeight && y <= floorHeight)
            {
                int randomNumber = rand() % 2;

                char wallShader = ' ';
                if (wallDistance <= playersMaxDepth / 4.0f)			{wallShader = (randomNumber == 0?'@':'%');}	
                else if (wallDistance < playersMaxDepth / 3.0f)     {wallShader = (randomNumber == 0?'&':'*');}
                else if (wallDistance < playersMaxDepth / 2.0f)		{wallShader = (randomNumber == 0?'x':'=');}
                else if (wallDistance < playersMaxDepth)			{wallShader = (randomNumber == 0?'~':':');}
                else											    {wallShader = ' ';}
                
                attron(COLOR_PAIR(1));
                mvprintw(y,x,"%c",wallShader);
                attroff(COLOR_PAIR(1));
            } 
            else
            {
                float d = 1.0f - ((float)y - screenHeight / 2.0) / ((float)screenHeight / 2.0);
                int randomNumber = rand() % 2;

                char floorShader = ' ';
                if (d < 0.25f)          {floorShader = (randomNumber == 0?'x':'&');}
                else if (d < 0.5f)      {floorShader = (randomNumber == 0?'+':'*');}
                else if (d < 0.75f)     {floorShader = (randomNumber == 0?'~':',');}
                else if (d < 0.9f)      {floorShader = (randomNumber == 0?'.':'-');}
                else                    {floorShader = ' ';}
                
                attron(COLOR_PAIR(3));
                mvprintw(y,x,"%c",floorShader);
                attroff(COLOR_PAIR(3));
            } 
        }
    }
}

int main() 
{
    setWindowSize();                            // Initialization of Window Size

    srand(time(NULL));                          // Инициализируем генератор случайных чисел
    clock_t timeBefore, timeAfter;              // Инициализируем временные пременные 
    timeBefore = clock();                       // Запоминаем текущее время 
    timeAfter = clock();

    map[(int)playerY][(int)playerX] = 'P';      // Инициализация игрока на карте

    initscr();                                  // Инициализируем режим curses
    cbreak();                                   // Отключаем буферизацию строк
    noecho();                                   // Не отображаем вводимые символы
    nodelay(stdscr, TRUE);                      // Включаем неблокирующий режим для getch()
    keypad(stdscr, TRUE);                       // Получаем специальные клавиши, такие как стрелки
    start_color();                              // Инициализация цветового режима
    init_pair(1, COLOR_RED, COLOR_BLACK);       // Для стен - красный
    init_pair(2, COLOR_BLUE, COLOR_BLACK);      // Для потолка - синий
    init_pair(3, COLOR_BLACK + 8, COLOR_BLACK); // Для пола - серый

    while(1)    // Game cycle
    {
        timeAfter = clock();                                            // Обновляем время конца кадра
        frameTime = (float)(timeAfter - timeBefore) / CLOCKS_PER_SEC;   // Подсчет времени кадра
        timeBefore = timeAfter;                                         // Обновляем время начала кадра
        
        map[(int)playerY][(int)playerX] = '.';                          // Когда игрок начинает движение, он удаляется с карты

        checkKeyState();                                                // Check the Players Movement depends on Key  

        map[(int)playerY][(int)playerX] = 'P';                          // Когда игрок закончил движение, он добавляется на карту по новым координатам

        renderFrame();                                                  // Render a New Frame depends on New Coordinates

        displayStats();                                                 // Display Stats and Mini-map on the Screen

        refresh();                                                      // Обновляем кадр
    }

    endwin();   // Выходим из curses

    return 0;
}