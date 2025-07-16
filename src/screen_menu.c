#include "raylib.h"
#include "screen_manager.h"
int hello = 0;
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
    
    EndDrawing();
}
void ScreenMenu_Unload(void) {
    hello = 0;
}

