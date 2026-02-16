#include "layout.h"
#include <sstream>
#include <algorithm>
#include <functional>
#include "js.h"

static void collect_text(const DomTree* n, std::string& out){
    auto &e = n->element;
    if(e.type==Token::Type::Text){ 
        out += e.data;
        };
    for(auto &ch: n->children) {
        collect_text(ch.get(), out);
    };
}

// create display list by using DFS
DisplayList build_display_list(const DomTree* root, TTF_Font* font,
                                const std::vector<Rule>& css){
    DisplayList dl;
    int x0 = 16, y = 16;
    int lineH = TTF_FontHeight(font) + 4;
    int pMargin = 8;

    std::function<void(const DomTree*)> walk = [&](const DomTree* n){
        const auto& e = n->element;

        if(e.type == Token::Type::StartTag && e.name[0] == 'p'){
            std::string text; 
            collect_text(n, text);
            Style st=style_for(css, "p"); 
            TTF_SetFontSize(font, st.px);
            int w=0,h=0;
            TTF_SizeText(font, text.c_str(), &w, &h);

            // deal with js info
            SDL_Rect box{ x0, y, w, h };
            std::string onclick = extract_onclick(e.name); // e.g. e.name = "p onclick="alert('hi'); count += 1""

            dl.push_back({x0, y, text, st.c, font, box, onclick}); // add DrawTextCmd
            y += (TTF_FontHeight(font)+4) + pMargin;                       
            return;
        }
        for(const auto& ch : n->children) {
            walk(ch.get());
        };
    };
    walk(root);
    return dl;
}
