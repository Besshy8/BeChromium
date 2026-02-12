#ifndef LAYOUT_H
#define LAYOUT_H

#include <SDL_ttf.h>
#include <string>
#include <vector>
#include "tokenizer.h"

struct DrawTextCmd { 
    int x,y; 
    std::string text;  
};
using DisplayList = std::vector<DrawTextCmd>;

// create display list (for drawing)
DisplayList build_display_list(const DomTree* root, TTF_Font* font, int vw);

#endif // LAYOUT_H
