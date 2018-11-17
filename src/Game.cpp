/*                      *
 *                      *
 *        Swave         *
 *                      *
 *       Creators       *
 *     Erik Mattfolk    *
 *       Annie Wång     *
 *      Victor Lells    *
 *                      *
 */                    

#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "time.h"
#include <iostream>
#include <vector>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, TITLE = 1, GAMEPLAY = 2, ENDING = 3} GameScreen;

struct Symbol
{
	Vector2 position;
	Vector2 target_pos;
        int type;
	bool isNull;
};

struct Streak
{
	Vector2 start;
	int length;
	int type;
	bool horiz;
};

Vector2 GetGridPosition(Vector2, Vector2, int, int, float, int, int);
bool MouseRightPos( int, int, int, int, int, int);
bool IsValidMove(Vector2, Vector2);
std::vector<Streak> GetStreaks(Symbol[][8], int, int);
void RemoveStreaks(Symbol[][8], std::vector<Streak>);
void InitGrid(Symbol[][8], int, int, int);


//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{

    // Initialization (Note windowTitle is unused on Android)
    //--------------------------------------------------------------------------------------
    const int gameScale = 3;
    const int screenWidth = 214 *gameScale;
    const int screenHeight = 214 *gameScale;
    const int symbolCount = 4;
    const int symbolOffset = 1;
    const int gridWidth = 5;
    const int gridHeight = 8;
    const float gridScale = 3.0f;
    const int cellSize = 19;
    const int cellOffset = 1;
    const Vector2 gridPosition = { (screenWidth/2)-38*gameScale , (screenHeight/2) - 79* gameScale};
    const Vector2 backgroundPosition {0,0};
    InitWindow(screenWidth, screenHeight, "S-Wave");

	// Initalize rand
	srand(time(NULL));

    GameScreen currentScreen = LOGO;

    /* 
	 * Initialize variables
	 */




	// Initalize the grid, element access with [x][y]
    Symbol grid[gridWidth][gridHeight] = {};
	InitGrid(grid, gridWidth, gridHeight, symbolCount);

	// Remove eventual streaks from initial configuration
	std::vector<Streak> streaks = GetStreaks(grid, gridWidth, gridHeight);
	int iters = 0;
	while (streaks.size())
	{
		if (iters > 100)
		{
			InitGrid(grid, gridWidth, gridHeight, symbolCount);
			streaks.clear();
			streaks = GetStreaks(grid, gridWidth, gridHeight);
			iters = 0;
		}
		for (auto it = streaks.begin(); it != streaks.end(); it++)
		{
			if ((*it).horiz)
			{
				grid[(int)(*it).start.x + 1][(int)(*it).start.y].type++;
				grid[(int)(*it).start.x + 1][(int)(*it).start.y].type %= symbolCount;
			}
			else
			{
				grid[(int)(*it).start.x][(int)(*it).start.y + 1].type++;
				grid[(int)(*it).start.x][(int)(*it).start.y + 1].type %= symbolCount;
			}
		}
		streaks.clear();
		streaks = GetStreaks(grid, gridWidth, gridHeight);
		iters++;
	}

	// The cell that was last clicked
	// -1, -1 means there was no cell
	Vector2 clickedCell = (Vector2){-1, -1};

    int framesCounter = 0;

	/*
	 * Initialize textures
	 */

	Texture2D gridSprite = LoadTexture("src/sprites/gridSprite.png");

	Texture2D backgroundSprite =  LoadTexture("src/sprites/temp_background.png");

	// Texture2D backgroundSprite [] =
	// {
	//     LoadTexture("src/sprites/background1.png");
	//     LoadTexture("src/sprites/background2.png");
	//     LoadTexture("src/sprites/background3.png");
	//     LoadTexture("src/sprites/background4.png");
	//     LoadTexture("src/sprites/background5.png");
	// };

	Texture2D symbolSprites [] = 
	{
		LoadTexture("src/sprites/bowtie_sprite.png"),
		LoadTexture("src/sprites/romb_sprite.png"),
		LoadTexture("src/sprites/square_sprite.png"),
		LoadTexture("src/sprites/triangle_sprite.png")
	};


    SetTargetFPS(60);

    //--------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------------------------------
        switch(currentScreen) 
        {
            case LOGO: 
            {
                // TODO: Update LOGO screen variables here!

                framesCounter++;    // Count frames

                // Wait for 2 seconds (120 frames) before jumping to TITLE screen
                if (framesCounter > 120)
                {
                    currentScreen = TITLE;
                }
            } break;
            case TITLE: 
            {
                // TODO: Update TITLE screen variables here!

                // Press enter to change to GAMEPLAY screen 
      
		if (IsKeyPressed(KEY_ENTER) || (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) - 50*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {

                    currentScreen = GAMEPLAY;
                } 

            } break;
            case GAMEPLAY:
            { 
                // Update the game
				
				// Handle mouseinput
                if (IsMouseButtonPressed(0))
                {
					clickedCell = GetGridPosition(GetMousePosition(), gridPosition, cellOffset, cellSize, gridScale, gridWidth, gridHeight);
					std::cout << "Press:   " << clickedCell.x << ", " << clickedCell.y << std::endl;
                }
				else if (IsMouseButtonDown(0))
				{
					// Do nothing yet...
				}
				else if (IsMouseButtonReleased(0))
				{
					Vector2 releasedCell = GetGridPosition(GetMousePosition(), gridPosition, cellOffset, cellSize, gridScale, gridWidth, gridHeight);
					std::cout << "Release: " << releasedCell.x << ", " << releasedCell.y << std::endl;
					std::cout << "Move validity: " << IsValidMove(clickedCell, releasedCell) << std::endl;
					if (IsValidMove(clickedCell, releasedCell))
					{
						int cx, cy, rx, ry;
						cx = (int)clickedCell.x; 
						cy = (int)clickedCell.y; 
						rx = (int)releasedCell.x; 
						ry = (int)releasedCell.y; 

						int temp = grid[cx][cy].type;
						grid[cx][cy].type = grid[rx][ry].type;
						grid[rx][ry].type = temp;

						streaks = GetStreaks(grid, gridWidth, gridHeight);
						if (streaks.size())
						{
							RemoveStreaks(grid, streaks);
						}
						else
						{
							temp = grid[cx][cy].type;
							grid[cx][cy].type = grid[rx][ry].type;
							grid[rx][ry].type = temp;
						}
					}
				}
				else
				{
					clickedCell = (Vector2){-1, -1};
				}
            } break;
            case ENDING: 
            {
                // TODO: Update ENDING screen variables here!

                // Press enter to return to TITLE screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = TITLE;
                }  
            } break;
            default: break;
        }
        //-----------------------------------------------------------------------------
        
        // Draw
        //-----------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            switch(currentScreen) 
            {
                case LOGO: 
                {
                    // TODO: Draw LOGO screen here!
		    DrawTextureEx(backgroundSprite, backgroundPosition, 0, gridScale, (Color){255,255,255,255});
		    DrawRectangle(0, 100, 214*gameScale, 80, BLACK);
		    DrawText("LOGO SCREEN", 0, 110, 40, WHITE);
                    DrawText("WAIT for 2 SECONDS...", 0, 150, 20, GRAY);
                    
                } break;
                case TITLE: 
                {
                    // TODO: Draw TITLE screen here!
		    DrawTextureEx(backgroundSprite, backgroundPosition, 0, gridScale, (Color){255,255,255,255});
		    DrawRectangle(0, 100, 214*gameScale, 80, BLACK);


		    DrawRectangle( (screenWidth/2) - 49*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, BLACK);
		    DrawRectangle( (screenWidth/2) + 11*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, BLACK);

		    DrawRectangle( (screenWidth/2) - 50*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale, DARKBLUE);
		    DrawRectangle( (screenWidth/2) + 10*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale, DARKBLUE);
		    

		    DrawText("RUN", (screenWidth/2) - 49*gameScale, (screenHeight/3)* 2+gameScale, 58, (Color){233,0,132,255});
                    DrawText("SCORE", (screenWidth/2) + 11*gameScale, ((screenHeight/3)+7) * 2+gameScale, 34, (Color){233,0,132,255});
		    DrawText("S-Wave Unlimited", 0, 110, 40, WHITE);
                    DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 0, 150, 20, GRAY);
                    
                } break;
                case GAMEPLAY:
                { 
					// Draw the game
		                        DrawTextureEx(backgroundSprite, backgroundPosition, 0, gridScale, (Color){255,255,255,255});
					// Draw the grid
					DrawRectangle( (screenWidth/2)-38*gameScale , (screenHeight/2) - 79* gameScale, 96*gameScale, 153*gameScale, BLACK);
					DrawTextureEx(gridSprite, gridPosition, 0, gridScale, (Color){255,255,255,255});
					// Draw the symbols
					for (int i = 0; i < gridWidth; i++)
					{
						for (int j = 0; j < gridHeight; j++)
						{
							float drawX, drawY;
							drawX = gridPosition.x + (cellOffset + symbolOffset) * gridScale + cellSize * gridScale * i;
							drawY = gridPosition.y + (cellOffset + symbolOffset) * gridScale + cellSize * gridScale * j;
							Vector2 symbolPos = (Vector2) { (int)drawX, (int)drawY };
							DrawTextureEx(symbolSprites[grid[i][j].type], symbolPos, 0, gridScale, (Color){255,255,255,255});
						}
					}

                } break;
                case ENDING: 
                {
                    // TODO: Draw ENDING screen here!
                    DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
                    DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
                    DrawText("PRESS ENTER or TAP to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
                    
                } break;
                default: break;
            }
        
        EndDrawing();
        //----------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------
    
	/*
	 * Unload images, fonts, audio
	 */

    UnloadTexture(gridSprite);
    UnloadTexture(backgroundSprite);
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------

    return 0;
}

