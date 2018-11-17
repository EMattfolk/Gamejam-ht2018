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

Vector2 GetGridPosition(Vector2, Vector2, int, int, float, int, int);
bool IsValidMove(Vector2, Vector2);

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
	const int symbolCount = 4;
	const int symbolOffset = 1;
	const int gridWidth = 5;
	const int gridHeight = 8;
	const float gridScale = 3.0f;
	const int cellSize = 19;
	const int cellOffset = 1;
	const Vector2 gridPosition = { 500, 100 };

    InitWindow(screenWidth, screenHeight, "Swave");

	// Initalize rand
	srand(time(NULL));

    GameScreen currentScreen = LOGO;

    /* 
	 * Initialize variables
	 */

	// Initalize the grid, element access with [x][y]
    Symbol grid[gridWidth][gridHeight] = {};

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

	// The cell that was last clicked
	// -1, -1 means there was no cell
	Vector2 clickedCell = (Vector2){-1, -1};

    int framesCounter = 0;

	/*
	 * Initialize textures
	 */

	Texture2D gridSprite = LoadTexture("src/gridSprite.png");
	Texture2D symbolSprites [] = 
	{
		LoadTexture("src/bowtie_sprite.png"),
		LoadTexture("src/romb_sprite.png"),
		LoadTexture("src/square_sprite.png"),
		LoadTexture("src/triangle_sprite.png")
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
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
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
                    DrawText("LOGO SCREEN", 20, 20, 40, LIGHTGRAY);
                    DrawText("WAIT for 2 SECONDS...", 290, 220, 20, GRAY);
                    
                } break;
                case TITLE: 
                {
                    // TODO: Draw TITLE screen here!
                    DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
                    DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
                    DrawText("PRESS ENTER or TAP to JUMP to GAMEPLAY SCREEN", 120, 220, 20, DARKGREEN);
                    
                } break;
                case GAMEPLAY:
                { 
					// Draw the game

					// Draw the grid
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
