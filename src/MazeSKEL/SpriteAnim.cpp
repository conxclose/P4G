#include "SpriteAnim.h"
#include "D3D.h"
#include "D3DUtil.h"
#include "FX.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

void SpriteAnim::Init(const Vector2& origin, float rotation, float scale, float depth)
{
	mOrigin = origin;
	mRotation = rotation;
	mScale = Vector2(scale, scale);
	mDepth = depth;
	mNumPlays = 0;
	mFrame = 0;
}

void SpriteAnim::Load(ID3D11ShaderResourceView* texture, int frameCount, const Vector2& frameWH, int framesPerSecond)
{
	if (frameCount <= 0 || framesPerSecond <= 0 || frameWH.x <= 0 || frameWH.y <= 0)
		DBOUT("bad argument");

	mPaused = true;
	mFrameCount = frameCount;
	mFrameWH = frameWH;
	mTimePerFrame = 1.f / float(framesPerSecond);
	mTotalElapsed = 0.f;
	mTexture = texture;

	assert(texture);
	FX::MyFX& fx = *FX::GetMyFX();
	const Vector2 &dim = fx.mCache.Get(texture).dim;
	mTextureWidth = (int)dim.x;
	mTextureHeight = (int)dim.y;
}

void SpriteAnim::Update(float elapsed)
{
	if (mPaused)
		return;

	mTotalElapsed += elapsed;

	if (mTotalElapsed > mTimePerFrame)
	{
		++mFrame;
		if (mFrame >= mFrameCount)
			++mNumPlays;
		mFrame = mFrame % mFrameCount;
		mTotalElapsed -= mTimePerFrame;
	}
}

void SpriteAnim::Draw(DirectX::SpriteBatch* batch, int frame, const Vector2& screenPos) const
{
	int maxFramesX = (int)((float)mTextureWidth / mFrameWH.x);

	RECT sourceRect;
	sourceRect.left = (int)(mFrameWH.x * (frame%maxFramesX));
	sourceRect.top = (int)(mFrameWH.y * (frame / maxFramesX));
	sourceRect.right = (int)(sourceRect.left + mFrameWH.x);
	sourceRect.bottom = (int)(sourceRect.top + mFrameWH.y);

	batch->Draw(mTexture, screenPos, &sourceRect, DirectX::Colors::White, mRotation, mOrigin, mScale, DirectX::SpriteEffects_None, mDepth);
}

void SpriteAnim::Draw(DirectX::SpriteBatch* batch, const RECT& dest, const Vector2& srcScale) const
{
	int maxFramesX = (int)((float)mTextureWidth / mFrameWH.x);

	RECT sourceRect;
	sourceRect.left = (int)(mFrameWH.x * (mFrame%maxFramesX));
	sourceRect.top = (int)(mFrameWH.y * (mFrame / maxFramesX));
	sourceRect.right = (int)((sourceRect.left + mFrameWH.x) * srcScale.x);
	sourceRect.bottom = (int)((sourceRect.top + mFrameWH.y) * srcScale.y);

	batch->Draw(mTexture, dest, &sourceRect, DirectX::Colors::White, mRotation, mOrigin);

}
