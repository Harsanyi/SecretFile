#define RAYGUI_IMPLEMENTATION

#include <format>

#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "Test.h"
#include "UIElements/UIButton.h"
#include "UIElements/UIContainer.h"
#include "UIElements/UIDropZone.h"
#include "UIElements/UIImageDropZone.h"
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
UIImageDropZone* dropZone;
UISlider* bitSlider;
Texture2D* originalImage;
Texture2D* previewImage;
UIButton* saveButton;
UIText* sizeText;
UIButton* openButton;
Container* content;
UIContainer* containerUI;

void OnTestButtonClick()
{
	std::cout << "OnTestButtonClick" << std::endl;
	Test::RunAllTests();
}

void SetImagePreviewToDepth(uint8_t depth)
{
	if (depth <= 0)
	{
		dropZone->SetTexture(originalImage);
		return;
	}
	
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
	dropZone->SetTexture(previewImage);
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

	saveButton = new UIButton();
	saveButton->transform.SetPosition({10,255});
	saveButton->transform.SetSize({100,25});
	saveButton->label = "Save Image";
	saveButton->onClick.Add(OnSaveButtonClick);

	sizeText = new UIText();
	sizeText->transform.SetPosition({10,290});
	sizeText->transform.SetSize({200,25});
	sizeText->text = "Container size: ?";

	openButton = new UIButton();
	openButton->transform.SetPosition({10,325});
	openButton->transform.SetSize({100,25});
	openButton->label = "Open";
	openButton->onClick.Add(OnOpenButtonClick);

	containerUI = new UIContainer();
	containerUI->transform.SetPosition({410.,10});
	containerUI->transform.SetSize({600,600});
}

void Update()
{
	BeginDrawing();
	ClearBackground(BG_COLOR);

	UIElement::DrawAll();
		
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
