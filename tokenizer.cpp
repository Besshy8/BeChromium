#include "tokenizer.h"
#include <iostream>
#include <stdio.h>

void tokenization(const std::string body) {
    // extract all <p>...</p> elements
    size_t pos = 0;
    while (pos != std::string::npos) {
        auto p_s = body.find("<p>", pos);
        auto p_e = body.find("</p>", pos);
        //printf("p_s: %zu, p_e: %zu", p_s, p_e);
        if (p_s == std::string::npos || p_e == std::string::npos) {
            //std::cout << "no more <p> or </p>\n";
            break;
        }
        std::string p_str = body.substr(p_s + 3, p_e - (p_s + 3));
        pos = p_e + 4;
        printf("%s\n", p_str.c_str());
    }
    //printf("%s", "END_token");
}
