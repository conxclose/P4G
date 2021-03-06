#include "WindowUtils.h"
#include "D3D.h"
#include "FX.h"
#include "Mesh.h"
#include "Input.h"
#include "File.h"
#include "UserFolder.h"
#include "AudioMgrFMOD.h"
#include "Game.h"


using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

Game gGame;


void Update(float dTime)
{
	gGame.Update(dTime);
	GetIAudioMgr()->Update();
}

void Render(float dTime)
{
	gGame.Render(dTime);
}

void OnResize(int screenWidth, int screenHeight)
{
	gGame.OnResize(screenWidth, screenHeight);
}


LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_INPUT:
		GetMouseAndKeys()->MessageEvent((HRAWINPUT)lParam);
		break;
	}

	return gGame.WindowsMssgHandler(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	SeedRandom();
	File::initialiseSystem();
	USER::SetUserDataFolder("Mark of Duty");

	if (!InitMainWindow(1024, 768, hInstance, "Mark of Duty: P4G Ops", MainWndProc))
		assert(false);

	if (!InitDirect3D(OnResize))
		assert(false);

	//startup
	new FX::MyFX(gd3dDevice);
	new MeshManager;
	new MouseAndKeys;
	new AudioMgrFMOD;
	GetIAudioMgr()->Initialise();
	GetMouseAndKeys()->Initialise(GetMainWnd());
	gGame.Initialise();

	Run(Update, Render);

	//shut down
	gGame.Release();
	delete GetIAudioMgr();
	delete GetMouseAndKeys();
	delete GetMeshManager();
	delete FX::GetMyFX();

	ReleaseD3D();

	return 0;
}
