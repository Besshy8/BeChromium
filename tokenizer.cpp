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

DomTree* make_dom_tree(const std::vector<Token>& tokens) {
    DomTree* root = new DomTree{};
    root->element = Token{Token::Type::Top, "", ""} ; 

    DomTree* dom_p = root;
    std::vector<DomTree*> node_stack;

    const size_t N = tokens.size();
    size_t i = 0;

    while (i < N) {
        if (tokens[i].type == Token::Type::StartTag) {
            dom_p->children.push_back(std::make_unique<DomTree>());
            DomTree* child = dom_p->children.back().get(); // childへのポインタを取得 
            child->element = tokens[i];
            node_stack.push_back(dom_p); // 親を保存
            dom_p = child;
            ++i;
        } else if (tokens[i].type == Token::Type::EndTag) {
            dom_p = node_stack.back(); // 自分の親にポインタを戻す
            node_stack.pop_back();
            ++i;
        } else {
            // case Token::Type::Text
            dom_p->children.push_back(std::make_unique<DomTree>());
            DomTree* child = dom_p->children.back().get();
            child->element = tokens[i];
            //dom_p = child;
            ++i;
        }
    }
    return root;
}
