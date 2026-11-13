#include "../include/search_engine/text_processor.hpp"
#include "../include/search_engine/tokenizer.hpp"

std::vector<std::string> preprocess_text(const std::string& text, Stemmer &stemmer){
    auto tokens = tokenize(text);

    for (std::string &token : tokens) {
        token = stemmer.stem(token);
    }

    return tokens;
}
