#include <algorithm>

#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"
#include "FX.h"
#include "Input.h"
#include <fstream>
#include <ctime>
#include <iomanip>


#include "TextRenderer.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//Engine doesn't respect encapsulation so...
userStats gUserStats;
gameAttributes gGameAttributes;
gameStates currentState;

void Game::OnResize(int screenWidth, int screenHeight)
{
	mScreenHeight = screenHeight;
	mScreenWidth = screenWidth;
	OnResize_Default(mScreenWidth, mScreenHeight);
}

void Game::Initialise()
{
	TextRenderer::Initialise();

	//Seed rand with time for VC++' terrible RNG
	srand(time(nullptr));
	
	//Reset struct data
	gUserStats = {};
	gGameAttributes = {};

	//Load main menu
	currentState = main;

	//Read in highscore from text file
	ReadFromFile();

	//Creates 25 missiles at beginning of the game to be stored in memory and reused
	//Active(true)= in use, false = in memory 
	Mesh& pt = GetMeshManager()->CreateMesh("missile");
	pt.CreateFrom("data/missile.obj", gd3dDevice, FX::GetMyFX()->mCache);
	MaterialExt matP;
	for(int i = 0; i < 25; i++)
	{
		Model* projectile = new Model;
		projectile->Initialise(*GetMeshManager()->GetMesh("missile"));
		projectile->GetRotation() = Vector3(PI, 0, 0);
		projectile->GetScale() = Vector3(.075, .075, .075);
		
		matP = projectile->GetMesh().GetSubMesh(0).material;
		matP.gfxData.Set(Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1), Vector4(1, 1, 1, 1));
		matP.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("missile.dds", true, gd3dDevice);
		matP.texture = "missile.dds";
		
		projectile->SetOverrideMat(&matP);
		mOpaques.push_back(projectile);
		mProjectiles.push_back(projectile);
	}
	
	//Plane
	Mesh& et = GetMeshManager()->CreateMesh("Plane");
	et.CreateFrom("data/plane.obj", gd3dDevice, FX::GetMyFX()->mCache);
	
	mPlane.Initialise(et);
	mPlane.GetPosition() = Vector3(0,0,0);
	mPlane.GetRotation() = Vector3(0, PI, 0);
	mPlane.GetScale() = Vector3(.01, .01, .01);
	mPlane.active = true;
	
	MaterialExt mat = mPlane.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.5, 0.5, 0.5, 1), Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("plane.dds", true, gd3dDevice);
	mat.texture = "plane.dds";
	mPlane.SetOverrideMat(&mat);
	
	//Skybox
	Mesh& sb = GetMeshManager()->CreateMesh("skybox");
	sb.CreateFrom("data/skybox.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mSkybox.Initialise(sb);
	mSkybox.GetScale() = Vector3(1,1,1);
	mSkybox.GetPosition() = Vector3(0,0,0);
	mSkybox.GetRotation() = Vector3(PI/2,0,0);
	MaterialExt& defMat = mSkybox.GetMesh().GetSubMesh(0).material;
	defMat.flags &= ~MaterialExt::LIT;
	defMat.flags &= ~MaterialExt::ZTEST;

	mOpaques.push_back(&mPlane);

	FX::SetupDirectionalLight(0, true, Vector3(-0.7f, -0.7f, 0.7f), Vector3(0.67f, 0.67f, 0.67f), Vector3(0.25f, 0.25f, 0.25f), Vector3(0.15f, 0.15f, 0.15f));
	mCamera.Initialise(Vector3(0, 30, -55), Vector3(0, 20, 1), FX::GetViewMatrix());
	mCamera.LockMovementAxis(0, 30, -55);
	mCamera.LockRotationAxis(0, 0, 0, 0, 0, 0);
}

void Game::Release()
{
}

