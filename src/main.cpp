#include "AppContext.h"
#include "Config.h"
#include "ProjectTree.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <random>

int buttonSpacing = 5, buttonRadius = 5, GRID_COLS = 15, GRID_ROWS = 6;
Vector2 windowSize = {1280, 720}, blockSize = {1, 1};
bool running = true;
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *f_font = nullptr;
int currentFPS = 0;
Color colorPalette[100];

ProjectNode root = {"Root", true, true, {{"addons", true, false, {{"spout-gd", true, true, {{"SpoutLibrary.dll", false, false}, {"spout_gd.gdextension", false, false}}}}}, {"config", true, true, {{"settings.json", false, false}}}}};

std::random_device randomGenerator;
std::mt19937 gen(randomGenerator());

Color genColor(int alpha) {
  std::uniform_int_distribution<> R(0, 255), G(0, 255), B(0, 255);
  return Color{R(gen), G(gen), B(gen), alpha};
}

bool initializeSDL() {
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    return false;
  if (TTF_Init() < 0)
    return false;
  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    return false;
  return Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) >= 0;
}

bool loadAssets() {
  SDL_Surface *icon = IMG_Load(ASSET_PATH "icon.png");
  if (icon == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not load icon: %s\n", IMG_GetError());
    return false;
  }

  SDL_SetWindowIcon(window, icon);
  SDL_FreeSurface(icon);

  f_font = TTF_OpenFont(ASSET_PATH "font.ttf", 32);

  if (f_font == NULL) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to load font: %s\n", TTF_GetError());
    return false;
  }

  return true;
}

