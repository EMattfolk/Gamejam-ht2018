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

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization (Note windowTitle is unused on Android)
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1280;
    const int screenHeight = 720;
	const int gridWidth = 5;
	const int gridHeight = 8;

    InitWindow(screenWidth, screenHeight, "Swave");

    GameScreen currentScreen = LOGO;

    /* 
	 * Initialize variables
	 */

	const Vector2 gridPosition = { 100, 100 };

	// Initalize the grid, element access with [x][y]
    Symbol grid[gridWidth][gridHeight] = {};

    int framesCounter = 0;

	/*
	 * Initialize textures
	 */

	Texture2D gridSprite = LoadTexture("src/gridSprite.png");

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
                // TODO: Update GAMEPLAY screen variables here!

                // Press enter to change to ENDING screen
                if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP))
                {
                    currentScreen = ENDING;
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
					DrawTexture(gridSprite, gridPosition.x, gridPosition.y, (Color){255,255,255,255});
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
