#include "raylib.h"
#include <malloc.h>
#include <math.h>
// #include "screen_gameplay.h"
// #include "screen_menu.h"
#include "screen_manager.h"


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{

    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitAudioDevice();      // Initialize audio device

    InitWindow(screenWidth, screenHeight, "raylib ASTEROIDS");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    ChangeToScreen(SCREEN_MENU);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        ProcessScreenChange(); // Inits new Sceen and unloads

        // Update
        Update();

        // Draw
        Draw();
    }

    Unload();

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    CloseAudioDevice();     // Close audio device
    //UnloadSound(soundArray[0]); 
    //--------------------------------------------------------------------------------------

    return 0;
}