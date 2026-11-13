#include "../include/search_engine/tokenizer.hpp"

#include <cctype>

std::vector<std::string> tokenize(const std::string &text) {
    std::vector<std::string> tokens;
    std::string current_token;

    for(unsigned char ch : text) {
        if (std::isalnum(ch)) {
            current_token += static_cast<char>(std::tolower(ch));
        } else {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        }
    }

    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    return tokens;
}
