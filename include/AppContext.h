#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include "DrawUtils.h"

const std::string WINDOW_TITLE = "macro";

extern int buttonSpacing;
extern int buttonRadius;
extern int GRID_COLS;
extern int GRID_ROWS;
extern Vector2 blockSize;
extern Vector2 windowSize;
extern int currentFPS;
extern bool running;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* f_font;
extern Color colorPalette[100];

//TODO:
struct Actionable {
  int type;
};