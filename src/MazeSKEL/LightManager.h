// by Mark Featherstone (C) 2013 All Rights Reserved.
#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <vector>

#include "d3d.h"
#include "UtilsDefines.h"

/*
We only have 8 lights so they need to be carefully shared out 
We'll ignore light0 and leave that as a general purpose one
that isn't managed
*/
class LightManager
{
public:
	LightManager();

	//request and release lights, something with a higher priority than us
	//can pinch it without us knowing, hence we only hang on to a handle
	unsigned int CachePointLight(char priority);
	void ReleaseCachePointLight(unsigned int& hdl);
	//we have a light handle, can we have the actual D3D light ID it refers to?
	bool GetLightId(unsigned int& hdl, int &lightId);

private:

	//for each light we use (8) track who currently has it
	struct Data
	{
		Data() : hdl(UDEF::MAX_UINT), priority(-1), lightId(0) {}
		unsigned int hdl;
		char priority;
		unsigned short lightId;
	};
	//array of lights
	typedef std::vector<Data> CachedLights;
	CachedLights mLights;
	//put the parameters in here when configuring a light
	//D3DLIGHT9 mLight;
	//just counts up to ensure every new handle is unique
	static unsigned int sHandleGenerator;
};


#endif