// by Mark Featherstone (C) 2013 All Rights Reserved.
#pragma once 

#include <string>
#include "UtilsDefines.h"

//you cannot save anywhere you like, in windows game saves have to go in a specific folder
namespace USER
{

// under Win32, returns the actual location of "My Documents" or "appdata"
bool GetUserFolder( UDEF::utf8string &userFolder );
		
//user data will always go in <mydocs><somefolder> so set that <somefolder> up here
void SetUserDataFolder( const UDEF::utf8string &dataFolder);
//some people use foreign characters in their names so you need unicode
//strings to hold the path
const UDEF::utf16string &GetUserDataFolder();	
const UDEF::utf8string &GetUserDataFolderUTF8();
}

