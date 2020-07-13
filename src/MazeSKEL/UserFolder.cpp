// by Mark Featherstone (C) 2013 All Rights Reserved.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "shlobj.h"

#include "UserFolder.h"
#include "File.h"
#include "UtilsDefines.h"
#include "Error.h"

using namespace std;
using namespace UDEF;

namespace USER
{

static utf8string s_userDataPath;	
static utf16string s_userDataPathUTF16;

bool TestCreateFile( utf8string &fileName )
{
	//create a test file and write into it
	File fileO( fileName, File::MPF_WRITE);

	int num = 1;
	if( !File::write( fileO, num ) )
	{
		fileO.close();
		return false;
	}
	if( !fileO.close() )
		return false;

	//read it back
	File fileI( fileName, File::MPF_READ );

	num = 0;
	if( !File::read( fileI, num ) )
	{
		fileI.close();
		return false;
	}
	if( num != 1 )
		return false;
	if( !fileI.close() )
		return false;

	//erase it
	if( !File::remove( fileName ) )
		return false;

	return true;
}

/*
User permissions in windows can stop us writing to certain locations
so we'll actually try and write a small file to a couple of different places
and see what works
*/
bool GetUserFolder( utf8string &userFolder)
{
	vector<utf16char> myBuffer;
	myBuffer.insert( myBuffer.begin(), MAX_PATH, 0 );
	static bool s_pathTest = false;
	static bool s_useExeDirectory = false;		 
	utf8string tmp;

	if( s_pathTest )
	{
		if( s_useExeDirectory )
		{
			userFolder = File::getFirstRunDirectory();
		}
		else
		{
			MPOD_VERIFY( SUCCEEDED( SHGetFolderPathW( NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, &myBuffer[0] ) ) );
			convertUTF16toUTF8( userFolder, myBuffer );
		}
		return true;				  
	}	

	s_pathTest = true;

	//can we find the user folder
	bool userFolderOk = SUCCEEDED( SHGetFolderPathW( NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE, NULL, 0, &myBuffer[0] ) );
	if( userFolderOk )
	{
		//try to actually write into this so-called mydocuments
		convertUTF16toUTF8( tmp, myBuffer );
		tmp += "\\test.txt";
		if( TestCreateFile( tmp ) )
			convertUTF16toUTF8( userFolder, myBuffer );
		else
			userFolderOk = false;
	}	

	if( !userFolderOk )
	{
		//try and use the exe folder directly
		userFolder = File::getFirstRunDirectory();
		tmp = userFolder + "\\test.txt";
		if( !TestCreateFile( tmp ) )
		{
			MPOD_VERIFY_MSG( 0, "Error: I cannot find your mydocuments folder, this could be because someone has altered the configuration of windows in an unexpected way." );
			return false;
		}
		s_useExeDirectory = true;
	}

	return true;
}


void SetUserDataFolder( const utf8string &dataFolder )
{
	MPOD_VERIFY( GetUserFolder( s_userDataPath ) );
	s_userDataPath += "\\";
	s_userDataPath += dataFolder;
	if( !File::fileExists( s_userDataPath ) )
		File::createFolder( s_userDataPath );
	convertUTF8toUTF16( s_userDataPathUTF16, s_userDataPath );
}

const utf16string &GetUserDataFolder()
{
	MPOD_ASSERT( !s_userDataPathUTF16.empty() );
	return s_userDataPathUTF16;
}

const utf8string &GetUserDataFolderUTF8()
{
	MPOD_ASSERT( !s_userDataPath.empty() );
	return s_userDataPath;
}

}