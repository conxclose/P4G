#ifndef SPRITEANIM_H
#define SPRITEANIM_H

#include "d3d.h"
#include "SpriteBatch.h"

class SpriteAnim
{
public:
	SpriteAnim()
	{
		Init(DirectX::SimpleMath::Vector2(0, 0), 0, 1, 0);
	}

	SpriteAnim(const DirectX::SimpleMath::Vector2& origin, float rotation, float scale, float depth)
	{
		Init(origin, rotation, scale, depth);
	}
	void Init(const DirectX::SimpleMath::Vector2& origin, float rotation, float scale, float depth);
	void Load(ID3D11ShaderResourceView* texture, int frameCount, const DirectX::SimpleMath::Vector2& frameWH, int framesPerSecond);

	void Update(float elapsed);

	void Draw(DirectX::SpriteBatch* batch, const RECT& dest, const DirectX::SimpleMath::Vector2& srcScale) const;
	void Draw(DirectX::SpriteBatch* batch, const DirectX::SimpleMath::Vector2& screenPos) const
	{
		Draw(batch, mFrame, screenPos);
	}

	void Draw(DirectX::SpriteBatch* batch, int frame, const DirectX::SimpleMath::Vector2& screenPos) const;


	void Reset()
	{
		mFrame = 0;
		mTotalElapsed = 0.f;
		mNumPlays = 0;
	}

	void Stop()
	{
		mPaused = true;
		mFrame = 0;
		mTotalElapsed = 0.f;
	}
	void Play() {
		mPaused = false;
	}
	void Pause() {
		mPaused = true;
	}



	int GetNumPlays() const {
		return mNumPlays;
	}
	bool IsPaused() const {
		return mPaused;
	}
	void SetRotation(float rot) {
		mRotation = rot;
	}
	void SetScale(const DirectX::SimpleMath::Vector2& scale) {
		mScale = scale;
	}
	const DirectX::SimpleMath::Vector2& GetScale() const {
		return mScale;
	}
	int GetNumFrames() const {
		return mFrameCount;
	}
	int GetFrameRate() const{
		return (int)(1.f/mTimePerFrame);
	}
	float GetTotalTime() const {
		return mTimePerFrame*mFrameCount;
	}

private:
	bool mPaused = true;
	float mRotation = 0;
	int mFrame = 0;
	int mFrameCount = 0;
	DirectX::SimpleMath::Vector2 mFrameWH{ 0, 0 };
	int mTextureWidth = 0;
	int mTextureHeight = 0;
	int mNumPlays = 0;
	float mTimePerFrame = 0;
	float mTotalElapsed = 0;
	float mDepth = 0;
	DirectX::SimpleMath::Vector2 mOrigin{ 0, 0 };
	DirectX::SimpleMath::Vector2 mScale{ 1, 1 };
	ID3D11ShaderResourceView* mTexture = nullptr;
};

#endif
