#define RAYGUI_IMPLEMENTATION

#include <format>

#include "BitStream.h"
#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "SecFile.h"
#include "Test.h"
#include "UIElements/UIButton.h"

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

UIButton testButton;

void OnTestButtonClick()
{
	std::cout << "OnTestButtonClick" << std::endl;
	Test::RunAllTests();
}

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

	testButton.label = (char*)"Test Button";
	testButton.onClick.Add(OnTestButtonClick);
}

void Update()
{
	BeginDrawing();
	ClearBackground(BG_COLOR);

	testButton.Draw();
	
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