// Function for getting the grid position
Vector2 GetGridPosition (Vector2 mousePos, Vector2 gridPos, int cellOffset, int cellSize, float scale, int maxX, int maxY)
{
	float x, y;
	x = mousePos.x - gridPos.x - scale * cellOffset;
	x /= scale * cellSize;
	y = mousePos.y - gridPos.y - scale * cellOffset;
	y /= scale * cellSize;
	if (x < 0 || y < 0 || x >= maxX || y >= maxY)
	{
		return (Vector2) {-1, -1};
	}
	return (Vector2){ (int)x, (int)y };
}


bool MouseRightPos( int mousePosX, int mousePosY, int minX, int minY, int sizeX, int sizeY)
{
    if (minX < mousePosX && (minX+sizeX) > mousePosX && minY < mousePosY && (minY+sizeY) > mousePosY)
	{
	    return true;
	}
    return false;
}

// Function for determining if a move is valid
bool IsValidMove (Vector2 origin, Vector2 end)
{
	if (origin.x == -1 or end.x == -1)
	{
		return false;
	}
	if (abs(origin.x - end.x) == 1 && origin.y - end.y == 0)
	{
		return true;
	}
	if (abs(origin.y - end.y) == 1 && origin.x - end.x == 0)
	{
		return true;
	}
	return false;
}

