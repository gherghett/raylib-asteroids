#include "raylib.h"
#include <malloc.h>
#include <math.h>

#define STARTING_ASTEROIDS 10
#define ASTEROID_START_SIZE 40
#define ASTEROID_MIN_SIZE 10
#define MAX_ASTEROIDS 150
#define POINTS_ARRAY_SIZE 1600

// int count_total_asteroids(int size) {
//     if (size <= ASTEROID_MIN_SIZE) return 0;
//     int children = 2;
//     int child_size = size / 2;
//     return children + children * count_total_asteroids(child_size);
// }

// int sum_total_sizes(int size) {
//     if (size < ASTEROID_MIN_SIZE) return 0;
//     int children = 2;
//     int child_size = size / 2;
//     return children * child_size + sum_total_sizes(child_size) * children;
// }

typedef struct {
    Vector2 *points;
    int size; // size of point array
    Vector2 position;
    Vector2 speed;
    float rotation;
    float rotationSpeedRadians;
    bool active;
} Asteroid;

typedef struct {
    Vector2 position;
    Vector2 direction;
    bool active;
} Bullet;

Vector2 AddVectors(Vector2 a, Vector2 b) {
    return (Vector2){
        a.x + b.x,
        a.y + b.y
    };
}

Vector2 RotatePoint(Vector2 point, Vector2 *pivot, float angle) {
    float s = sinf(angle);
    float c = cosf(angle);

    Vector2 zero = {0,0};
    if( pivot == NULL) {
        pivot = &zero;
    }

    // Translate point back to origin:
    point.x -= pivot->x;
    point.y -= pivot->y;

    // Rotate
    float xnew = point.x * c - point.y * s;
    float ynew = point.x * s + point.y * c;

    // Translate point back:
    point.x = xnew + pivot->x;
    point.y = ynew + pivot->y;

    return point;
}


Vector2 *MakeAsteroidShape(int points, int radius, Vector2 *loc) {
    const float full = 360.0;
    float wedge = full/points;
    Vector2 *relativePoints = loc;
    int radiusVariance = radius/4;
    int radiusMin = radius - radiusVariance;
    int radiusMax = radius + radiusVariance;
    for(int i = 0; i < points-1; i++) {
        float distance = GetRandomValue(radiusMin, radiusMax);

        float angleRad = (wedge * i) * DEG2RAD;  // raylib constant for degrees to radians
        Vector2 newPoint = {
            cosf(angleRad) * distance,
            sinf(angleRad) * distance
        };
        relativePoints[i] = newPoint;
    }
    relativePoints[points-1] = relativePoints[0];
    return relativePoints;
}

