#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>

// Token型
struct Token {
    enum class Type { StartTag, EndTag, Text } type;
    std::string name; // Start/End 
    std::string data; // Text 
};

std::vector<Token> tokenize_html(const std::string& input);

#endif // TOKENIZER_H
