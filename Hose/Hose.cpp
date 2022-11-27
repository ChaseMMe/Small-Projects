////////////////////////////////////////////////////////////////////////////////////////////
// 
//  Messing around with water physics
//	Completed 8/20/22
// 
//  Simple project written by Chase Meadows using raylib v4.2 (www.raylib.com)
//	raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
//
// 
//	Wanted to make something completely from scratch and get a handle on manipulating 
//	objects and collision.
//	
//	Program allows for creation of water objects that interact with platforms, 
//	each other, and boxes. No real game here, but satisfying. :)
//
// 
////////////////////////////////////////////////////////////////////////////////////////////


#include "raylib.h"
#include <cmath>
#include <list>
#include <vector>

const int SCREEN_WIDTH = 800;	
const int SCREEN_HEIGHT = 600;

const int HOSE_THICKNESS = 30;		// Hose height
const int HOSE_DEPTH = 50;			// Depth of hose tube. Hoze nozzle is scaled from this.

const int WATER_SIZE = 5;			// Water droplet size
const int WATER_SPEED = 12;			// Base speed for water from hose.

const float PLAT_HEIGHT = 20;		// Thickness of platforms
const int MAX_PLATFORMS = 3;		// Maximum number of platforms allowed

const float MIN_BOX_SIZE = 25;		// Minimum size for spawned boxes.
const int BOX_SIZE_VARIANCE = 75;	// Max random value added to minimum box size.


//////////////////////////////
// Hose definition
struct Hose
{
	float x{}, y{};
	float height, width;

	// Default constructor sets hose to top left of screen and initializes sizes based off constants.
	Hose() : x{}, y{}, height{ HOSE_THICKNESS }, width{ HOSE_DEPTH * 3 / 2 } {}

	// Returns tube portion of hose.
	Rectangle getHoseTube() const {
		return { x, y, HOSE_DEPTH, height };
	}
		
	// Returns nozzle portion of hose.
	Rectangle getHoseTip() const {
		return { x + HOSE_DEPTH, y, HOSE_DEPTH/2, height };
	}

	// Draw function to draw entire hose.
	void draw() const {
		DrawRectangleRec(this->getHoseTube(), GREEN);
		DrawRectangleRec(this->getHoseTip(), ORANGE);
	}
};

//////////////////////////////
// Water droplet definition.
struct Water
{
	float x, y;
	float airtime;				// Timer for how long droplet has been airborne to determine fall speed.
	float xSpeed, ySpeed;

	// Constructor initializes values to passed parameters and randomizes initial x speed to 
	// WATER_SPEED + 0 to 60
	Water(float x, float y) : x{ x }, y{ y }, airtime{}, ySpeed{} {
		xSpeed = WATER_SPEED + static_cast<float>(rand() % 60) * GetFrameTime();
	}

	// Updates airtime by 1 and returns updated vertical speed
	float getYSpeed() {
		airtime += 1;
		ySpeed = airtime * airtime * 0.5 * GetFrameTime();
		return ySpeed;
	}

	// Returns raylib Rectangle object for collision and drawing.
	Rectangle getWaterRec() const {
		return { x, y, WATER_SIZE, WATER_SIZE };
	}

	const void draw() const {
		DrawRectangleRec(getWaterRec(), BLUE);
	}
};

//////////////////////////////
// Platform definition
struct Platform 
{
	float x, y;
	float width, height{ PLAT_HEIGHT };

	// Constructor accepts position and width. Height is constant across all platforms.
	Platform(float x, float y, float width) :
		x{ x }, y{ y }, width{ width } {}

	Rectangle getPlatRec() const {
		return { x, y, width, height };
	}

	void draw() const {
		DrawRectangleRec(getPlatRec(), GRAY);
	} 
};

//////////////////////////////
// Box definition
struct Box
{
	float x, y;
	float size;
	float airtime{};			// Timer for how long droplet has been airborne to determine fall speed.
	float xSpeed{}, ySpeed{};

	// Constructor accepts x and y values (will be taken from mouse position)
	Box(float mX, float mY)
	{
		// Randomizes size using constants above
		size = MIN_BOX_SIZE + static_cast<float>(rand() % BOX_SIZE_VARIANCE);
		// Creates center of box at mouse position.
		x = mX - size / 2;
		y = mY - size /2;
	}

	// Same as Water getYSpeed.
	// Updates airtime and uses it to update and return vertical speed.
	float getYSpeed() {
		airtime += 1;
		ySpeed = airtime * airtime * 0.45 * GetFrameTime();
		return ySpeed;
	}

	Rectangle getRec() const {
		return { x, y, size, size };
	}

	const void draw() const {
		DrawRectangleRec(getRec(), BROWN);
	}
};

// Update all game object data.
void updateGame(Hose& hose, std::list<Water>& water, std::vector<Platform>& plats, std::vector<Box>& boxes);

