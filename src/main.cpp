#define RAYGUI_IMPLEMENTATION

#include <format>
#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "Test.h"
#include "SecFileUI.h"
#include "UIElements/UIButton.h"
#include "UIElements/UIContainer.h"
#include "UIElements/UIDropZone.h"
#include "UIElements/UISlider.h"
#include "UIElements/UIText.h"

const Color BG_COLOR = {44,44,44,255};
const int FPS = 60;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const char* WINDOW_TITLE = "SecretFile";
const char* RESOURCE_DIR = "resources";
const char* GUI_STYLE_PATH = "styles/jungle/style_jungle.rgs";

UIButton* testButton;
UISlider* bitSlider;
Texture2D* originalImage;
Texture2D* previewImage;
UIButton* saveButton;
UIText* sizeText;
UIButton* openButton;
Container* content;
UIContainer* containerUI;

SecFile secFile;
SecFileUI ui(&secFile, {SCREEN_WIDTH, SCREEN_HEIGHT});

void OnTestButtonClick()
{
	std::cout << "OnTestButtonClick" << std::endl;
	Test::RunAllTests();
}

void SetImagePreviewToDepth(uint8_t depth)
{
	
	long dataSize = SecFile::GetSizeInBytes(originalImage, depth);
	dataSize -= 100;
	std::vector<uint8_t> data = Random::GetBytes(dataSize);
	if (previewImage != nullptr)
	{
		UnloadTexture(*previewImage);
		delete previewImage;
	}
	previewImage = new Texture2D(*originalImage);
	SecFile::WriteDataToTexture(*previewImage, data, depth);
}

void UpdateSizeText()
{
	long size = SecFile::GetSizeInBytes(originalImage, (int)bitSlider->value);
	sizeText->text = std::format("Container size:{0}", size);
}

void OnBitDepthChanged(float bitDepth)
{
	std::cout << "OnBitDepthChanged:" << bitDepth << std::endl;
	if (originalImage == nullptr) return;
	int bits = std::round(bitDepth);
	SetImagePreviewToDepth(bits);
	UpdateSizeText();
}

void OnImageDropped(Texture2D* img)
{
	std::cout << "OnImageDropped" << std::endl;
	if (originalImage != nullptr)
	{
		UnloadTexture(*originalImage);
		delete originalImage;
	}
	originalImage = img;

	OnBitDepthChanged(bitSlider->value);
}

void OnSaveButtonClick()
{
	std::cout << "OnSaveButtonClick" << std::endl;
	if (previewImage == nullptr) return;

	Image img = LoadImageFromTexture(*previewImage);
	ExportImage(img, "saveImg.png");
	UnloadImage(img);
	std::cout << "Image Saved!" << std::endl;
}

void OnOpenButtonClick()
{
	std::cout << "OnOpenButtonClick" << std::endl;
	if (originalImage == nullptr) return;

	uint8_t bitDepth = bitSlider->value;
	if (content != nullptr)delete content;
	content = SecFile::GetContentFromTexture(*originalImage, bitDepth);
	if (content == nullptr)content = new Container();
}

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
