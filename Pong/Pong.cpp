////////////////////
// Chase Meadows
// 
// First attempt at coding a game. :D
// 
// A lot of the base functionality was gleaned from Youtube video listed in README.
// Mostly this was an exercise to learn how I'm supposed to set up a gameplay loop and utilize a library like raylib.
// A lot of hard-coded constants rather than adaptable to different screen sizes, as well as everything being in main. 
//	Wasn't really worried about it for this endeavor. Only really interested in figuring out the basics of building a game.
// 
// 
// My additions over tutorial:
//	Added menu to select 1- or 2-player game.
//	Added menu to select difficulty, as well as variable AI difficulty for single-player game.
//	Best-of-5 system for winning, instead of just 1 round.
//	Variable vertical speed on ball depending on where it impacted paddle.
//
#include "raylib.h"
#include <cmath>


const float PADDLE_SPEED = 300;	
const float BALL_SPEED = 180;	// Base value for horizontal ball speed, modified by random increase on initialization.
const float MAX_VERTICAL = 400;	// Maximum vertical speed for ball after paddle collision.


struct Ball
{
	float x, y;
	float speedX, speedY;
	float radius;

	Ball()
	{
		this -> init();
	}

	// Resets ball to default values to start new game.
	void init()
	{
		this->x = GetScreenWidth() / 2.0f;			// Start in center of screen
		this->y = GetScreenHeight() / 2.0f;
		this->speedX = (rand() % 20) + BALL_SPEED;	// 200 +- 20 default x speed
		if(rand() % 2 == 1)							// Coin flip who it's going toward first.
			this->speedX *= -1;						
		this->speedY = rand() % 100 + 150;			// 200 +- 50 default y speed (hard-coded value :( )
		if (rand() % 2 == 1)						// Coin flip up or down
			this->speedY *= -1;
		this->radius = 5;
	}
};

struct Paddle
{
	float x, y;
	float speed;
	float width, height;
	int score{};

	Paddle(float x) :
		speed{ PADDLE_SPEED }, width{ 10 }, height{ 100 } 
	{
		this->init(x);
	}

	// Initializes paddle to default position.
	// Only x will differ between left and right paddles.
	void init(float x)						
	{
		this->x = x;
		this->y = GetScreenHeight() / 2 - 50;
	}

	// Rectangle function for collision detection
	Rectangle getRec() 
	{
		return Rectangle{ x, y, width, height };
	}
};


