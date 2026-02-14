#pragma once

#include <string>
#include <vector>
#include <SDL.h>

struct Style{ 
    bool hasC = false;
    bool hasS = false;
    SDL_Color c{0,0,0,255};
    int px = 18;
};

struct Rule{ 
    std::string sel;
    Style st;
};

// Parse CSS text into a list of rules. Signature matches existing implementation.
std::vector<Rule> parse_css(std::string css);

// Compute the style for a given tag from the ruleset.
Style style_for(const std::vector<Rule>& rs, const std::string& tag);