// Find all streaks and return them
std::vector<Streak> GetStreaks(Symbol grid[][8], int gridWidth, int gridHeight)
{
	// Put on heap if this does not work
	std::vector<Streak> streaks = {};

	for (int i = 0; i < gridWidth; i++)
	{
		int count = 0, type = -1;
		Vector2 start = (Vector2){-1, -1};
		for (int j = 0; j < gridHeight; j++)
		{
			if (grid[i][j].type == type)
			{
				count++;
			}
			else
			{
				if (count > 2)
				{
					streaks.push_back({
							start,
							count,
							type,
							false
							});
				}
				type = grid[i][j].type;
				count = 1;
				start = (Vector2){i, j};
			}
		}
		if (count > 2)
		{
			streaks.push_back({
					start,
					count,
					type,
					false
					});
		}
	}
	
	for (int j = 0; j < gridHeight; j++)
	{
		int count = 0, type = -1;
		Vector2 start = (Vector2){-1, -1};
		for (int i = 0; i < gridWidth; i++)
		{
			if (grid[i][j].type == type)
			{
				count++;
			}
			else
			{
				if (count > 2)
				{
					streaks.push_back({
							start,
							count,
							type,
							true
							});
				}
				type = grid[i][j].type;
				count = 1;
				start = (Vector2){i, j};
			}
		}
		if (count > 2)
		{
			streaks.push_back({
					start,
					count,
					type,
					false
					});
		}
	}

	return streaks;
}

// Remove all streaks from grid
void RemoveStreaks(Symbol grid[][8], std::vector<Streak> streaks)
{

}

// Initialize grid
void InitGrid(Symbol grid[][8], int gridWidth, int gridHeight, int symbolCount)
{
	for (int i = 0; i < gridWidth; i++)
	{
		for (int j = 0; j < gridHeight; j++)
		{
			grid[i][j] = {
				{ (float) i, (float) j },
				{ (float) i, (float) j },
				rand() % symbolCount,
				false};
		}
	}
}