int main()
{	
	InitWindow(800, 600, "Pong");						
	SetWindowState(FLAG_VSYNC_HINT);					
	
	Ball ball;
	Paddle leftPaddle(50);
	Paddle rightPaddle(GetScreenWidth()-50-10);

	bool roundOver{ false },		// Point has been scored
		gameOver{ false },			// Someone has 3 points
		selectionMade{ false },		// 1- or 2-player has been selected
		selectionMadeAI{ false },	// AI difficulty has been selected
		singlePlayer{true};			// Single-player game
	int difficulty{ 40 };			// Represents probability that AI paddle will successfully move on any given frame. 
									// Has a (100-difficulty)% chance of moving toward ball. Higher value means a slower enemy paddle.

	while(!WindowShouldClose())						
	{	
		// Menu for selecting difficulty
		while (!selectionMade)
		{
			// Make sure player can still exit from here.
			if (WindowShouldClose())
			{
				CloseWindow();
				return 0;
			}

			BeginDrawing();
			ClearBackground(BLACK);

			// Draw menu text.
			DrawText("PONG", (GetScreenWidth() - MeasureText("PONG", 100)) / 2, 10, 100, WHITE);
			DrawText("1-Player Game", (GetScreenWidth() - MeasureText("1-Player Game", 50)) / 2, GetScreenHeight() / 2 - 60, 50, BLUE);
			DrawText("2-Player Game", (GetScreenWidth() - MeasureText("1-Player Game", 50))  / 2, GetScreenHeight() / 2 + 10, 50, RED);
			
			// Draw selection cursor (pong ball) at correct position.
			if(singlePlayer)
				DrawCircle((GetScreenWidth() - MeasureText("1-Player Game", 50)) / 2 - 20, GetScreenHeight() / 2 - 35, 5, WHITE);
			else 
				DrawCircle((GetScreenWidth() - MeasureText("1-Player Game", 50)) / 2 - 20, GetScreenHeight() / 2 + 35, 5, WHITE);

			// Allow for toggling selection.
			if (IsKeyPressed('W') || IsKeyPressed('S') || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP))
				singlePlayer = !singlePlayer;

			// Make selection.
			if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
				selectionMade = true;
			EndDrawing();
		}
		
		// Menu for selecting difficulty if 1-player was chosen.
		while (singlePlayer && !selectionMadeAI)
		{
			// Make sure player can still exit from here.
			if (WindowShouldClose())
			{
				CloseWindow();
				return 0;
			}
			BeginDrawing();
			ClearBackground(BLACK);

			// Draw menu text.
			DrawText("Easy", (GetScreenWidth() - MeasureText("Easy", 50)) / 2, GetScreenHeight() / 2 - 100, 50, GREEN);
			DrawText("Normal", (GetScreenWidth() - MeasureText("Easy", 50)) / 2, GetScreenHeight() / 2 - 25, 50, YELLOW);
			DrawText("Hard", (GetScreenWidth() - MeasureText("Easy", 50)) / 2, GetScreenHeight() / 2 + 50, 50, RED);

			// Draw selection cursor (pong ball) at correct position.
			if (difficulty == 40)
				DrawCircle((GetScreenWidth() - MeasureText("Easy", 50)) / 2 - 20, GetScreenHeight() / 2 - 75, 5, WHITE);
			else if(difficulty == 20)
				DrawCircle((GetScreenWidth() - MeasureText("Easy", 50)) / 2 - 20, GetScreenHeight() / 2 , 5, WHITE);
			else if(difficulty == 0)
				DrawCircle((GetScreenWidth() - MeasureText("Easy", 50)) / 2 - 20, GetScreenHeight() / 2 + 75, 5, WHITE);

			// Allow for toggling selection and cycling menu.
			if (IsKeyPressed('W') || IsKeyPressed(KEY_UP))
			{
				difficulty += 20;
				if (difficulty > 40)
					difficulty = 0;
			}
			if (IsKeyPressed('S') || IsKeyPressed(KEY_DOWN))
			{
				difficulty -= 20;
				if (difficulty < 0)
					difficulty = 40;
			}

			// Make selection.
			if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
				selectionMadeAI = true;

			EndDrawing();
		}



		// Update ball position using current speed
		ball.x += ball.speedX * GetFrameTime();
		ball.y += ball.speedY * GetFrameTime();

		// If ball bounces off top or bottom, reverse y-speed
		if (ball.y < 0)
		{
			ball.y = 0;
			ball.speedY *= -1;
		}
		if (ball.y > GetScreenHeight())
		{
			ball.y = GetScreenHeight();
			ball.speedY *= -1;
		}

		// Player 1 control
		if(IsKeyDown('W') && leftPaddle.y > 0)
			leftPaddle.y -= leftPaddle.speed * GetFrameTime();
		if(IsKeyDown('S') && leftPaddle.y < GetScreenHeight()-leftPaddle.height)
			leftPaddle.y += leftPaddle.speed * GetFrameTime();



		// AI control if 1-player game.
		if(singlePlayer)
		{ 
			// If the computer rolls higher than the difficulty value, it will be allowed an input this frame.
			// Otherwise won't move.
			// Leads to slower-moving paddle on lower difficulty.
			if (rand() % 100 > difficulty)
			{
				// Instructions are identical to 2-player controls, 
				//	except AI will automatically try to level itself with the current height of the ball every frame.
				// Could have let the AI determine where the ball would end up by the time it got to the right side, but this would lead to a 
				//	perfectly-centered hit every time leading to 0 y-speed. Boring.
				if (ball.y > rightPaddle.y + rightPaddle.height / 2 && rightPaddle.y < GetScreenHeight() - rightPaddle.height)
					rightPaddle.y += rightPaddle.speed * GetFrameTime();
				if (ball.y < rightPaddle.y + rightPaddle.height / 2 && rightPaddle.y > 0)
					rightPaddle.y -= rightPaddle.speed * GetFrameTime();
			}
		
		}

		// Player 2 control if 2-player game
		else
		{
			// Allow paddle to move up and down according to key press and restrict paddle to screen dimensions.
			if (IsKeyDown(KEY_UP) && rightPaddle.y > 0)
				rightPaddle.y -= rightPaddle.speed * GetFrameTime();
			if (IsKeyDown(KEY_DOWN) && rightPaddle.y < GetScreenHeight() - rightPaddle.height)
				rightPaddle.y += rightPaddle.speed * GetFrameTime();
		}








		// Check for collision with right paddle.
		if (CheckCollisionCircleRec({ ball.x,ball.y }, ball.radius, rightPaddle.getRec()))
		{
			// Initialize ball speed to just whether it was moving up or down when it hit the paddle.
			// This does lead to the ball only moving in one direction the entire match, unfortunately.
			if (ball.speedY > 0)
				ball.speedY = 1;
			else
				ball.speedY = -1;
			// Alter ball's vertical speed from 0 to MAX_VERTICAL depending on where the ball impacts the paddle.
			// The closer to the center of the paddle, the less vertical speed will be transfered, up to a maximum value at the edge.
			ball.speedY *= std::abs(ball.y - (rightPaddle.y + (rightPaddle.height / 2.0f))) / (rightPaddle.height / 2.0f) * MAX_VERTICAL;

			ball.x = rightPaddle.x - ball.radius/2.0f;
			// Slightly increase ball speed each impact with paddle.
			ball.speedX *= -1.1f;
		}

		// Check for collision with left paddle. (Same as above)
		if (CheckCollisionCircleRec({ ball.x,ball.y }, ball.radius, leftPaddle.getRec()))
		{		
			if (ball.speedY > 0)
				ball.speedY = 1;
			else
				ball.speedY = -1;
			ball.speedY *= std::abs(ball.y - (leftPaddle.y + (leftPaddle.height / 2.0f))) / (leftPaddle.height / 2.0f) * MAX_VERTICAL;
			ball.x = leftPaddle.x + ball.radius + leftPaddle.width;
			ball.speedX *= -1.1f;
		}

		// If ball leaves left side of screen
		if (ball.x <= 0)
		{
			// End round and increment player 2 score.
			if (!roundOver)
			{
				rightPaddle.score++;
				// If game over, set flag.
				if (rightPaddle.score == 3)
					gameOver = true;
				roundOver = true;
			}
			

			// Display point message for end round or end game.
			if(!gameOver)
				DrawText("Point Player 2", (GetScreenWidth() - MeasureText("Point Player 2", 50))/2, 20, 50, RED);
			else
				DrawText("Player 2 Wins!", (GetScreenWidth() - MeasureText("Player 2 Wins!", 60)) / 2, GetScreenHeight()/2 - 30, 60, RED);
		}

		// If ball leaves right side of screen
		if (ball.x >= GetScreenWidth())
		{
			// End round and increment player 2 score.
			if (!roundOver)
			{
				leftPaddle.score++;
				// If game over, set flag.
				if (leftPaddle.score == 3)
					gameOver = true;
				roundOver = true;
			}
			

			// Display point message for end round or end game.
			if(!gameOver)
				DrawText("Point Player 1", (GetScreenWidth() - MeasureText("Point Player 1", 50))/2, 20, 50, BLUE);
			else
				DrawText("Player 1 Wins!", (GetScreenWidth() - MeasureText("Player 1 Wins!", 60)) / 2, GetScreenHeight() / 2 - 30, 60, BLUE);
		}


		BeginDrawing();									
			ClearBackground(BLACK);	

			// Wait for input to restart.
			if (roundOver)
			{
				DrawText("(Press SPACE to Continue)", (GetScreenWidth() - MeasureText("(Press SPACE to Continue)", 30)) / 2, GetScreenHeight() - 40, 30, WHITE);
				if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
				{
					// If game is over, reset scores and paddles to center.
					if (gameOver)
					{
						gameOver = false;
						selectionMade = false;
						selectionMadeAI = false;
						leftPaddle.score = 0;
						rightPaddle.score = 0;
						leftPaddle.init(50);
						rightPaddle.init(GetScreenWidth() - 50 - 10);
					}
					roundOver = false;
					ball.init();
				}
			}
				


			DrawCircle((int)ball.x, (int)ball.y, ball.radius, WHITE);													// Ball

			DrawRectangle(leftPaddle.x, leftPaddle.y, leftPaddle.width, leftPaddle.height, BLUE);						// Left paddle
			DrawText(TextFormat("%d", leftPaddle.score), MeasureText("0", 60) - 5, GetScreenHeight()-60, 40, BLUE);		// Left score

			DrawRectangle(rightPaddle.x, rightPaddle.y, rightPaddle.width, rightPaddle.height, RED);					// Right paddle
			DrawText(TextFormat("%d", rightPaddle.score), GetScreenWidth() - MeasureText("0", 60) - MeasureText("0", 40) + 5, GetScreenHeight() - 60, 40, RED);
																														// Right score


		DrawFPS(0, 0);							
		EndDrawing();									
	}

	CloseWindow();										
	return 0;
}