#define RAYGUI_IMPLEMENTATION

#include <format>

#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "Test.h"
#include "UIElements/UIButton.h"
#include "UIElements/UIDropZone.h"
#include "UIElements/UIImageDropZone.h"
#include "UIElements/UISlider.h"

const Color BG_COLOR = {44,44,44,255};
const int FPS = 60;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "SecretFile";
const char* RESOURCE_DIR = "resources";
const char* GUI_STYLE_PATH = "styles/jungle/style_jungle.rgs";

UIButton* testButton;
UIImageDropZone* dropZone;
UISlider* bitSlider;

void OnTestButtonClick()
{
	std::cout << "OnTestButtonClick" << std::endl;
	Test::RunAllTests();
}

void OnImageDropped(Texture2D* img)
{
	std::cout << "OnImageDropped" << std::endl;
}

void OnBitDepthChanged(float bitDepth)
{
	std::cout << "OnBitDepthChanged:" << bitDepth << std::endl;
}

void Init()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
	SearchAndSetResourceDir(RESOURCE_DIR);
	GuiLoadStyle(GUI_STYLE_PATH);
	SetTargetFPS(FPS);

	testButton = new UIButton();
	testButton->transform.SetPosition({1170,10});
	testButton->transform.SetSize({100, 25});
	testButton->label = "Run All Tests";
	testButton->onClick.Add(OnTestButtonClick);

	dropZone = new UIImageDropZone();
	dropZone->transform.SetPivot({0.,0.});
	dropZone->transform.SetPosition({10,10});
	dropZone->transform.SetSize({300,200});
	dropZone->onImageDropped.Add(OnImageDropped);

	bitSlider = new UISlider();
	bitSlider->transform.SetPosition({100,220});
	bitSlider->transform.SetSize({200,25});
	bitSlider->min = 0;
	bitSlider->max = 8;
	bitSlider->value=1;
	bitSlider->intValues=true;
	bitSlider->label = "Bit Depth";
	bitSlider->onValueChanged.Add(OnBitDepthChanged);
}

void Update()
{
	BeginDrawing();
	ClearBackground(BG_COLOR);

	testButton->Draw();
	dropZone->Draw();
	bitSlider->Draw();
	
	EndDrawing();
}

int main ()
{
	Init();
	
	while (!WindowShouldClose())
	{
		Update();
	}

	delete dropZone;
	
	CloseWindow();
	
	return 0;
}
