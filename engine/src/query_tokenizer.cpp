#include "../include/search_engine/query_tokenizer.hpp"

#include <cctype>

// Decides wether its an operator or a term and adds it to the tokens list
static void add_word(
    const std::string &word,
    std::vector<QueryToken> &tokens
) {
    if (word.empty()) {
        return;
    }

    if (word == "and") {
        tokens.push_back({QueryTokenType::And, ""});
    } else if (word == "or") {
        tokens.push_back({QueryTokenType::Or, ""});
    } else if (word == "not") {
        tokens.push_back({QueryTokenType::Not, ""});
    } else {
        tokens.push_back({QueryTokenType::Term, word});
    }
}

std::vector<QueryToken> tokenize_query(const std::string &query) {
    std::vector<QueryToken> tokens;
    std::string current_word;

    for (unsigned char ch : query) {
        if (std::isalnum(ch)) {
            current_word += static_cast<char>(std::tolower(ch));
        } else {
            add_word(current_word, tokens);
            current_word.clear();

            if (ch == '(') {
                tokens.push_back({QueryTokenType::LeftParen, ""});
            } else if (ch == ')') {
                tokens.push_back({QueryTokenType::RightParen, ""});
            }
        }
    }

    add_word(current_word, tokens);

    return tokens;
}
