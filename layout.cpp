#include "layout.h"
#include <sstream>
#include <algorithm>
#include <functional>

static void collect_text(const DomTree* n, std::string& out){
    auto &e = n->element;
    if(e.type==Token::Type::Text){ 
        out += e.data;
        };
    for(auto &ch: n->children) {
        collect_text(ch.get(), out);
    };
}

// create display list (for drawing)
DisplayList build_display_list(const DomTree* root, TTF_Font* font, int vw){
    DisplayList dl;
    int x0 = 16, y = 16;
    int lineH = TTF_FontHeight(font) + 4;
    int pMargin = 8;

    std::function<void(const DomTree*)> walk = [&](const DomTree* n){
        if(!n) return;
        const auto& e = n->element;

        if(e.type == Token::Type::StartTag && e.name == "p"){
            std::string text; 
            collect_text(n, text);
            //text = collapse_spaces(text);
            dl.push_back({x0, y, text}); //std::vector<DrawTextCmd>
            y += lineH + pMargin;                       
            return;
        }
        for(const auto& ch : n->children) {
            walk(ch.get());
        };
    };
    walk(root);
    return dl;
}