bool initializeWin() {
  std::string titleWithDate = WINDOW_TITLE + " [Built: " + __DATE__ + " " + __TIME__ + "]";

  window = SDL_CreateWindow(titleWithDate.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.x, windowSize.y, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window could not be created! SDL_Error:  %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Renderer could not be created! SDL_Error:  %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  return true;
}

void drawBG() {
  SDL_RenderSetViewport(renderer, NULL);
  SDL_RenderSetClipRect(renderer, NULL);

  SDL_Rect screenRect = {0, 0, windowSize.x, windowSize.y};
  SDL_SetRenderDrawColor(renderer, 0, 0, 25, 255);
  SDL_RenderClear(renderer);

  SDL_Rect container = DrawUtils::GetAlignedRect(0, 0, windowSize.x - 250, windowSize.y, 20, Align::TOP_LEFT);
  SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
  SDL_RenderFillRect(renderer, &container);
}

void drawFG() {
  SDL_Rect container = DrawUtils::GetAlignedRect(0, 0, windowSize.x - 250, windowSize.y, 20, Align::TOP_LEFT);

  if (container.w / GRID_COLS < container.h / GRID_ROWS) {
    blockSize = {container.w / GRID_COLS, container.w / GRID_COLS};
  } else {
    blockSize = {container.h / GRID_ROWS, container.h / GRID_ROWS};
  }

  int totalGridW = (blockSize.x * GRID_COLS);
  int totalGridH = (blockSize.y * GRID_ROWS);

  int offsetX = (container.w - totalGridW + buttonSpacing) / 2;
  int offsetY = (container.h - totalGridH + buttonSpacing) / 2;

  int lineWidth = 2;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  for (int y = 0; y < GRID_ROWS; y++) {
    for (int x = 0; x < GRID_COLS; x++) {
      int rectX = container.x + offsetX + (x * blockSize.x);
      int rectY = container.y + offsetY + (y * blockSize.y);
      int rectW = blockSize.x;
      int rectH = blockSize.y;

      DrawUtils::DrawRoundedRect(renderer, rectX, rectY, rectW - buttonSpacing, rectH - buttonSpacing, buttonRadius, lineWidth, LineAlign::CENTER, Align::TOP_LEFT);
    }
  }
}

void drawButtons() {
  SDL_Rect container = DrawUtils::GetAlignedRect(0, 0, windowSize.x - 250, windowSize.y, 20, Align::TOP_LEFT);

  if (container.w / GRID_COLS < container.h / GRID_ROWS) {
    blockSize = {container.w / GRID_COLS, container.w / GRID_COLS};
  } else {
    blockSize = {container.h / GRID_ROWS, container.h / GRID_ROWS};
  }

  int totalGridW = (blockSize.x * GRID_COLS);
  int totalGridH = (blockSize.y * GRID_ROWS);
  int offsetX = (container.w - totalGridW + buttonSpacing) / 2;
  int offsetY = (container.h - totalGridH + buttonSpacing) / 2;

  int colorIndex = 0;
  for (int y = 0; y < GRID_ROWS; y++) {
    for (int x = 0; x < GRID_COLS; x++) {
      int rectX = container.x + offsetX + (x * blockSize.x);
      int rectY = container.y + offsetY + (y * blockSize.y);

      SDL_Rect fillRect = {rectX, rectY, blockSize.x - buttonSpacing, blockSize.y - buttonSpacing};

      SDL_SetRenderDrawColor(renderer, colorPalette[colorIndex].R, colorPalette[colorIndex].G, colorPalette[colorIndex].B, colorPalette[colorIndex].A);
      SDL_RenderFillRect(renderer, &fillRect);

      colorIndex = (colorIndex + 1) % 100;
    }
  }
}

void drawMenu() {
  bool changed = false;
  ImGui::SetNextWindowPos(ImVec2(windowSize.x - 250, windowSize.y), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
  ImGui::SetNextWindowSize(ImVec2(250, windowSize.y));
  ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

  ImGui::Text("FPS: %d", currentFPS);
  ImGui::Separator();
  ImGui::BeginChild("FileTreeRegion", ImVec2(0, 200), true);

  DrawFolderTree(root);
  ImGui::EndChild();

  if (ImGui::Button("Open Settings Folder", ImVec2(-FLT_MIN, 0))) {
    Config::OpenFolder();
  }
  if (ImGui::SliderInt("Spacing", &buttonSpacing, 0, 50)) {
    changed = true;
  }
  if (ImGui::SliderInt("Radius", &buttonRadius, 0, 50)) {
    changed = true;
  }
  if (changed || ImGui::IsItemDeactivatedAfterEdit()) {
    Config::Save();
  }

  ImGui::PushItemWidth(100);
  ImGui::InputInt("X", &GRID_COLS);
  ImGui::InputInt("Y", &GRID_ROWS);
  ImGui::PopItemWidth();
  ImGui::End();
}

void handleInput(SDL_Scancode key) {
  switch (key) {
  case SDL_SCANCODE_W:
    break;

  case SDL_SCANCODE_ESCAPE:
    running = false;
    break;

  default:
    break;
  }
}

void close() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  TTF_CloseFont(f_font);

  f_font = NULL;

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  window = NULL;
  renderer = NULL;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

int main(int argc, char *argv[]) {
  Config::Load();

  if (!initializeSDL() || !initializeWin() || !loadAssets()) {
    return 1;
  }

  for (int i = 0; i < 100; i++) {
    colorPalette[i] = genColor(255);
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  SDL_Event windowEvent;
  Uint32 startTime = SDL_GetTicks();
  int frameCount = 0;

  SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
  while (running) {
    while (SDL_PollEvent(&windowEvent)) {
      ImGui_ImplSDL2_ProcessEvent(&windowEvent);

      switch (windowEvent.type) {
      case SDL_WINDOWEVENT:
        switch (windowEvent.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        case SDL_WINDOWEVENT_RESIZED:
          SDL_GetWindowSize(window, &windowSize.x, &windowSize.y);
          break;
        }
        break;
      case SDL_QUIT:
        running = false;
        break;
      case SDL_KEYDOWN:
        if (!io.WantCaptureKeyboard) {
          handleInput(windowEvent.key.keysym.scancode);
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (!io.WantCaptureMouse) {
          // Logic
        }
        break;
      }
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    drawBG();
    drawButtons();
    drawFG();
    drawMenu();

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);

    frameCount++;
    if (SDL_GetTicks() - startTime >= 1000) {
      currentFPS = frameCount;
      frameCount = 0;
      startTime = SDL_GetTicks();
    }
  }

  close();
  return 0;
}