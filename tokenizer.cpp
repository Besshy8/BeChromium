#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <cctype>
#include "tokenizer.h"


// tokenize http body 
std::vector<Token> tokenize_html(const std::string& input) {
    std::vector<Token> out;
    std::string textBuf; 
    std::string name; // tag name

    const size_t N = input.size();
    size_t i = 0;

    while (i < N) {
        char c = input[i];
        if (c != '<') {
            textBuf.push_back(c);
            ++i;
            continue;
        }

        // case '<' → assignment just before text to Token(Text)
        if (!textBuf.empty()) {
            out.push_back(Token{Token::Type::Text, "", textBuf});
        }
        textBuf.clear();

        if (input[i + 1] == '/') { // e.g. </div>
            i += 2; // '<' and '/' 
            while (i < N) {
                if (input[i] == '>') {
                    ++i;
                    break;
                };
                name.push_back(input[i]);
                ++i;
            }
            out.push_back(Token{Token::Type::EndTag, name, ""});
            name.clear();
        } else { // e.g. <div>
            ++i; // '<' 
            while (i < N) {
                if (input[i] == '>') {
                    ++i;
                    break;
                } 
                name.push_back(input[i]);
                ++i;
            }
            out.push_back(Token{Token::Type::StartTag, name, ""});
            name.clear();
        }
    }

    return out;
}

// 動作確認
//int main() {
//    std::string body = "<body><div><p>Hello <b>world</b></p><p>Second paragraph.</p></div></body>";
//    auto tokens = tokenize_minimal_html(body);
//
//    for (const auto& t : tokens) {
//        switch (t.type) {
//            case Token::Type::StartTag:
//                std::cout << "StartTag(" << t.name << ")\n";
//                break;
//            case Token::Type::EndTag:
//                std::cout << "EndTag(" << t.name << ")\n";
//               break;
//            case Token::Type::Text:
//                std::cout << "Text(" << t.data << ")\n";
//                break;
//        }
//    }
//    return 0;}
