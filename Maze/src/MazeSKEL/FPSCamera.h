#ifndef FPSCAMERA_H
#define FPSCAMERA_H

#include "d3d11.h"
#include "SimpleMath.h"
/*
First person camera, forwards backwards left right and rotate
*/
class FPSCamera
{
public:
	enum { UNLOCK = 999999 };

	//set initial camera position and the matrix to modify
	void Initialise(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& tgt, DirectX::SimpleMath::Matrix& viewSpaceTfm);

	//movement can be driven by WASD
	void Move(float dTime, bool forward, bool back, bool left, bool right);
	//rotation can be driven by mouse x=yaw and y=pitch
	void Rotate(float dTime, float _yaw, float _pitch, float _roll);
	//stop camera moving in a certain axis, e.g. a FPS camera that always stays a fixed height in Y
	void LockMovementAxis(float x = UNLOCK, float y = UNLOCK, float z = UNLOCK) {
		mLockAxis = DirectX::SimpleMath::Vector3(x, y, z);
	}
	const DirectX::SimpleMath::Vector3& GetPos() {
		return mCamPos;
	}

private:
	DirectX::SimpleMath::Vector3 mLockAxis{ UNLOCK,UNLOCK,UNLOCK };
	//the camera matrix to modify
	DirectX::SimpleMath::Matrix* mpViewSpaceTfm;
	//movement and rotation speed
	float speed = 10.f, rspeed = 10.f;
	//camera position
	DirectX::SimpleMath::Vector3 mCamPos;
	//camera rotation
	float yaw = 0, pitch = 0, roll = 0;
};


#endif
