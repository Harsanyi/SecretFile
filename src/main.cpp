#define RAYGUI_IMPLEMENTATION

#include <format>
#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "Test.h"
#include "SecFileUI.h"

const Color BG_COLOR = {44,44,44,255};
const int FPS = 60;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "SecretFile";
const char* RESOURCE_DIR = "resources";
const char* GUI_STYLE_PATH = "styles/jungle/style_jungle.rgs";

SecFile secFile;
SecFileUI ui(&secFile, {SCREEN_WIDTH, SCREEN_HEIGHT});

void Init()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SearchAndSetResourceDir(RESOURCE_DIR);
	GuiLoadStyle(GUI_STYLE_PATH);
	SetTargetFPS(FPS);
}

void Update()
{
	BeginDrawing();
	ClearBackground(BG_COLOR);

	ui.Draw();
		
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
