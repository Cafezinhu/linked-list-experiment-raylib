#include "raylib.h"
#include "raymath.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
const int PLAYER_SPEED = 400;
const int BULLET_SPEED = 300;

Texture bullet_texture;

struct Entity {
  struct Entity *next_entity;
  float x;
  float y;
  Texture texture;
  bool dead;

  void (*Update)(struct Entity *player);
};

struct Entity *first_entity;

void UpdateBullet(struct Entity *bullet);
void UpdateBullet(struct Entity *bullet) {
  (*bullet).x += BULLET_SPEED * GetFrameTime();
  DrawTexture(bullet->texture, bullet->x, bullet->y, WHITE);
  if (bullet->x > screenWidth) {
    (*bullet).dead = true;
  }
}

struct Entity CreateBullet(int x, int y, struct Entity *next_entity);
struct Entity CreateBullet(int x, int y, struct Entity *next_entity) {
  struct Entity bullet = {};

  bullet.x = x;
  bullet.y = y;
  bullet.texture = bullet_texture;
  bullet.next_entity = next_entity;
  bullet.Update = &UpdateBullet;

  return bullet;
}

void UpdatePlayer(struct Entity *player);
void UpdatePlayer(struct Entity *player) {
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

  (*player).x += vec.x * PLAYER_SPEED * GetFrameTime();
  (*player).y += vec.y * PLAYER_SPEED * GetFrameTime();

  DrawTexture(player->texture, player->x, player->y, WHITE);

  if (IsKeyPressed(KEY_SPACE)) {
    struct Entity *old_next = player->next_entity;
    (*player).next_entity = malloc(sizeof(struct Entity));
    *(*player).next_entity = CreateBullet(player->x, player->y, old_next);
  }
}

struct Entity CreatePlayer();
struct Entity CreatePlayer() {
  struct Entity player = {};

  player.texture = LoadTexture("boneco.png");
  player.Update = &UpdatePlayer;

  return player;
}

void UpdateDrawFrame(struct Entity *first_entity);

//------------------ASSETS--------------

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
  // struct Entity *first_entity = (struct Entity *)malloc(sizeof(struct
  // player));
  bullet_texture = LoadTexture("bala.png");
  first_entity = malloc(sizeof(struct Entity));

  *first_entity = (struct Entity){};
  *first_entity = CreatePlayer();
  // #ifndef PLATFORM_ANDROID
  // ChangeDirectory("..");
  // #endif
  while (!WindowShouldClose()) {
    UpdateDrawFrame(first_entity);
  }
#endif

  CloseWindow();
  return 0;
}

void UpdateDrawFrame(struct Entity *first_entity) {
  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();
  ClearBackground(RAYWHITE);
  DrawFPS(10, 10);
  struct Entity *entity = first_entity;
  struct Entity *previous_entity = NULL;
  while (entity != NULL) {
    if (entity->dead) {
      printf("Killing entity\n");
      struct Entity *next_entity = entity->next_entity;
      struct Entity *dead_entity = entity;
      if (first_entity != NULL && previous_entity != NULL) {
        (*previous_entity).next_entity = entity->next_entity;
        entity = entity->next_entity;
      } else {
        first_entity = next_entity;
      }
      free(dead_entity);
    } else {
      entity->Update(entity);
      previous_entity = entity;
      entity = entity->next_entity;
    }
  }

  EndDrawing();
}