// Draw current frame.
void drawGame(const Hose& hose, const std::list<Water>& water, const std::vector<Platform>& plats, const std::vector<Box>& boxes);

// Generate a new set of platforms.
void initializePlats(std::vector<Platform>& plats);

// Make a box.
void makeBox(std::vector<Box>& boxes);

//////////////////////////////
// main
int main()
{
	// Window and fps initialization.
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "hose");
	SetTargetFPS(60);

	// Initialize objects and data structures.
	Hose hose;
	std::list<Water> water;
	std::vector<Platform> plats;
	std::vector<Box> boxes;

	// Main game loop.
	while (!WindowShouldClose())
	{
		updateGame(hose, water, plats, boxes);
		drawGame(hose, water, plats, boxes);
	}

	CloseWindow();
	return 0;
}

//////////////////////////////
// Erase all current platforms and generate a new set of random ones with given constraints.
void initializePlats(std::vector<Platform>& plats)
{
	// Erase any current platforms.
	plats.clear();
	float x, y, w;

	// For each index up to maximum allowed platforms
	for (int i{}; i < MAX_PLATFORMS; i++)
	{
		// Width of platform will be at least 1/10 screen length + up to 1/3 the screen length.
		w = SCREEN_WIDTH / 10 + rand() % (SCREEN_WIDTH / 3);

		// x position will be at least a hose-length away from the hose and will be constrained by the right edge of the screen.
		x = (HOSE_DEPTH * 2) + rand() % static_cast<int>(SCREEN_WIDTH - (HOSE_DEPTH * 2) - w);

		// y position will be at least 1 platform height away from the top of the screen and not below the bottom of the screen.
		y = PLAT_HEIGHT + rand() % static_cast<int>(SCREEN_HEIGHT - 2 * PLAT_HEIGHT);

		// Make new platform with these constraints
		plats.emplace_back(x, y, w);

		// Check each other platform in the vector (Can't have too many anyway)
		for (int j{}; j < i; j++)

			// If this new platform collides with an existing platform or it is less than the maximum box length above or below another...
			if (CheckCollisionRecs(plats[i].getPlatRec(), plats[j].getPlatRec()) || abs(plats[i].y - plats[j].y) < 100 + PLAT_HEIGHT)
			{
				// Delete the one we just made
				plats.pop_back();
				// Decrement the count so we can try again and don't bother checking the others.
				i--;
				break;
			}
	}
}
//////////////////////////////
// Make box will generate a new box at the current mouse position.
// boxes is a vector to allow for multiple boxes, but it's restrained to 1 total box until I get around to adding collision between boxes.
void makeBox(std::vector<Box>& boxes)
{
	float x = (float)(GetMouseX());
	float y = (float)(GetMouseY());
	// Add new box if no boxes currently exist.
	if(boxes.empty())
		boxes.emplace_back(x,y);
}

