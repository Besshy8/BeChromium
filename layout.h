#ifndef LAYOUT_H
#define LAYOUT_H

#include "css.h"
#include "tokenizer.h"
#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

struct DrawTextCmd {
  int x, y;
  std::string text;
  SDL_Color c;
  TTF_Font *font;
  SDL_Rect box;
  std::string onclick; // "alert('hi'); count += 1"
};
using DisplayList = std::vector<DrawTextCmd>;

// create display list (for drawing)
DisplayList build_display_list(const DomTree *root, TTF_Font *font,
                               const std::vector<Rule> &);

#endif // LAYOUT_H
