#pragma once
#ifndef GAME_H
#define GAME_H

#define HIGHSCORE "data/highscore.txt"

#include <vector>

#include "Mesh.h"
#include "Model.h"
#include "AudioMgr.h"
#include "FPSCamera.h"

//Holds user stats
struct userStats
{	
	int lives = 3;
	float timeSurvived = 0.0f;
};

//Holds game variables
struct gameAttributes
{
	//Timer for projectile spawn
	float spawnCountdown;
	
	//number input from a(-1) and d(1) keys
	float controllerInput;
	
	//player position
	float controllerPosition;
	
	//Speed of projectile movement
	float projectileSpeed;
};

//Game states for render function
enum gameStates
{
	main,
	instruct,
	game,
	death
};

class Game
{
public:

	Game() {}
	~Game() {
		Release();
	}
	void Update(float dTime);
	void Render(float dTime);
	void OnResize(int screenWidth, int screenHeight);
	void Initialise();
	void Release();
	
	/**
	 * \brief Takes keyboard input and assigns number value for direction
	 */
	void MovePlane();
	
	/**
	 * \brief Loops through projectile vector in memory and assigns a rand position between -25 to 25
	 */
	void SpawnProjectiles();
	
	/**
	 * \brief Assigns a movement speed to projectiles using the increasespeed function and checks for hit detection
	 * \param dTime 
	 */
	void MoveProjectiles(float dTime);
	
	/**
	 * \brief Creates a mapped axis that the player can move across left to right
	 * \param inMin 
	 * \param inMax 
	 * \param outMin 
	 * \param outMax 
	 * \param inNumber 
	 * \return 
	 */
	float MapNumber(float inMin, float inMax, float outMin, float outMax, float inNumber);
	
	/**
	 * \brief Had to write my own clamp function as C++15 doesn't have one
	 * \param n 
	 * \param lower 
	 * \param upper 
	 * \return 
	 */
	float Clip(float n, float lower, float upper);
	
	/**
	 * \brief Uses max function to create a linear equation for progression of spawn timer
	 * \return new lower spawn countdown
	 */
	float IncreaseDifficultyOverTime();
	
	/**
	 * \brief Uses min function to create a linear equation for progression of movement speed
	 * \return new higher move speed
	 */
	float IncreaseMoveSpeedOverTime();

	/**
	 * \brief Reads highscore from external txt file
	 */
	void ReadFromFile();
	
	/**
	 * \brief Writes new highscore to external txt file
	 */
	void WriteToFile();
	
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Model mBox, mQuad, mSkybox, mPlane;

	//Vector of projectiles stored in memory to be reused rather than created/destroyed
	std::vector<Model*> mProjectiles;

private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	//Player move speed
	float moveSpeed = 1.5f;

	//Difficulty factor for spawn counter linear equation
	float difficultyFactor = 0.027f;

	//Difficulty factor for movement speed linear equation
	float speedDifficultyFactor = 0.375f;

	//Longest time between projectile spawn
	float maxSpawnInterval = 2.f;
	
	//Shortest time between projectile spawn
	float lowestSpawnInterval = 0.3f;

	//Fastest projectile movement speed
	float maxProjectileSpeed = 100.f;

	//Slowest projectile movement speed
	float lowestProjectileSpeed = 25.f;

	//Highscore initalised by reading from txt file
	float highScore;

	int mScreenWidth, mScreenHeight;
	
	unsigned int mMusicHandler;
	unsigned int mSoundEffectHandler;
	
	DirectX::SimpleMath::Vector3 planePosition = DirectX::SimpleMath::Vector3(0, 0, 0);
	
	FPSCamera mCamera;
	std::vector<Model*> mOpaques;

	//Render functions for each game state
	void MainMenu();
	void Instruct();
	void MainGame();
	void DeathScreen();
};

#endif