//////////////////////////////
// updateGame is the main workhorse of the program.
// All updating/collision checking/object making is done in or called from this function.
void updateGame(Hose& hose, std::list<Water>& water, std::vector<Platform>& plats, std::vector<Box>& boxes)
{
	// If space is pressed, generate 3 new platforms.
	if (IsKeyPressed(KEY_SPACE))
		initializePlats(plats);
	
	// If right click or up key are pressed, spawn box at mouse position.
	if (IsMouseButtonPressed(1) || IsKeyPressed(KEY_UP))
		makeBox(boxes);
	
	////////////////////
	// Update hose position
	// Set center of hose to current mouse position.
	// Hose x-value is constant and fixed to left side of screen.
	hose.y = static_cast<float>(GetMouseY()) - HOSE_THICKNESS / 2.0f;
	// Constrain hose to top and bottom of screen.
	if (hose.y < 0)
		hose.y = 0;
	if (hose.y > SCREEN_HEIGHT - hose.height)
		hose.y = SCREEN_HEIGHT - hose.height;

	////////////////////
	// Update box position.
	// Cycle through each box in boxes
	for (int i{}; i < boxes.size(); i++)
	{
		// Update box x value from speed.
		boxes[i].x += boxes[i].xSpeed;

		// Update box y speed and position
		boxes[i].y += boxes[i].getYSpeed();

		// Reduce box speed by 75% to simulate friction.
		boxes[i].xSpeed *= 0.25;

		// Cycle through all currently-generated platforms.
		for (int j{}; j < plats.size(); j++)
			// If the box is colliding with the platform, set it on top of the platform and reset its airtime.
			if (CheckCollisionRecs(boxes[i].getRec(), plats[j].getPlatRec()))
			{
				boxes[i].y = plats[j].y - boxes[i].size;
				boxes[i].airtime = 0;
			}

		// Whenever box exits screen, delete it.
		if (boxes[i].x > SCREEN_WIDTH || boxes[i].y > SCREEN_HEIGHT)
			boxes.erase(boxes.begin()+(i--));
	}

	////////////////////
	// Create new water.
	// If left-click held down
	if(IsMouseButtonDown(0) || IsKeyDown(KEY_DOWN))
		// Create 10 new water droplets.
		for (int i{}; i<10; i++)
			// These will be placed at the tip of the hose, at a random y-location along the hose nozzle.
			water.emplace_back(hose.width - WATER_SPEED, hose.y + static_cast<float>(rand()%(static_cast<int>(hose.height - WATER_SIZE))));

	////////////////////
	// Update water positions.
	// Iterate through every water droplet.
	std::list<Water>::iterator drop = water.begin();
	while (drop != water.end())
	{
		// Update droplet x and y positions (this will also update y speed).
		drop->x += drop->xSpeed;
		drop->y += drop->getYSpeed();

		// Check for collision with boxes.
		for (int i{}; i < boxes.size(); i++)
			// If water droplet is colliding with box...
			if (CheckCollisionRecs(drop->getWaterRec(), boxes[i].getRec()))
			{	
				// If water is colliding with box from above.
				if (drop->y <= boxes[i].y - WATER_SIZE + 5)
				{
					// Set water droplet on top of box
					drop->y = boxes[i].y - WATER_SIZE;

					// If drop is going a certain speed downward.
					if (drop->ySpeed > 5)
					{
						// Have water "bounce" off of box and slow it down.
						drop->y -= drop->ySpeed * 1.5;
						drop->airtime /= 2;
					}

					else drop->airtime = 0;

					// Every frame on top of box, simulate friction by slowing it down.
					drop->xSpeed *= 0.95;
				}

				// If water is colliding with box from the side.
				else if (drop->x <= boxes[i].x - WATER_SIZE + 10)
				{	
					// Don't let it go inside box.
					drop->x = boxes[i].x - WATER_SIZE;

					// Transfer some of water speed to box speed (Less as box gets larger)
					boxes[i].xSpeed += drop->xSpeed / boxes[i].size;

					// Maintain SOME forward speed from water. (Mostly to create the "flood" effect as the water pushes the box off the side.
					drop->xSpeed *= 0.1;
				}
			}

		// Check collision with platforms.
		for (int i{}; i < plats.size(); i++)
		{
			if (CheckCollisionRecs(drop->getWaterRec(), plats[i].getPlatRec()))
			{
				// If drop is going a certain speed downward.
				if (drop->ySpeed > 5)
				{
					// Have water "bounce" off of box and slow it down.
					drop->y -= drop->ySpeed*1.5;
					drop->airtime /= 2;
				}
				// Otherwise just set it on top of platform.
				else
				{
					drop->y = plats[i].y - WATER_SIZE;
					drop->airtime = 0;
				}

				// Every frame on top of box, simulate friction by slowing it down.
				drop->xSpeed *= 0.92;

				// To stop water from "pooling" on platforms. If the water is going below a certain speed, give it a small nudge toward the nearest edge.
				// This will create a dripping effect from the sides of the platform.
				if (drop->xSpeed <= 0.01)
				{
					if (drop->x < plats[i].x + plats[i].width / 2)
						drop->xSpeed -= 0.005;
					else
						drop->xSpeed += 0.005;
				}

				// Check collision with next droplet in the list. If it's next to an adjacent one, accelerate both.
				// This is a really basic way of pushing droplets off the edge faster in the event that there is a LOT of water pooled up.
				if (std::next(drop) != water.end() && CheckCollisionRecs(drop->getWaterRec(), (std::next(drop))->getWaterRec()))
				{
					drop->y-=WATER_SIZE/2;
					drop->xSpeed *= 1.11;
					(std::next(drop))->xSpeed *= 1.11;
				}
			}
		}

		// If water has left the screen, delete it.
		if (drop->x > SCREEN_WIDTH || drop->y > SCREEN_HEIGHT)
			water.erase(drop++);
		else
			drop++;
	}
}

// Draw function simply clears the screen, displays the current FPS, and draws out all the updated objects.
void drawGame(const Hose& hose, const std::list<Water>& water, const std::vector<Platform>& plats, const std::vector<Box>& boxes)
{	
	BeginDrawing();
	ClearBackground(BLACK);
	DrawFPS(0, 30);

	// Draw hose
	hose.draw();
	// Draw all water droplets.
	for (const auto& i : water)
		i.draw();
	// Draw platforms.
	for (const auto& i : plats)
		i.draw();
	// Draw boxes.
	for (const auto& i : boxes)
		i.draw();

	// This is a counter to track how many water droplets are currently on the screen.
	DrawText(TextFormat("%d", water.size()), 0, 0, 20, WHITE);
	EndDrawing();
}