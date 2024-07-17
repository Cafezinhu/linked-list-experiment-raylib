#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100
#endif
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

const int screenWidth = 800;
const int screenHeight = 600;
const int PLAYER_SPEED = 300;

struct Entity {
  float x;
  float y;
  Texture texture;

  void (*Update)(struct Entity *entity);
};

struct Item {
  struct Item *next;
  struct Entity entity;
};

void UpdatePlayer(struct Entity *entity);
void UpdatePlayer(struct Entity *entity) {
  Vector2 vec = {};

  if (IsKeyDown(KEY_D)) {
    vec.x = 1;
  } else if (IsKeyDown(KEY_A)) {
    vec.x = -1;
  }

  if (IsKeyDown(KEY_W)) {
    vec.y = -1;
  } else if (IsKeyDown(KEY_S)) {
    vec.y = 1;
  }

  if (vec.x != 0 || vec.y != 0) {
    vec = Vector2Normalize(vec);
  }

  (*entity).x += vec.x * PLAYER_SPEED * GetFrameTime();
  (*entity).y += vec.y * PLAYER_SPEED * GetFrameTime();

  DrawTexture(entity->texture, entity->x, entity->y, WHITE);
}

struct Entity CreatePlayer();
struct Entity CreatePlayer() {
  struct Entity player = {};

  player.x = 0;
  player.y = 0;
  player.texture = LoadTexture("boneco.png");
  player.Update = &UpdatePlayer;

  return player;
}

void UpdateDrawFrame(struct Item *first_item);

//------------------ASSETS--------------
Texture tex;

int main() {

  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Raylib basic window");

#ifndef PLATFORM_ANDROID
  ChangeDirectory("assets");
#endif

  SetExitKey(0);

  SetTargetFPS(60);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  // SetTargetFPS(60);
  tex = LoadTexture("boneco.png");
  struct Item *first_item = (struct Item *)malloc(sizeof(struct Item));

  *first_item = (struct Item){};
  (*first_item).entity = CreatePlayer();
  // #ifndef PLATFORM_ANDROID
  // ChangeDirectory("..");
  // #endif
  while (!WindowShouldClose()) {
    UpdateDrawFrame(first_item);
  }
#endif

  CloseWindow();
  return 0;
}

void UpdateDrawFrame(struct Item *first_item) {
  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawFPS(10, 10);
  first_item->entity.Update(&(first_item->entity));

  EndDrawing();
}
