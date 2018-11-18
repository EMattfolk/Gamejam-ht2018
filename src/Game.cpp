/*                      *
 *                      *
 *        S-wave        *
 *                      *
 *       Creators       *
 *     Erik Mattfolk    *
 *       Annie Wång     *
 *      Victor Lells    *
 *                      *
 */                    

/*
 *
 * Polishing:
 *  Nothing ATM
 */

#include "raylib.h"
#include "raymath.h"
#include "stdlib.h"
#include "time.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO = 0, SPLASH = 1,TITLE = 2, GAMEPLAY = 3, SCORE = 4, ENDING = 5} GameScreen;

struct Symbol
{
    Vector2 position;
    Vector2 target_pos;
    int type;
    bool isNull;
    bool marked;
};

struct Streak
{
    Vector2 start;
    int length;
    int type;
    bool horiz;
};

struct intTrio
{
    int timer;
    int houseCycle;
    int skyCycle;
}; 

Vector2 GetGridPosition(Vector2, Vector2, int, int, float, int, int);
bool MouseRightPos(int, int, int, int, int, int);
bool IsValidMove(Vector2, Vector2);
std::vector<Streak> GetStreaks(Symbol[][8], int, int);
bool MarkStreak(Symbol[][8], Streak);
void InitGrid(Symbol[][8], int, int, int);
intTrio BackgroundAnimation(int,int,int);
void RespawnSymbols(Symbol[][8], int, int, int);
void ModifyScore(float*, float*, float, int, int);
float GetScoreRate(float);

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
    const float symbolSpeed = 1.0f/8;
    const int gridWidth = 5;
    const int gridHeight = 8;
    const float gridScale = 3.0f;
    const int cellSize = 19;
    const int cellOffset = 1;
    const int fadeInTime = 900;
    const Vector2 scorePosition = {(screenWidth/2)-30*gameScale , (screenHeight/2) - 105* gameScale};
    const Vector2 gridPosition = { (screenWidth/2)-38*gameScale , (screenHeight/2) - 79* gameScale};
    const Vector2 barPosition = { gridPosition.x - 15 * gameScale, gridPosition.y };
    const Vector2 backgroundPosition {0,0};
    InitWindow(screenWidth, screenHeight, "S-Wave");

    // Initalize rand
    srand(time(NULL));	
    GameScreen currentScreen = LOGO; 

    // Initialize variables
    intTrio aniTrio = {0,0,0};
    GameScreen targetScreen = TITLE;

	// Initialize beat timings
	std::vector<float> beats = {};
	std::ifstream beatFile;
	beatFile.open("src/Songs_TxtBeat/time_list_avicii_levels_new.txt");
	int beatn = 1;
	while (!beatFile.eof())
	{
		float temp;
		beatFile >> temp;
		if (beatn % 2 == 0) 
		beats.push_back(temp);
		beatn++;
	}
	beatFile.close();

	// Set the current beat
	int currentBeat = 0;

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

	// Load scores
	std::vector<int> highScores = {};
	std::ifstream scoreFile;
	scoreFile.open("src/scores.txt");
	while (!scoreFile.eof())
	{
		int temp;
		scoreFile >> temp;
		highScores.push_back(temp);
	}
	scoreFile.close();

	// The cell that was last clicked
	// -1, -1 means there was no cell
	Vector2 clickedCell = (Vector2){-1, -1};
	
	// The last cell that was held
	Vector2 lastHeldCell = (Vector2){-1, -1};

	// Inialize the different scores and rates
	float currentScore = 200, score = 0, negativeRateTime = 0;
	// Frame counter
	int framesCounter = 0;
	// Secs per frame (for time calculations)
	float secsPerFrame = 1.0f / 60;

	/*
	 * Initialize textures
	 */

	Texture2D gridSprite = LoadTexture("src/sprites/gridSprite.png");

	Texture2D backgroundSprite =  LoadTexture("src/sprites/temp_background.png");

	Texture2D backgroundSprites [] =
	{
	    LoadTexture("src/sprites/background1.png"),
	    LoadTexture("src/sprites/background2.png"),
	    LoadTexture("src/sprites/background3.png"),
	    LoadTexture("src/sprites/background4.png"),
	    LoadTexture("src/sprites/background5.png")
	};

	Texture2D buttonSprites [] =
	{
	    LoadTexture("src/sprites/play_button.png"),
	    LoadTexture("src/sprites/score_button.png"),
	    LoadTexture("src/sprites/retry_button.png"),
	    LoadTexture("src/sprites/title_button.png")
	};

	Texture2D backgroundHouseSprite = LoadTexture("src/sprites/buildings.png");

	Texture2D barSprite = LoadTexture("src/sprites/bar_sprite.png");

	Texture2D beatSprite = LoadTexture("src/sprites/beat_sprite.png");

	Texture2D scoreSprite = LoadTexture("src/sprites/score_bar_sprite.png");
	
	Texture2D upArrowSprite = LoadTexture("src/sprites/arrow_up_sprite.png");

	Texture2D downArrowSprite = LoadTexture("src/sprites/arrow_down_sprite.png");

	Texture2D dadzLogoSprite = LoadTexture("src/sprites/the_dadz_logo.png");
	Texture2D sWaveLogoSprite = LoadTexture("src/sprites/s-wave_logo.png");
	Texture2D presLogoSprite = LoadTexture("src/sprites/presents.png");
	
	Texture2D symbolSprites [] = 
	{
		LoadTexture("src/sprites/bowtie_sprite.png"),
		LoadTexture("src/sprites/romb_sprite.png"),
		LoadTexture("src/sprites/square_sprite.png"),
		LoadTexture("src/sprites/triangle_sprite.png")
	};

	// Init audio
	InitAudioDevice();
	// Load songs
	Music levels = LoadMusicStream("src/Songs_TxtBeat/Levels.ogg");
	Music doItTonight = LoadMusicStream("src/Songs_TxtBeat/DoItTonight.ogg");

	PlayMusicStream(doItTonight);

    SetTargetFPS(60);

    //--------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
		// Update Sound
		if (currentScreen == GAMEPLAY)
		{
			if (IsMusicPlaying(levels))
			{
				UpdateMusicStream(levels);
			}
			else
			{
				StopMusicStream(doItTonight);
				PlayMusicStream(levels);
			}
		}
		else
		{
			if (IsMusicPlaying(doItTonight))
			{
				UpdateMusicStream(doItTonight);
			}
			else
			{
				StopMusicStream(levels);
				PlayMusicStream(doItTonight);
			}
		}
        // Update
        //-----------------------------------------------------------------------------
        switch(currentScreen) 
        {
            case LOGO: 
            {
                // TODO: Update LOGO screen variables here!

                framesCounter++;    // Count frames

		if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
		{
		    currentScreen = TITLE;
		    framesCounter = 0;

		}
                // Wait for fadeInTime before jumping to TITLE screen
                if (framesCounter > fadeInTime)
                {
		    currentScreen = TITLE;
		    framesCounter = 0;
                }
            } break;
	    case SPLASH: 
            {
                // TODO: Update LOGO screen variables here!

                framesCounter++;    // Count frames

		if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
		{
		    currentScreen = targetScreen;
		    framesCounter = 0;
		    currentScore = 200;
		    currentBeat = 0;
		}
                // Wait for fadeInTime before jumping to TITLE screen
                if (framesCounter > fadeInTime/3)
                {
                    currentScreen = targetScreen;
		    framesCounter = 0;
		    currentScore = 200;
		    currentBeat = 0;
                }
            } break;
            case TITLE: 
            {
                // TODO: Update TITLE screen variables here!

                // Press enter or the button to change the screen 
      
		if( (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) - 50*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {
		    targetScreen = GAMEPLAY;
                    currentScreen = SPLASH;
		    currentScore = 200;
		    currentBeat = 0;
                }
		else if( (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) + 10*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {
		    targetScreen = SCORE;
                    currentScreen = SPLASH;
		    currentScore = 200;
		    currentBeat = 0;
                }

            } break;
            case GAMEPLAY:
            { 
                // Update the game

				// Update to the frame we are on
                framesCounter++;

		if (IsKeyPressed(KEY_O))
		{
		    targetScreen = ENDING;
		    currentScreen = SPLASH;
		    framesCounter = 0;

		}
				// Handle mouseinput
                if (IsMouseButtonPressed(0))
                {
					clickedCell = GetGridPosition(GetMousePosition(), gridPosition, cellOffset, cellSize, gridScale, gridWidth, gridHeight);
					lastHeldCell = clickedCell;
                }
				else if (IsMouseButtonDown(0))
				{
					Vector2 holdCell = GetGridPosition(GetMousePosition(), gridPosition, cellOffset, cellSize, gridScale, gridWidth, gridHeight);
					grid[(int)lastHeldCell.x][(int)lastHeldCell.y].target_pos = lastHeldCell;
					if (IsValidMove(clickedCell, holdCell))
					{
						grid[(int)holdCell.x][(int)holdCell.y].target_pos = clickedCell;
						grid[(int)clickedCell.x][(int)clickedCell.y].target_pos = holdCell;
					}
					else
					{
						grid[(int)clickedCell.x][(int)clickedCell.y].target_pos = clickedCell;
					}

					lastHeldCell = holdCell;
				}
				else if (IsMouseButtonReleased(0))
				{
					Vector2 releasedCell = GetGridPosition(GetMousePosition(), gridPosition, cellOffset, cellSize, gridScale, gridWidth, gridHeight);

					int cx, cy, rx, ry;
					cx = (int)clickedCell.x; 
					cy = (int)clickedCell.y; 
					rx = (int)releasedCell.x; 
					ry = (int)releasedCell.y; 

					if (IsValidMove(clickedCell, releasedCell))
					{
						Symbol temp = grid[cx][cy];
						grid[cx][cy] = grid[rx][ry];
						grid[rx][ry] = temp;

						streaks = GetStreaks(grid, gridWidth, gridHeight);
						if (!streaks.size())
						{
							temp = grid[cx][cy];
							grid[cx][cy] = grid[rx][ry];
							grid[rx][ry] = temp;
						}
					}

					grid[rx][ry].target_pos = releasedCell;
					grid[cx][cy].target_pos = clickedCell;
				}
				else
				{
					clickedCell = (Vector2){-1, -1};
				}

				// Update symbols in grid
				for (int i = 0; i < gridWidth; i++)
				{
					for (int j = 0; j < gridHeight; j++)
					{
						if (grid[i][j].position.y != grid[i][j].target_pos.y)
						{
							float delta = grid[i][j].target_pos.y - grid[i][j].position.y;
							delta = abs(delta) / delta;
							grid[i][j].position.y += symbolSpeed * delta;
						}
						if (grid[i][j].position.x != grid[i][j].target_pos.x)
						{
							float delta = grid[i][j].target_pos.x - grid[i][j].position.x;
							delta = abs(delta) / delta;
							grid[i][j].position.x += symbolSpeed * delta;
						}
					}
				}

				float currentTime = secsPerFrame * framesCounter;
				streaks = GetStreaks(grid, gridWidth, gridHeight);
				if (streaks.size())
				{
					for (auto it = streaks.begin(); it != streaks.end(); it++)
					{
						if (MarkStreak(grid, *it))
						{
							int extraScore = 0;
							if (currentBeat != beats.size() && beats[currentBeat] - currentTime < 0.2)
							{
								extraScore += 200;
							}
							if ((*it).length > 4) negativeRateTime = 3.0f;
							ModifyScore(&score, &currentScore, 0, (*it).length, extraScore);
						}
					}
					RespawnSymbols(grid, gridWidth, gridHeight, symbolCount);
				}

			if (negativeRateTime > 0)
			{
				ModifyScore(&score, &currentScore, -1, 0, 0);
			}
			else
			{
				ModifyScore(&score, &currentScore, framesCounter * secsPerFrame, 0, 0);
			}

			negativeRateTime -= secsPerFrame;

			// When done go to end screen
			if (currentScore < 0 || currentBeat == beats.size())
			{
			    targetScreen = ENDING;
			    currentScreen = SPLASH;
				highScores.push_back((int)score);
				std::sort(highScores.begin(), highScores.end(), std::greater<int>());
				std::ofstream scoreFile;
	            scoreFile.open("src/scores.txt");
				for (int i = 0; i < 20 && i < highScores.size(); i++)
				{
					scoreFile << highScores[i] << "\n";
				}
				scoreFile.close();
			}
			
	    } break;
	    case SCORE: 
            {
                // TODO: Update TITLE screen variables here!

                // Press enter or the button to change the screen 
      
		if( (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) - 50*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {
		    targetScreen = TITLE;
                    currentScreen = SPLASH;
		    currentScore = 200;
		    currentBeat = 0;
                }
		else if( (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) + 10*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {
		    targetScreen = GAMEPLAY;
                    currentScreen = SPLASH;
		    currentScore = 200;
		    currentBeat = 0;
                }

            } break;
            case ENDING: 
            {
                // TODO: Update ENDING screen variables here!

                // Press to return to TITLE screen / GAMEPLAY screen
		if( (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) - 50*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {
		    targetScreen = GAMEPLAY;
                    currentScreen = SPLASH;
		    currentScore = 200;
		    currentBeat = 0;
                }
		else if( (IsGestureDetected(GESTURE_TAP) &&  MouseRightPos( GetMouseX(),  GetMouseY(), (screenWidth/2) + 10*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale)))
                {
		    targetScreen = TITLE;
                    currentScreen = SPLASH;
					score = 0;
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
		    DrawRectangle( 0,0, screenWidth, screenHeight, BLACK);
		    if (framesCounter < 300)
		    {
			DrawTextureEx(dadzLogoSprite, Vector2{(screenWidth/2)-85*gameScale,(screenHeight/2)-44*gameScale}, 0, (gridScale/3), (Color){255,255,255,(255*sin(framesCounter*0.01047198))});
		    }
		    else if(framesCounter < 600)
		    {
			DrawTextureEx(presLogoSprite, Vector2{(screenWidth/2)-85*gameScale,(screenHeight/2)-44*gameScale}, 0, (gridScale/3), (Color){255,255,255,(255*sin((framesCounter-300)*0.01047198))});
		    }
		    else if(framesCounter < 900)
		    {
			DrawTextureEx(sWaveLogoSprite, Vector2{(screenWidth/2)-103*gameScale,(screenHeight/2)-44*gameScale}, 0, (gridScale/3), (Color){255,255,255,(255*sin((framesCounter-600)*0.01047198))});
		    }
                } break;
	        case SPLASH: 
                {
                    DrawRectangle( 0,0, screenWidth, screenHeight, BLACK);
		    DrawTextureEx(sWaveLogoSprite, Vector2{(screenWidth/2)-103*gameScale,(screenHeight/2)-44*gameScale}, 0, (gridScale/3), (Color){255,255,255,(255*sin(framesCounter*0.01047198))});
			
                } break;
                case TITLE: 
                {
                    // TODO: Draw TITLE screen here!

		    //Recives the correct position modifers from the function
		    aniTrio = BackgroundAnimation(aniTrio.timer, aniTrio.houseCycle, aniTrio.skyCycle );
		    
		    // Applies the houseCycle modifer to the house textture pos
		    float houseY =  ((22 * gameScale)*sinf(aniTrio.houseCycle/30.55775f))+22*gameScale;
		    Vector2 housePosition = (Vector2) {0, houseY}; 

		    //Draws the Background and Houses
		    DrawTextureEx(backgroundSprites[aniTrio.skyCycle], backgroundPosition, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(backgroundHouseSprite, housePosition, 0, gridScale, (Color){255,255,255,255});

		    //Draws the sonic like text background
		    DrawRectangle(0, 100, (screenWidth/3)*2, 80, BLACK);

		    //Draws the button shadow
		    DrawRectangle( (screenWidth/2) - 49*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, (Color){0,0,0,64});
		    DrawRectangle( (screenWidth/2) + 11*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, (Color){0,0,0,64});

		    //Draws the buttons
		    DrawTextureEx(buttonSprites[0], Vector2 {(screenWidth/2) - 50*gameScale, (screenHeight/3)*2}, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(buttonSprites[1], Vector2 {(screenWidth/2) + 10*gameScale, (screenHeight/3)*2}, 0, gridScale, (Color){255,255,255,255});

		    // DrawRectangle( (screenWidth/2) - 50*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale, DARKBLUE);
		    // DrawRectangle( (screenWidth/2) + 10*gameScale, (screenHeight/3)*2, 40*gameScale, 20*gameScale, DARKBLUE);
		    
		    //Draws both the button text and Background text
		    // DrawText("RUN", (screenWidth/2) - 49*gameScale, (screenHeight/3)* 2+gameScale, 58, (Color){233,0,132,255});
                    // DrawText("SCORE", (screenWidth/2) + 11*gameScale, ((screenHeight/3)+7) * 2+gameScale, 34, (Color){233,0,132,255});
		    DrawText("S-Wave Unlimited", 5*gameScale, 37*gameScale, 40, WHITE);
                    DrawText("Play the game or see the leaderboard!", 5*gameScale, 50*gameScale, 20, GRAY);
                    
                } break;
                case GAMEPLAY:
                { 

		    // DRAW THE GAME
		    std::string s = std::to_string((int)score);
		    //Recives the correct position modifers from the function
		    aniTrio = BackgroundAnimation(aniTrio.timer, aniTrio.houseCycle, aniTrio.skyCycle );
		    
		    // Applies the houseCycle modifer to the house textture pos
		    float houseY =  ((11 * gameScale)*sinf(aniTrio.houseCycle/30.55775f))+11*gameScale;
		    Vector2 housePosition = (Vector2) {0, houseY}; 

		    // Draws the Background and Houses
		    DrawTextureEx(backgroundSprites[aniTrio.skyCycle], backgroundPosition, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(backgroundHouseSprite, housePosition, 0, gridScale, (Color){255,255,255,255});
		    
		    // Draw bar
		    DrawTextureEx(barSprite, barPosition, 0, gridScale, (Color){255,255,255,255});

		    // Draw Score Bar
		    DrawTextureEx(scoreSprite, scorePosition, 0, gridScale, (Color){255,255,255,255});

		    // Draw Score Text
		    DrawText("SCORE:", (screenWidth/2)-25*gameScale , (screenHeight/2) - 105* gameScale, 35, (Color){219,21,206,255});
		    DrawText(s.c_str(), (screenWidth/2)-25*gameScale , (screenHeight/2) - 92* gameScale, 35, (Color){219,21,206,255});

			// Draw rectangle in bar depending on score
			int barHeight = (int)(151 * gameScale * currentScore / 1000);
			DrawRectangleGradientV(barPosition.x + cellOffset * gameScale, barPosition.y + 152 * gameScale - barHeight, 30, barHeight, ORANGE, RED);

			// Draw the arrow(s) in the bar
			Vector2 arrowPos;
			if (negativeRateTime > 0)
			{
				arrowPos = (Vector2){ barPosition.x + 2 * gameScale, barPosition.y + 152 * gameScale - barHeight - 12 * gameScale};
				DrawTextureEx(upArrowSprite, arrowPos, 0, gridScale, (Color){255,255,255,255});
			}
			else
			{
				arrowPos = (Vector2){ barPosition.x + 2 * gameScale, barPosition.y + 152 * gameScale - barHeight + 2 * gameScale};
				DrawTextureEx(downArrowSprite, arrowPos, 0, gridScale, (Color){255,255,255,255});
			}

			// Draw beat indicators
			float currentTime = secsPerFrame * framesCounter;
			for (int b = currentBeat; b < beats.size(); b++)
			{
				if (beats[b] < currentTime)
				{
					currentBeat++;
					continue;
				}
				float delta = beats[b] - currentTime;
				if (delta < 2)
				{
					delta -= 0.2;
					// Set color to fade beat
					Color c;
					if (delta < 0) c = (Color){255,255,255,(int)(255 * (delta + 0.2f) / 0.2f)};
					else c = (Color){255,255,255,255};
					// Draw beat
					Vector2 beatPosition1 = (Vector2){ barPosition.x - 10 * gameScale - (int)(35 * delta * gameScale), barPosition.y + 51 * gameScale};
					Vector2 beatPosition2 = (Vector2){ gridPosition.x + 96 * gameScale + (int)(35 * delta * gameScale), barPosition.y + 51 * gameScale};
					DrawTextureEx(beatSprite, beatPosition1, 0, gridScale, c);
					DrawTextureEx(beatSprite, beatPosition2, 0, gridScale, c);
				}
			}

		    // DRAW THE GRID
		    // DrawRectangle( (screenWidth/2)-38*gameScale , (screenHeight/2) - 79* gameScale, 96*gameScale, 153*gameScale, BLACK); Dis dos black box behind grid
		    DrawTextureEx(gridSprite, gridPosition, 0, gridScale, (Color){255,255,255,255});
		    // DRAW THE SYMBOLS
		    for (int i = 0; i < gridWidth; i++)
		    {
			for (int j = 0; j < gridHeight; j++)
			{
			    float drawX, drawY;
			    drawX = gridPosition.x + (cellOffset + symbolOffset) * gridScale + cellSize * gridScale * grid[i][j].position.x;
			    drawY = gridPosition.y + (cellOffset + symbolOffset) * gridScale + cellSize * gridScale * grid[i][j].position.y;
			    Vector2 symbolPos = (Vector2) { (int)drawX, (int)drawY };
			    DrawTextureEx(symbolSprites[grid[i][j].type], symbolPos, 0, gridScale, (Color){255,255,255,255});
			}
		    }

                } break;
	        case SCORE:
		{
		    std::string s = std::to_string((int)score);
		    //Recives the correct position modifers from the function
		    aniTrio = BackgroundAnimation(aniTrio.timer, aniTrio.houseCycle, aniTrio.skyCycle );
		    
		    // Applies the houseCycle modifer to the house textture pos
		    float houseY =  ((22 * gameScale)*sinf(aniTrio.houseCycle/30.55775f))+22*gameScale;
		    Vector2 housePosition = (Vector2) {0, houseY}; 

		    //Draws the Background and Houses
		    DrawTextureEx(backgroundSprites[aniTrio.skyCycle], backgroundPosition, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(backgroundHouseSprite, housePosition, 0, gridScale, (Color){255,255,255,255});

		    for(int i = 0; i < 10; i++)
		    {
			if (i < highScores.size()) s = std::to_string((int)highScores[i]);
			else s = std::to_string(0);
			if (i % 2 == 0)
			{
			    DrawTextureEx(scoreSprite, Vector2 {(screenWidth/2) - (65* gameScale), (10*gameScale) + i/2*25*gameScale}, 0, gridScale, (Color){255,255,255,255});
			}
			DrawText("SCORE:", (screenWidth/2) - (63* gameScale), (10*gameScale) + (int)(i*12.5*gameScale), 25, (Color){219,21,206,255});
			DrawText(s.c_str(), (screenWidth/2) - (30* gameScale),(10*gameScale) + (int)(i*12.5*gameScale), 25, (Color){219,21,206,255});

		    }

		    for(int i = 0; i < 10; i++)
		    {
			if (i+10 < highScores.size()) s = std::to_string((int)highScores[i+10]);
			else s = std::to_string(0);
			if (i % 2 == 0)
			{
			    DrawTextureEx(scoreSprite, Vector2 {(screenWidth/2) + (5* gameScale), (10*gameScale) + i/2*25*gameScale}, 0, gridScale, (Color){255,255,255,255});
			}
			
		    
			DrawText("SCORE:", (screenWidth/2) + (8* gameScale), (10*gameScale) + (int)(i*12.5*gameScale), 25, (Color){219,21,206,255});
			DrawText(s.c_str(), (screenWidth/2) + (40* gameScale),(10*gameScale) + (int)(i*12.5*gameScale), 25, (Color){219,21,206,255});

		    }
		    
		    //Draws the button shadow
		    DrawRectangle( (screenWidth/2) - 49*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, (Color){0,0,0,64});
		    DrawRectangle( (screenWidth/2) + 11*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, (Color){0,0,0,64});

		    //Draws the buttons
		    DrawTextureEx(buttonSprites[3], Vector2 {(screenWidth/2) - 50*gameScale, (screenHeight/3)*2}, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(buttonSprites[0], Vector2 {(screenWidth/2) + 10*gameScale, (screenHeight/3)*2}, 0, gridScale, (Color){255,255,255,255});
		} break;
	        case ENDING: 
                {

		    std::string s = std::to_string((int)score);
		    //Recives the correct position modifers from the function
		    aniTrio = BackgroundAnimation(aniTrio.timer, aniTrio.houseCycle, aniTrio.skyCycle );
		    
		    // Applies the houseCycle modifer to the house textture pos
		    float houseY =  ((22 * gameScale)*sinf(aniTrio.houseCycle/30.55775f))+22*gameScale;
		    Vector2 housePosition = (Vector2) {0, houseY}; 

		    //Draws the Background and Houses
		    DrawTextureEx(backgroundSprites[aniTrio.skyCycle], backgroundPosition, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(backgroundHouseSprite, housePosition, 0, gridScale, (Color){255,255,255,255});

		    DrawTextureEx(scoreSprite, Vector2 {(screenWidth/2) - (90* gameScale), 57*gameScale}, 0, gridScale*3, (Color){255,255,255,255});
		    
		    //DrawRectangle((screenWidth/2) - (55* gameScale), 100*gameScale, 112*gameScale, 30*gameScale, (Color){0,0,0,255});
		    DrawText("SCORE:", (screenWidth/2) - (50* gameScale), 30*gameScale, 80, (Color){219,21,206,255});
		    DrawText(s.c_str(), (screenWidth/2) - (85* gameScale), 70*gameScale, 160, (Color){219,21,206,255});

		    //Draws the button shadow
		    DrawRectangle( (screenWidth/2) - 49*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, (Color){0,0,0,64});
		    DrawRectangle( (screenWidth/2) + 11*gameScale, (screenHeight/3)* 2+1*gameScale, 41*gameScale, 21*gameScale, (Color){0,0,0,64});

		    //Draws the buttons
		    DrawTextureEx(buttonSprites[2], Vector2 {(screenWidth/2) - 50*gameScale, (screenHeight/3)*2}, 0, gridScale, (Color){255,255,255,255});
		    DrawTextureEx(buttonSprites[3], Vector2 {(screenWidth/2) + 10*gameScale, (screenHeight/3)*2}, 0, gridScale, (Color){255,255,255,255});

		    
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
    
    for(int i = 0; i < 4; i++)
    {
	UnloadTexture(symbolSprites[i]);
    }

    for(int i = 0; i < 5; i++)
    {
	UnloadTexture(backgroundSprites[i]);
    }
    for(int i = 0; i < 4; i++)
    {
	UnloadTexture(buttonSprites[i]);
    }
    UnloadTexture(scoreSprite);
    UnloadTexture(dadzLogoSprite);
    UnloadTexture(sWaveLogoSprite);
    UnloadTexture(presLogoSprite);
    UnloadTexture(backgroundHouseSprite);

    UnloadTexture(backgroundSprite);

	UnloadMusicStream(levels);
	UnloadMusicStream(doItTonight);

	CloseAudioDevice();
    
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

// Function for checkin if the mouse position is correct
bool MouseRightPos( int mousePosX, int mousePosY, int minX, int minY, int sizeX, int sizeY)
{
    //If the mouse postion is within a box of space retrun true
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
					true
					});
		}
	}

	return streaks;
}

// Mark streak for deletion returns if it could be deleted
bool MarkStreak(Symbol grid[][8], Streak streak)
{
	int x = (int)streak.start.x, y = (int)streak.start.y;
	if (streak.horiz)
	{
		for (int i = 0; i < streak.length; i++)
		{
			if (grid[x + i][y].position.x != grid[x + i][y].target_pos.x || grid[x + i][y].position.y != grid[x + i][y].target_pos.y)
			{
				return false;
			}
		}

		for (int i = 0; i < streak.length; i++)
		{
			grid[x + i][y].marked = true;
		}
	}
	else
	{
		for (int i = 0; i < streak.length; i++)
		{
			if (grid[x][y + i].position.x != grid[x][y + i].target_pos.x || grid[x][y + i].position.y != grid[x][y + i].target_pos.y)
			{
				return false;
			}
		}

		for (int i = 0; i < streak.length; i++)
		{
			grid[x][y + i].marked = true;
		}
	}

	return true;
}

// Respawn all destroyed symbols
void RespawnSymbols(Symbol grid[][8], int gridWidth, int gridHeight, int symbolCount)
{
	for (int i = 0; i < gridWidth; i++)
	{
		int count = 0;
		for (int j = gridHeight - 1; j >= 0; j--)
		{
			if (grid[i][j].marked && grid[i][j].position.y == grid[i][j].target_pos.y - count)
			{
				count++;
				Symbol temp = grid[i][j];
				temp.type = rand() % symbolCount;
				temp.position = (Vector2){ i, grid[i][0].position.y - 1 };
				temp.target_pos = (Vector2){ i, 0 };
				temp.marked = false;

				for (int k = j - 1; k >= 0; k--)
				{
					grid[i][k].target_pos.y++;
					grid[i][k+1] = grid[i][k];
				}
				grid[i][0] = temp;
				j++;
			}
		}
	}
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
				false,
				false};
		}
	}
}

// Function that keeps track of animations 
intTrio BackgroundAnimation(int animationTimer, int houseAnimationCycle, int skyAnimationCycle)
{
    // Keeps track of how many frames has passed
    animationTimer++;
    // If the current frame rotaion is under 48 the cycle number should rise
    if(animationTimer <= 48)
    {
	// Every 12 frames the background should change
	if ((animationTimer % 12) == 0)
        {
	    skyAnimationCycle++;
        }

	// Every frame the house position should change
 	houseAnimationCycle++;
	
    }
    //If the current frame rotaion is over 48 the cycle number should be reduced
    else if(animationTimer <= 96)
    {
	// Every 12 frames the background should change
	if ((animationTimer % 12) == 0)
	{
	    skyAnimationCycle--;
	}
	// Every frame the house position should change
	houseAnimationCycle++;

    }
    //After 96 frames reset the rotation
    else
    {
	animationTimer      = 0;
    }

    return {animationTimer, houseAnimationCycle, skyAnimationCycle};

}

// Update the score
void ModifyScore(float *score, float *currentScore, float gameTime, int combo, int extraScore)
{
	// Get multiplier for score
	float multiplier = 1.0f;
	if (*currentScore > 500)
	{
		multiplier += 0.5f;
	}
	if (*currentScore > 900)
	{
		multiplier += 0.5f;
	}
	// Assign points
	if (combo == 3)
	{
		*score += 10 * multiplier;
		*currentScore += 10;
	}
	else if (combo == 4)
	{
		*score += 20 * multiplier;
		*currentScore += 20;
	}
	else if (combo == 5)
	{
		*score += 50 * multiplier;
		*currentScore += 50;
	}
	else if (combo > 5)
	{
		*score += 200 * multiplier;
		*currentScore += 200;
	}
	// Calculate the rate and its multiplier
	float rateMultiplier = 1.0f + *currentScore / 1000;
	float rate = GetScoreRate(gameTime) * rateMultiplier;
	if (gameTime < 0) rate = -1.0f / 60;
	// Remove rate from currentScore
	*currentScore -= rate;
	// Clamp current score
	if (*currentScore > 1000)
	{
		*currentScore = 1000.0f;
	}
	// Add the extra score
	*score += extraScore;
	// Test
	if (extraScore) std::cout << "Extra score get" << std::endl;
}

// Returns the rate at which the current score is decreasing
float GetScoreRate (float time)
{
	return pow(time, 0.5f) / 60;
}
