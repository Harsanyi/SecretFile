#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raygui.h"
#include "resource_dir.h"
#include "SecFileUI.h"
#include <exception>

int main()
{
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 720, "SecretFile");
    InitAudioDevice();
    if (!SearchAndSetResourceDir("resources"))
    {
        TraceLog(LOG_ERROR, "Runtime resources directory was not found");
        CloseAudioDevice();
        CloseWindow();
        return 1;
    }
    GuiLoadStyle("styles/jungle/style_jungle.rgs");
    Font font = GetFontDefault(); // Owned and released by raylib's window context.
    GuiSetFont(font);
    Shader shader = LoadShader("shaders/red.vs", "shaders/red.fs");
    SetTargetFPS(60);
    int result = 0;
    try
    {
        SecFile file;
        SecFileUI ui(&file, {1280, 720});
        ui.SetFont(&font);
        ui.SetShader(&shader);
        while (!WindowShouldClose())
        {
            if (file.loadedModel) ui.UpdateModelTexture();
            BeginDrawing();
            ClearBackground({44, 44, 44, 255});
            ui.Draw();
            EndDrawing();
        }
    }
    catch (const std::exception& error)
    {
        TraceLog(LOG_WARNING, "%s", error.what());
        result = 1;
    }
    UnloadShader(shader);
    CloseAudioDevice();
    CloseWindow();
    return result;
}