Vector2 GetVectorForAsteroidPoint(Vector2 *point, Vector2 *relativePos, float angleRad) {
    return (Vector2){
        ( point->x * cosf(angleRad) - point->y * sinf(angleRad) ) + relativePos->x,
        ( point->x * sinf(angleRad) + point->y * cosf(angleRad) ) + relativePos->y
    };
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    // int max_asteroids = STARTING_ASTEROIDS * (1 + count_total_asteroids(ASTEROID_START_SIZE));
    // int total_size = STARTING_ASTEROIDS * (ASTEROID_START_SIZE + sum_total_sizes(ASTEROID_START_SIZE));
    // if(max_asteroids > MAX_ASTEROIDS) {
    //     exit(1);
    // }


    const int playerSize = 20;
    // Rectangle player = {.x = screenWidth/2-playerSize/2, .y=screenHeight/2-playerSize/2, .width=playerSize, .height=playerSize};
    Vector2 playerPos = {.x = screenWidth/2-playerSize/2, .y=screenHeight/2-playerSize/2};
    Vector2 playerOrigin = {playerSize/2, .y=playerSize/2};
    float playerRotation = 0.0;
    const int playerAcceleration = 20.0f;
    Vector2 playerSpeed = {0};
    // Vector2 playerTriangle[3] = {
    //     (Vector2){-10, -10},
    //     (Vector2){10, 0},
    //     (Vector2){-10, 10},
    // };
    Vector2 playerTriangle[3] = {
        {-10, -10},
        {-10, 10},
        {10, 0},
    };

    Bullet bullets[100] = {};
    int bulletsLoc = 0;

    InitWindow(screenWidth, screenHeight, "raylib ASTEROIDS");

    Asteroid asteroids[MAX_ASTEROIDS] = {0}; 
    int asteroidLoc = 0; 
    Vector2 *asteroidPointsArea = malloc(sizeof(Vector2) * POINTS_ARRAY_SIZE);
    Vector2 *asteroidPointsLoc = asteroidPointsArea;
    for(int i = 0; i < STARTING_ASTEROIDS; i++) {
        int x = GetRandomValue(0, screenHeight);
        int y = GetRandomValue(0, screenWidth);
        int speedX = GetRandomValue(-20, 20);
        int speedY = GetRandomValue(-20, 20);
        Asteroid new_asteroid = {
            .points = MakeAsteroidShape(ASTEROID_START_SIZE, ASTEROID_START_SIZE, asteroidPointsLoc),
            .size = ASTEROID_START_SIZE,
            .position = (Vector2){x, y},
            .rotation = 0.0,
            .rotationSpeedRadians = (GetRandomValue(1, 40) - 20) / 360 * DEG2RAD, 
            .active = true,
            .speed = (Vector2){speedX, speedY}
        };
        asteroidPointsLoc += ASTEROID_START_SIZE;
        asteroids[asteroidLoc++] = new_asteroid;
    }

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        float delta = GetFrameTime();
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        
        if(IsKeyDown(KEY_LEFT)) playerRotation -= 360.0 * delta;
        if(IsKeyDown(KEY_RIGHT)) playerRotation += 360.0 * delta;
        
        float acceleration = 0.0f;
        if(IsKeyDown(KEY_UP)) acceleration += playerAcceleration;
        if(IsKeyDown(KEY_DOWN)) acceleration -= playerAcceleration;
        
        float playerRotRad = playerRotation * DEG2RAD;  // raylib constant for degrees to radians

        
        Vector2 deltaSpeed = {
            cosf(playerRotRad) * acceleration,
            sinf(playerRotRad) * acceleration
        };
        
        playerSpeed.x += deltaSpeed.x;
        playerSpeed.y += deltaSpeed.y;
        
        playerPos.x += playerSpeed.x * delta;
        playerPos.y += playerSpeed.y * delta;
        
        if(IsKeyPressed(KEY_SPACE)) {
            Bullet newBullet = {
                .position = playerPos,
                .direction = AddVectors(RotatePoint((Vector2){500, 0}, NULL, playerRotRad), playerSpeed),
                .active = true
            };
            bullets[bulletsLoc++] = newBullet;
            bulletsLoc %= 100;
        }

        if(playerPos.x < -20.0 ) playerPos.x = screenWidth;
        if(playerPos.x > screenWidth + 20) playerPos.x = -20;

        if(playerPos.y < -20.0 ) playerPos.y = screenHeight;
        if(playerPos.y > screenHeight + 20) playerPos.y = -20;

        for(int a = 0; a < asteroidLoc; a++) {
            if(!asteroids[a].active) continue;
            asteroids[a].position.x += asteroids[a].speed.x * delta;
            asteroids[a].position.y += asteroids[a].speed.y * delta;
            asteroids[a].rotation += asteroids[a].rotationSpeedRadians * delta;
            if(asteroids[a].position.x < -40.0 ) asteroids[a].position.x = screenWidth + 40;
            if(asteroids[a].position.x > screenWidth + 40) asteroids[a].position.x = -40;
            if(asteroids[a].position.y < -40.0 ) asteroids[a].position.y = screenHeight + 40;
            if(asteroids[a].position.y > screenHeight + 40) asteroids[a].position.y = -40;
        }

        Vector2 playerTrianglePos[3] = {
            AddVectors(RotatePoint(playerTriangle[0], NULL, playerRotRad), playerPos),
            AddVectors(RotatePoint(playerTriangle[1], NULL, playerRotRad), playerPos),
            AddVectors(RotatePoint(playerTriangle[2], NULL, playerRotRad), playerPos),
        };

        for(int i = 0; i < 100; i++){
            if(!bullets[i].active) continue;
            bullets[i].position.x += bullets[i].direction.x * delta;
            bullets[i].position.y += bullets[i].direction.y * delta;

            int asteroidCount = asteroidLoc;
            for(int a = 0; a < asteroidCount; a++) {
                if(!bullets[i].active) break;
                if(!asteroids[a].active) continue;
                float xDistance = fabsf(bullets[i].position.x - asteroids[a].position.x);
                float yDistance = fabsf(bullets[i].position.y - asteroids[a].position.y);
                if(xDistance < asteroids[a].size && yDistance < asteroids[a].size) {
                    bullets[i].active = false;
                    asteroids[a].active = false;
                    if(asteroids[a].size <= 10) continue;
                    for( int j = 0; j < 2; j++) {
                        int speedX = GetRandomValue(-60, 60);
                        int speedY = GetRandomValue(-60, 60);
                        int new_size = asteroids[a].size/2;
                        Asteroid new_asteroid = {
                            .points = MakeAsteroidShape(new_size , new_size , asteroidPointsLoc),
                            .size = new_size,
                            .position = asteroids[a].position,
                            .rotation = 0.0,
                            .rotationSpeedRadians = (GetRandomValue(1, 40) - 20) / 360 * DEG2RAD, 
                            .active = true,
                            .speed = (Vector2){speedX, speedY}
                        };
                        asteroidPointsLoc +=  new_size;
                        asteroids[asteroidLoc++] = new_asteroid;
                    }
                }
            }

            if(bullets[i].position.x < -40.0 ) bullets[i].active = false;
            if(bullets[i].position.x > screenWidth + 40) bullets[i].active = false;
            if(bullets[i].position.y < -40.0 ) bullets[i].active = false;
            if(bullets[i].position.y > screenHeight + 40) bullets[i].active = false;
        }


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // DrawRectangleRec(player, RED);
            // DrawRectanglePro(player, playerOrigin, playerRotation, RED);

            // DrawPoly(playerPos, 3, 20.0, playerRotation, RED); 
            DrawTriangle(playerTrianglePos[0], playerTrianglePos[1], playerTrianglePos[2], RED);

            DrawLineEx(playerTrianglePos[0], playerTrianglePos[1], 2.0f, RED);
            DrawLineEx(playerTrianglePos[1], playerTrianglePos[2], 2.0f, RED);
            DrawLineEx(playerTrianglePos[2], playerTrianglePos[0], 2.0f, RED);

            float playerRotInRads = playerRotation  * DEG2RAD;

            Vector2 asteroidPos = {100.0 + playerPos.x, 100.0 + playerPos.y};
            for(int a = 0; a < asteroidLoc; a++) {
                if(!asteroids[a].active) continue;
                int pointCount = asteroids[a].size;
                for (int i = 0; i < pointCount - 1; i++) {
                    DrawLineEx(
                        GetVectorForAsteroidPoint(&asteroids[a].points[i], &asteroids[a].position, asteroids[a].rotation ),
                        GetVectorForAsteroidPoint(&asteroids[a].points[i + 1], &asteroids[a].position, asteroids[a].rotation ),
                        3.0, RED);
                }
            }

            for(int i = 0; i < 100; i++){
                if(!bullets[i].active) continue;
                DrawCircle(bullets[i].position.x, bullets[i].position.y, 5.0f, BLUE);
            }

            // DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    free(asteroidPointsArea);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}