#ifndef TextRenderer_H
#define TextRenderer_H

#include <map>
#include <vector>
#include <iostream>

#include <locale>
#include <codecvt>
#include <string>

#include "D3D.h"
#include "SpriteFont.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;

class TextRenderer
{
public:

	/*
		Initialise the TextRenderer first to begin setup.
	*/
	static void Initialise();
	
	/*
		Delete all resources held by the TextRenderer
	*/
	static void Release();

	/*
		Iterate through the fonts list to delete the resources from the heap before clearing 
	*/
	void ReleaseFonts();

	/*
		Load a new font into the system
		name = the name to reference it by
		path = the path to the spritefont file
	*/
	static void LoadFont(string name, string path);

	/*
		Set the active font by it's loaded name
	*/
	static void SetFont(string name);

	/*
		Reset the font back to default after changing it
	*/
	static void ResetFont();

	/*
		Draw a string on to the screen
		string = the string to draw
		position = the position on the screen
		colour = the colour of the text in RGB
	*/
	static void DrawString(wstring string, Vector2 position, Vector3 colour);

	/*
		Draw a string on to the screen
		string = the string to draw
		position = the position on the screen
		colour = the colour of the text in RGB
		origin = the origin of the text
		scale = the scale of the text
	*/
	static void DrawString(wstring string, Vector2 position, Vector3 colour, Vector2 origin, Vector2 scale);

private:
	/*
		private variables for a static class 
	*/
	static TextRenderer& GetInstance();

	/*
		The active font for the renderer
	*/
	string activeFont = "LatoRegular16";

	/*
		The default font for the renderer
	*/
	const string defaultFont = "LatoRegular16";

	/*
		The map of all available fonts
	*/
	map<string, SpriteFont*> fonts;

	/*
		The spritebatch to draw to
	*/
	SpriteBatch* mpSpriteBatch = nullptr;
};

#endif
