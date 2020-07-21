#pragma once
#ifndef GAME_H
#define GAME_H

#define HIGHSCORE "data/highscore.txt"

#include <vector>

#include "Mesh.h"
#include "Model.h"
#include "AudioMgr.h"
#include "FPSCamera.h"

struct userStats
{
	int lives = 3;
	int projectilesAvoided = 0;
	float timeSurvived = 0.0f;
};

struct gameAttributes
{
	float spawnCountdown;
	float controllerInput;
	float controllerPosition;
	float projectileSpeed;
};

enum gameStates
{
	main,
	instruct,
	game,
	death,
	score
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
	void MovePlane();
	void SpawnProjectiles();
	void MoveProjectiles(float dTime);	
	float MapNumber(float inMin, float inMax, float outMin, float outMax, float inNumber);
	float Clip(float n, float lower, float upper);
	float IncreaseDifficultyOverTime();
	float IncreaseMoveSpeedOverTime();

	void ReadFromFile();
	void WriteToFile();
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Model mBox, mQuad, mSkybox, mPlane;
	std::vector<Model*> mProjectiles;

	int mScreenWidth, mScreenHeight;
	unsigned int mMusicHandler;
	unsigned int mSoundEffectHandler;

private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	float moveSpeed = 1.5f;
	
	float difficultyFactor = 0.047f;
	float speedDifficultyFactor = 0.375f;
	
	float maxSpawnInterval = 5.f;
	float lowestSpawnInterval = 0.3f;
	
	float maxProjectileSpeed = 100.f;
	float lowestProjectileSpeed = 25.f;

	float highScore;

	DirectX::SimpleMath::Vector3 planePosition = DirectX::SimpleMath::Vector3(0, 0, 0);
	
	FPSCamera mCamera;
	std::vector<Model*> mOpaques;

	void MainMenu();
	void Instruct();
	void MainGame();
	void DeathScreen();
};

#endif

