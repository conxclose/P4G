#include <algorithm>

#include "WindowUtils.h"
#include "D3D.h"
#include "Game.h"
#include "GeometryBuilder.h"
#include "FX.h"
#include "Input.h"
#include <ctime>

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

//Vector3 gWorldScale(10, 10, 10);


void Game::OnResize(int screenWidth, int screenHeight)
{
	OnResize_Default(screenWidth, screenHeight);
}

void Game::Initialise()
{
	std::srand(std::time(nullptr));
	/*mQuad.Initialise(BuildQuad(*GetMeshManager()));*/

	//textured lit box
	/*mBox.Initialise(BuildCube(*GetMeshManager()));
	mBox.GetPosition() = Vector3(0, -0.5f, 1);
	mBox.GetScale() = Vector3(0.5f, 0.5f, 0.5f);
	MaterialExt mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.5, 0.5, 0.5, 1), Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("test.dds", true, gd3dDevice);
	mat.texture = "test.dds";
	mBox.SetOverrideMat(&mat);*/

	//Projectile
	Mesh& pt = GetMeshManager()->CreateMesh("missile");
	pt.CreateFrom("data/missile.obj", gd3dDevice, FX::GetMyFX()->mCache);
	for(int i = 0; i < 25; i++)
	{
		Model* projectile = new Model;
		projectile->Initialise(*GetMeshManager()->GetMesh("missile"));
		projectile->GetRotation() = Vector3(PI, 0, 0);
		projectile->GetScale() = Vector3(.075, .075, .075);
		mOpaques.push_back(projectile);
		mProjectiles.push_back(projectile);
	}

	
	//mProjectile.Initialise(BuildQuad(*GetMeshManager()));
	
	//Plane
	// Create an empty mesh refrence and add it to the mesh manager
	Mesh& et = GetMeshManager()->CreateMesh("Plane");
	// Using mesh refrence assign mesh to the target object
	et.CreateFrom("data/plane.obj", gd3dDevice, FX::GetMyFX()->mCache);
	
	mPlane.Initialise(et);
	//mPlane.Initialise(*GetMeshManager()->GetMesh("quad"));
	//mPlane.Initialise(BuildQuad(*GetMeshManager()));
	mPlane.GetPosition() = Vector3(0,0,0);
	mPlane.GetRotation() = Vector3(0, PI, 0);
	mPlane.GetScale() = Vector3(.01, .01, .01);
	mPlane.active = true;
	MaterialExt mat = mPlane.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.5, 0.5, 0.5, 1), Vector4(1, 1, 1, 0), Vector4(1, 1, 1, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("plane.dds", true, gd3dDevice);
	mat.texture = "plane.dds";
	mPlane.SetOverrideMat(&mat);
	

	// floor
	/*mQuad.GetScale() = Vector3(3, 1, 3);
	mQuad.GetPosition() = Vector3(0, -1, 0);
	mat = mQuad.GetMesh().GetSubMesh(0).material;
	mat.gfxData.Set(Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 0), Vector4(0.9f, 0.8f, 0.8f, 1));
	mat.pTextureRV = FX::GetMyFX()->mCache.LoadTexture("test.dds", true, gd3dDevice);
	mat.texture = "test.dds";
	mat.texTrsfm.scale = Vector2(10, 10);
	mQuad.SetOverrideMat(&mat);*/

	Mesh& sb = GetMeshManager()->CreateMesh("skybox");
	sb.CreateFrom("data/skybox.fbx", gd3dDevice, FX::GetMyFX()->mCache);
	mSkybox.Initialise(sb);
	mSkybox.GetScale() = Vector3(1,1,1);
	mSkybox.GetPosition() = Vector3(0,0,0);
	mSkybox.GetRotation() = Vector3(PI/2,0,0);
	MaterialExt& defMat = mSkybox.GetMesh().GetSubMesh(0).material;
	defMat.flags &= ~MaterialExt::LIT;
	defMat.flags &= ~MaterialExt::ZTEST;

	//mOpaques.push_back(&mQuad);
	//mOpaques.push_back(&mBox);
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

void Game::Update(float dTime)
{
	spawnCountdown -= dTime;
	if (spawnCountdown <= 0)
	{
		spawnCountdown = spawnInterval;
		SpawnProjectiles();
	}
	MovePlane();
	MoveProjectiles(dTime);
	controllerPosition += controllerInput * dTime * moveSpeed;
	controllerPosition = Clip(controllerPosition, -1, 1);
	planePosition.x = MapNumber(-1, 1, -25, 25, controllerPosition);
	mPlane.GetPosition() = Vector3(planePosition.x, 0, 0);
}

void Game::MovePlane()
{
	if (GetMouseAndKeys()->IsPressed(VK_A)) {
		controllerInput = -1;
	}
	else if (GetMouseAndKeys()->IsPressed(VK_D)) {
		controllerInput = 1;
	}
	else
		controllerInput = 0;
}

void Game::SpawnProjectiles()
{
	float pos = (rand() % 50 + 1) - 26;

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
	for (int i = 0; i < mProjectiles.size(); i++)
	{
		if (mProjectiles[i]->active)
		{
			mProjectiles[i]->GetPosition() = mProjectiles[i]->GetPosition() - Vector3(0, 0, dTime * 25);
			mProjectiles[i]->GetRotation() = mProjectiles[i]->GetRotation() - Vector3(0, 0, dTime * 5);
			
			if(mProjectiles[i]->GetPosition().z <= 0 && !mProjectiles[i]->playerHit)
			{
				mProjectiles[i]->playerHit = true;
				if(abs(mProjectiles[i]->GetPosition().x - mPlane.GetPosition().x) < 8)
				{
					mProjectiles[i]->active = false;
					mProjectiles[i]->playerHit = false;
				}
			}

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

	float alpha = 0.5f + sinf(gAngle * 2)*0.5f;


	FX::SetPerFrameConsts(gd3dImmediateContext, mCamera.GetPos());

	//CreateViewMatrix(FX::GetViewMatrix(), mCamPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
	CreateProjectionMatrix(FX::GetProjectionMatrix(), 0.25f*PI, GetAspectRatio(), 1, 1000.f);
	Matrix w = Matrix::CreateRotationY(sinf(gAngle));
	FX::SetPerObjConsts(gd3dImmediateContext, w);

	mSkybox.GetPosition() = mCamera.GetPos();
	FX::GetMyFX()->Render(mSkybox, gd3dImmediateContext);
	//render all the solid models first in no particular order
	for (Model*p : mOpaques)
	{
		if(!p->active)
			continue;
		FX::GetMyFX()->Render(*p, gd3dImmediateContext);	
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

