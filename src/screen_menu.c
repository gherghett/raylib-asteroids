#include "raylib.h"
#include "screen_manager.h"
int hello = 0;
static char *title = "ASTEROIDS";
static char *instructions = "Press ENTER to start \n arrow-keys + space to shoot";
void ScreenMenu_Init(void) {
    hello++;
}
void ScreenMenu_Update(void) {
    if(IsKeyPressed(KEY_ENTER)) {
        ChangeToScreen(SCREEN_GAMEPLAY);
    }
}
void ScreenMenu_Draw(void) {
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText(title, 0,0, 60, RED);

        DrawText(instructions, 100,100, 30, RED);

    
    EndDrawing();
}
void ScreenMenu_Unload(void) {
    hello = 0;
}