float Game::MapNumber(float inMin, float inMax, float outMin, float outMax, float inNumber)
{
	return (inNumber - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

float Game::Clip(float n, float lower, float upper)
{
	return max(lower, min(n, upper));
}

float Game::IncreaseDifficultyOverTime()
{
	return max(maxSpawnInterval - difficultyFactor * gUserStats.timeSurvived, lowestSpawnInterval);
}

float Game::IncreaseMoveSpeedOverTime()
{
	return min(speedDifficultyFactor * gUserStats.timeSurvived + lowestProjectileSpeed, maxProjectileSpeed);
}

void Game::ReadFromFile()
{
	ifstream readFile;
	readFile.open(HIGHSCORE);

	if(readFile.is_open())
	{
		while(!readFile.eof())
		{
			//ifstream only has 6 figures of precision, use more to stop rounding errors
			readFile >> setprecision(9);
			readFile >> highScore;
		}
	}

	readFile.close();
}

void Game::WriteToFile()
{
	ofstream writeFile(HIGHSCORE);

	if(writeFile.is_open())
	{
		if(gUserStats.timeSurvived > highScore)
		{
			highScore = gUserStats.timeSurvived;
		}
		//ofstream only has 6 figures of precision, use more to stop rounding errors
		writeFile << setprecision(9);
		writeFile << highScore;
	}

	writeFile.close();
}

void Game::Update(float dTime)
{
	//Play game soundtrack
	if (GetIAudioMgr()->GetSongMgr()->IsPlaying(mMusicHandler) == false)
		GetIAudioMgr()->GetSongMgr()->Play("Soundtrack", true, false, &mMusicHandler, 0.3f);

	//Update and Input for each gamestate
	switch (currentState) {
	case gameStates::main:
		if (GetMouseAndKeys()->IsPressed(VK_SPACE)) {
			currentState = game;
		}
		else if (GetMouseAndKeys()->IsPressed(VK_I)) {
			currentState = instruct;
		}
		break;
	case gameStates::instruct:
		if (GetMouseAndKeys()->IsPressed(VK_SPACE)) {
			currentState = game;
		}
		else if (GetMouseAndKeys()->IsPressed(VK_B)) {
			currentState = main;
		}
		break;
	case gameStates::game:
		//Increases time survived
		gUserStats.timeSurvived += dTime;
		//Countdown spawn timer
		gGameAttributes.spawnCountdown -= dTime;
		//Spawn new projectile and reset timer
		if (gGameAttributes.spawnCountdown <= 0)
		{
			SpawnProjectiles();
			gGameAttributes.spawnCountdown = IncreaseDifficultyOverTime();
		}
		//Player Input
		MovePlane();
		//Projectile Movement
		MoveProjectiles(dTime);

		//Player movement across mapped axis
		gGameAttributes.controllerPosition += gGameAttributes.controllerInput * dTime * moveSpeed;
		gGameAttributes.controllerPosition = Clip(gGameAttributes.controllerPosition, -1, 1);
		planePosition.x = MapNumber(-1, 1, -25, 25, gGameAttributes.controllerPosition);
		mPlane.GetPosition() = Vector3(planePosition.x, 0, 0);
		
		break;
	case gameStates::death:
		//reset game on death
		if (GetMouseAndKeys()->IsPressed(VK_SPACE)) {
			gUserStats = {};
			gGameAttributes = {};
			currentState = game;
		}
		break;
	default:
		break;
	}
}

void Game::MovePlane()
{
	if (GetMouseAndKeys()->IsPressed(VK_A)) {
		gGameAttributes.controllerInput = -1;
	}
	else if (GetMouseAndKeys()->IsPressed(VK_D)) {
		gGameAttributes.controllerInput = 1;
	}
	else
		gGameAttributes.controllerInput = 0;
}

void Game::SpawnProjectiles()
{
	//C++15 RNG for Projectile Pos
	float pos = (rand() % 50 + 1) - 26;

	//Set active to true for use
	//Assign position with RNG
	for(int i = 0; i < mProjectiles.size(); i++)
	{
		if(!mProjectiles[i]->active)
		{
			mProjectiles[i]->active = true;
			mProjectiles[i]->GetPosition() = Vector3(pos, 0, 150);
			break;
		}
	}
}

void Game::MoveProjectiles(float dTime)
{
	//Increases projectile movement speed
	gGameAttributes.projectileSpeed = IncreaseMoveSpeedOverTime();

	//Loop through projectile vector
	for (int i = 0; i < mProjectiles.size(); i++)
	{
		//If the projectile is active (in use, not in memory)
		if (mProjectiles[i]->active)
		{
			//Assign movement
			mProjectiles[i]->GetPosition() = mProjectiles[i]->GetPosition() - Vector3(0, 0, dTime * gGameAttributes.projectileSpeed);
			mProjectiles[i]->GetRotation() = mProjectiles[i]->GetRotation() - Vector3(0, 0, dTime * 5);

			//Hit detection
			if(mProjectiles[i]->GetPosition().z <= 0 && !mProjectiles[i]->playerHit)
			{
				mProjectiles[i]->playerHit = true;
				if(abs(mProjectiles[i]->GetPosition().x - mPlane.GetPosition().x) < 8)
				{
					//Player Hit Audio
					if (!GetIAudioMgr()->GetSfxMgr()->IsPlaying(mSoundEffectHandler))
						GetIAudioMgr()->GetSfxMgr()->Play("hit", false, false, &mSoundEffectHandler, 1.f);

					//Remove a life
					gUserStats.lives--;
					//Send projectile back to memory to be respawned
					mProjectiles[i]->active = false;
					//Reset hit bool
					mProjectiles[i]->playerHit = false;

					//Last life audio
					if(gUserStats.lives == 1)
						GetIAudioMgr()->GetSfxMgr()->Play("panic", true, false, &mSoundEffectHandler, 1.f);

					//Death mechanics
					if(gUserStats.lives <= 0)
					{
						//Make sure player cannot have negative lives
						gUserStats.lives = 0;
						//Save highscore
						WriteToFile();
						//Deactivate all missiles
						for (int i = 0; i < mProjectiles.size(); i++)
						{
								mProjectiles[i]->active = false;
						}
						currentState = death;
					}
				}
			}

			//Kill plane for any avoided missiles
			if (mProjectiles[i]->GetPosition().z < -65)
			{
				mProjectiles[i]->active = false;
				mProjectiles[i]->playerHit = false;
			}
		}
	}
}

void Game::Render(float dTime)
{
	BeginRender(Colours::Black);

	FX::SetPerFrameConsts(gd3dImmediateContext, mCamera.GetPos());

	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f * PI, GetAspectRatio(), 1, 1000.f);

	//Render calls for each state
	switch (currentState) {
	case gameStates::main:
		MainMenu();
		break;
	case gameStates::instruct:
		Instruct();
		break;
	case gameStates::game:
		MainGame();	
		break;
	case gameStates::death:
		DeathScreen();
		break;
	default:
		break;
	}
	EndRender();

	GetMouseAndKeys()->PostProcess();
}

LRESULT Game::WindowsMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	const float camInc = 200.f * GetElapsedSec();

	switch (msg)
	{
	case WM_INPUT:
		GetMouseAndKeys()->MessageEvent((HRAWINPUT)lParam);
		break;
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		}
	}

	//default message handling (resize window, full screen, etc)
	return DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

