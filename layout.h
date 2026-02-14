#ifndef LAYOUT_H
#define LAYOUT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "tokenizer.h"
#include "css.h"

struct DrawTextCmd { 
    int x,y; 
    std::string text;  
    SDL_Color c; 
    TTF_Font* font;
};
using DisplayList = std::vector<DrawTextCmd>;

// create display list (for drawing)
DisplayList build_display_list(const DomTree* root, TTF_Font* font, const std::vector<Rule>&);

#endif // LAYOUT_H
