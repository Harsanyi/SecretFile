#define RAYGUI_IMPLEMENTATION

#include <format>

#include "BitStream.h"
#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "SecFile.h"
#include "Test.h"

const Color BG_COLOR = {44,44,44,255};
const int FPS = 60;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "SecretFile";
const char* RESOURCE_DIR = "resources";
const char* GUI_STYLE_PATH = "styles/jungle/style_jungle.rgs";

Image origImg;
Image newImg;
Texture2D tex;
float noise = 0;

void Init()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SearchAndSetResourceDir(RESOURCE_DIR);
	GuiLoadStyle(GUI_STYLE_PATH);
	SetTargetFPS(FPS);

	origImg = LoadImage("wallp.jpg");
	ImageFormat(&origImg, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
	newImg = ImageCopy(origImg);
}

void Update()
{
	BeginDrawing();
	ClearBackground(BG_COLOR);

	UnloadTexture(tex);
	tex = LoadTextureFromImage(newImg);
	DrawTexturePro(tex,
		Rectangle{0,0, (float)(tex.width), (float)(tex.height)},
		Rectangle{0,0,SCREEN_WIDTH,SCREEN_HEIGHT},
		Vector2{0,0},
		0, WHITE);
	
	DrawFPS(10,10);
	
	auto valueText = std::format("{:.0f} %", noise);
	float newNoise = noise;
	GuiSlider(Rectangle{300,10,200,20}, "Noise", valueText.c_str(), &newNoise, 0,100);
	if (newNoise != noise)
	{
		Test::RunAllTests();
	}
	
	EndDrawing();
}

int main ()
{
	Init();
	
	while (!WindowShouldClose())
	{
		Update();
	}
	
	CloseWindow();
	
	return 0;
}
