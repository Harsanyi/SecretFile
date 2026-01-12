#define RAYGUI_IMPLEMENTATION

#include <format>

#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "Test.h"
#include "UIElements/UIButton.h"
#include "UIElements/UIDropZone.h"

const Color BG_COLOR = {44,44,44,255};
const int FPS = 60;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "SecretFile";
const char* RESOURCE_DIR = "resources";
const char* GUI_STYLE_PATH = "styles/jungle/style_jungle.rgs";

UIButton testButton;
UIDropZone dropZone;

void OnTestButtonClick()
{
	std::cout << "OnTestButtonClick" << std::endl;
	Test::RunAllTests();
}

void OnFileDropped(std::string path)
{
	std::cout << "OnFileDropped:" << path << std::endl;
}

void Init()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SearchAndSetResourceDir(RESOURCE_DIR);
	GuiLoadStyle(GUI_STYLE_PATH);
	SetTargetFPS(FPS);

	testButton = UIButton();
	testButton.transform.position = {1170,10};
	testButton.transform.size = {100, 25};
	testButton.label = "Run All Tests";
	testButton.onClick.Add(OnTestButtonClick);

	dropZone = UIDropZone();
	dropZone.transform.position = {10,10};
	dropZone.transform.size = {300,200};
	dropZone.onFileDropped.Add(OnFileDropped);
}

void Update()
{
	BeginDrawing();
	ClearBackground(BG_COLOR);

	testButton.Draw();
	dropZone.Draw();
	
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
