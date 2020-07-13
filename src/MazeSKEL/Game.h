#pragma once
#ifndef GAME_H
#define GAME_H

#include <vector>

#include "Mesh.h"
#include "Model.h"
#include "FPSCamera.h"

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
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Model mBox, mQuad, mSkybox, mPlane;
	std::vector<Model*> mProjectiles;

private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	float gAngle = 0;
	float spawnInterval = 5;
	float spawnCountdown = spawnInterval;
	float controllerInput;
	float controllerPosition;
	DirectX::SimpleMath::Vector3 planePosition = DirectX::SimpleMath::Vector3(0, -20, 0);
	float moveSpeed = 1.5f;
	FPSCamera mCamera;
	std::vector<Model*> mOpaques;

};

#endif