void Game::MainMenu()
{
	TextRenderer::SetFont("cod");
	TextRenderer::DrawString
	(
		L"Mark of Duty: P4G Ops", Vector2((mScreenWidth/2) - 325, 0), Vector3(1, 1, 1)
	);
	TextRenderer::SetFont("LatoRegular16");
	TextRenderer::DrawString
	(
		L"Press Space to Start", Vector2((mScreenWidth / 2) - 100, 540), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press I for Instructions", Vector2((mScreenWidth / 2) - 100, 570), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press Q to Quit", Vector2((mScreenWidth / 2) - 100, 600), Vector3(1, 1, 1)
	);
}

void Game::Instruct()
{
	TextRenderer::SetFont("cod");
	TextRenderer::DrawString
	(
		L"Mark of Duty: P4G Ops", Vector2((mScreenWidth / 2) - 325, 0), Vector3(1, 1, 1)
	);
	TextRenderer::SetFont("LatoRegular16");
	TextRenderer::DrawString
	(
		L"Press the A key to move left and the D key to move right.", Vector2((mScreenWidth / 2) - 250, 200), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Avoid the shooting missiles for as long as you can.", Vector2((mScreenWidth / 2) - 210, 230), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"The longer the time survived, the higher the score.", Vector2((mScreenWidth / 2) - 210, 260), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press Space to Start", Vector2((mScreenWidth / 2) - 100, 340), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press B to return to Menu", Vector2((mScreenWidth / 2) - 100, 370), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press Q to Quit", Vector2((mScreenWidth / 2) - 100, 400), Vector3(1, 1, 1)
	);
}

void Game::MainGame()
{
	mSkybox.GetPosition() = mCamera.GetPos();
	FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);
	//render all the solid models first in no particular order
	for (Model* p : mOpaques)
	{
		if (!p->active)
			continue;
		FX::GetMyFX()->Render(*p, gd3dImmediateContext);
	}

	TextRenderer::SetFont("LatoRegular16");
	TextRenderer::DrawString
	(
		L"TIME SURVIVED: " + to_wstring(gUserStats.timeSurvived), Vector2(0, 0), Vector3(1, 1, 1)
	);

	TextRenderer::DrawString
	(
		L"LIVES: " + to_wstring(gUserStats.lives), Vector2(0, 30), Vector3(1, 1, 1)
	);
}

void Game::DeathScreen()
{
	TextRenderer::SetFont("cod");
	TextRenderer::DrawString
	(
		L"YOU DIED", Vector2((mScreenWidth / 2) - 150, 0), Vector3(1, 1, 1)
	);
	TextRenderer::SetFont("LatoRegular16");
	TextRenderer::DrawString
	(
		L"TIME SURVIVED: " + to_wstring(gUserStats.timeSurvived), Vector2(mScreenWidth/2 - 150, 100), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"HIGHSCORE: " + to_wstring(highScore), Vector2(mScreenWidth / 2 - 150, 130), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press Space to Replay", Vector2((mScreenWidth / 2) - 120, 540), Vector3(1, 1, 1)
	);
	TextRenderer::DrawString
	(
		L"Press Q to Quit", Vector2((mScreenWidth / 2) - 120, 570), Vector3(1, 1, 1)
	);
}
