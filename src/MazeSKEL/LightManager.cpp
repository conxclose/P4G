// by Mark Featherstone (C) 2013 All Rights Reserved.
#include <algorithm>
#include "d3d.h"

#include "LightManager.h"
#include "FX.h"
#include "Error.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

unsigned int LightManager::sHandleGenerator=0;




unsigned int LightManager::CachePointLight(char priority)
{
	//if a light is free then great, use it
	//if there is no free light, maybe we have high enough priority
	//that we can steal one off someone?
	Data *pSteal=NULL;
	for(CachedLights::iterator it=mLights.begin();it!=mLights.end();++it)
	{
		if((*it).hdl!=UDEF::MAX_UINT)
		{
			if(!pSteal && priority>(*it).priority)
				pSteal = &(*it);
			continue;
		}
		(*it).hdl=sHandleGenerator++;
		(*it).priority=priority;
		return (*it).hdl; 
	}
	if(pSteal)
	{
		pSteal->hdl=sHandleGenerator++;
		pSteal->priority=priority;
		return pSteal->hdl;
	}
	return UDEF::MAX_UINT;
}

void LightManager::ReleaseCachePointLight( unsigned int& hdl )
{
	//if we don't let go of lights properly we'll soon run out
	//there are only 8!
	if(hdl==UDEF::MAX_UINT)
		return;
	//C++11 code - return the data item with a matching "hdl"
	CachedLights::iterator it=std::find_if(mLights.begin(),mLights.end(), 
		[hdl](const Data &d) { return d.hdl == hdl; });
	hdl = UDEF::MAX_UINT;
	if(it==mLights.end())
		return;
	MPOD_ASSERT((*it).hdl!=UDEF::MAX_UINT);
	FX::DisableLight((*it).lightId);
	(*it).hdl=UDEF::MAX_UINT;
}

bool LightManager::GetLightId(unsigned int& hdl, int &lightId)
{
	if(hdl==UDEF::MAX_UINT)
		return false;

	//C++11 code - return the data item with a matching "hdl"
	CachedLights::iterator it=std::find_if(mLights.begin(),mLights.end(), 
		[hdl](const Data &d) { return d.hdl == hdl; });

	if (it == mLights.end() || (*it).hdl == UDEF::MAX_UINT)
	{
		hdl = UDEF::MAX_UINT;
		return false;
	}
	lightId=(*it).lightId;
	return true;
}

LightManager::LightManager()
{
	//there are 8 hardware lights, we'll manage 7, leave the first for general use 
	mLights.insert(mLights.begin(),7,Data());
	int count(1);
	for(CachedLights::iterator it=mLights.begin(); it!=mLights.end(); ++it, ++count)
		(*it).lightId=count;
}
