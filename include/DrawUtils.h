#include <SDL.h>

struct Vector2 {
  int x;
  int y;
};

struct Color {
  int R;
  int G;
  int B;
  int A;
};

enum {
  EMPTY,
  BUTTON,
  SLIDER
};

enum class Align {
  TOP_LEFT,
  MIDDLE_LEFT,
  BOTTOM_LEFT,
  TOP_CENTER,
  MIDDLE_CENTER,
  BOTTOM_CENTER,
  TOP_RIGHT,
  MIDDLE_RIGHT,
  BOTTOM_RIGHT
};

enum class LineAlign {
    INNER,
    CENTER,
    OUTER
};

class DrawUtils {
public:
  static SDL_Rect GetAlignedRect(int x, int y, int w, int h, int margin, Align align) {
    SDL_Rect r = {x, y, w, h};

    switch (align) {
    case Align::TOP_CENTER:
    case Align::MIDDLE_CENTER:
    case Align::BOTTOM_CENTER:
      r.x -= w / 2;
      break;
    case Align::TOP_RIGHT:
    case Align::MIDDLE_RIGHT:
    case Align::BOTTOM_RIGHT:
      r.x -= w;
      break;
    default:
      break;
    }

    switch (align) {
    case Align::MIDDLE_LEFT:
    case Align::MIDDLE_CENTER:
    case Align::MIDDLE_RIGHT:
      r.y -= h / 2;
      break;
    case Align::BOTTOM_LEFT:
    case Align::BOTTOM_CENTER:
    case Align::BOTTOM_RIGHT:
      r.y -= h;
      break;
    default:
      break;
    }

    if (margin != 0) {
      r.x += margin;
      r.y += margin;
      r.w -= margin * 2;
      r.h -= margin * 2;
    }

    return r;
  }

  static void DrawRect(SDL_Renderer *renderer, int x, int y, int w, int h, Align align) {
    SDL_Rect r = GetAlignedRect(x, y, w, h, 0, align);
    SDL_RenderDrawRect(renderer, &r);
  }

  static void FillRect(SDL_Renderer *renderer, int x, int y, int w, int h, Align align) {
    SDL_Rect r = GetAlignedRect(x, y, w, h, 0, align);
    SDL_RenderFillRect(renderer, &r);
  }

static void DrawRoundedRect(SDL_Renderer* renderer, int x, int y, int w, int h, int radius, int thickness, LineAlign lineAlign, Align align) {
        SDL_Rect r = GetAlignedRect(x, y, w, h, 0, align);

        float offset = 0;
        switch (lineAlign) {
            case LineAlign::INNER:  offset = 0; break;
            case LineAlign::CENTER: offset = -(thickness / 2.0f); break;
            case LineAlign::OUTER:  offset = -thickness; break;
        }

        for (int i = 0; i < thickness; i++) {
            int curX = r.x + (int)offset + i;
            int curY = r.y + (int)offset + i;
            int curW = r.w - ((int)offset + i) * 2;
            int curH = r.h - ((int)offset + i) * 2;
            int curR = radius;
            
            // Adjust radius for concentric layers to keep them smooth
            if (lineAlign == LineAlign::INNER) curR -= i;
            else curR = radius + (int)offset + i;

            drawRoundedFrame(renderer, curX, curY, curW, curH, curR);
        }
    }

private:
    static void drawRoundedFrame(SDL_Renderer* renderer, int x, int y, int w, int h, int radius) {
        if (radius < 0) radius = 0;
        if (radius > w / 2) radius = w / 2;
        if (radius > h / 2) radius = h / 2;

        if (radius == 0) {
            SDL_Rect r = {x, y, w, h};
            SDL_RenderDrawRect(renderer, &r);
            return;
        }

        int cx = radius;
        int cy = 0;
        int radiusError = 1 - cx;

        while (cx >= cy) {
            SDL_RenderDrawPoint(renderer, x + radius - cx, y + radius - cy);
            SDL_RenderDrawPoint(renderer, x + radius - cy, y + radius - cx);
            SDL_RenderDrawPoint(renderer, x + w - radius + cx, y + radius - cy);
            SDL_RenderDrawPoint(renderer, x + w - radius + cy, y + radius - cx);
            SDL_RenderDrawPoint(renderer, x + radius - cx, y + h - radius + cy);
            SDL_RenderDrawPoint(renderer, x + radius - cy, y + h - radius + cx);
            SDL_RenderDrawPoint(renderer, x + w - radius + cx, y + h - radius + cy);
            SDL_RenderDrawPoint(renderer, x + w - radius + cy, y + h - radius + cx);

            cy++;
            if (radiusError < 0) {
                radiusError += 2 * cy + 1;
            } else {
                cx--;
                radiusError += 2 * (cy - cx + 1);
            }
        }

        SDL_RenderDrawLine(renderer, x + radius, y, x + w - radius, y);
        SDL_RenderDrawLine(renderer, x + radius, y + h, x + w - radius, y + h);
        SDL_RenderDrawLine(renderer, x, y + radius, x, y + h - radius);
        SDL_RenderDrawLine(renderer, x + w, y + radius, x + w, y + h - radius);
    }
};