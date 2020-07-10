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
	LRESULT WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	Model mBox, mQuad, mSkybox, mPlane;

private:
	Game& operator=(const Game&) = delete;
	Game(const Game& m) = delete;

	float gAngle = 0;
	FPSCamera mCamera;
	std::vector<Model*> mOpaques;

};

#endif

