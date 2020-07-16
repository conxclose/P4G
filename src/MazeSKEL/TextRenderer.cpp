#include "TextRenderer.h"

void TextRenderer::Initialise()
{
	GetInstance().mpSpriteBatch = new SpriteBatch(gd3dImmediateContext);
	assert(GetInstance().mpSpriteBatch);

	//Load all fonts
	LoadFont("LatoRegular16", "../bin/data/latoRegular16.spritefont");
	LoadFont("cod", "../bin/data/callofopsduty.spritefont");
}

void TextRenderer::Release()
{
	delete GetInstance().mpSpriteBatch;
	GetInstance().mpSpriteBatch = nullptr;
	
	GetInstance().ReleaseFonts();
}

void TextRenderer::ReleaseFonts()
{
	vector<SpriteFont*> ftd;
	for (auto const& it : fonts) {
		cout << "Unloading font " << it.first << endl;
		ftd.push_back(it.second);
	}
	fonts.clear();
	while (!ftd.empty()) {
		SpriteFont* sf = ftd.back();
		ftd.pop_back();
		delete sf;
	}
	cout << "End release" << endl;
}

void TextRenderer::LoadFont(string name, string path)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	wstring widePath = converter.from_bytes(path);

	SpriteFont* sf = new SpriteFont(gd3dDevice, widePath.c_str());
	assert(sf);

	GetInstance().fonts[name] = sf;
	cout << "Loaded font " << path << " as " << name << endl; //Only on debug mode
}

void TextRenderer::SetFont(string name)
{
	//Check if the font exists
	map<string, SpriteFont*>::iterator it = GetInstance().fonts.find(name);
	assert(it != GetInstance().fonts.end());

	//Then set
	GetInstance().activeFont = name;
}

void TextRenderer::ResetFont()
{
	GetInstance().activeFont = GetInstance().defaultFont;
}

void TextRenderer::DrawString(wstring string, Vector2 position, Vector3 colour)
{
	DrawString(string, position, colour, Vector2(0, 0), Vector2(1, 1));
}

void TextRenderer::DrawString(wstring string, Vector2 position, Vector3 colour, Vector2 origin, Vector2 scale)
{
	GetInstance().mpSpriteBatch->Begin();
	GetInstance().fonts[GetInstance().activeFont]->DrawString(GetInstance().mpSpriteBatch, string.c_str(), position, colour, 0, origin, scale);
	GetInstance().mpSpriteBatch->End();
}

TextRenderer & TextRenderer::GetInstance()
{
	static TextRenderer instance;
	return instance;
}
