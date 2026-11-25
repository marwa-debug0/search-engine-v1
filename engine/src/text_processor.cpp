#include "../include/search_engine/text_processor.hpp"
#include "../include/search_engine/tokenizer.hpp"


TextProcessor::TextProcessor(const PreprocessingConfig &config) {
    if (config.remove_stopwords) {
        stopword_filter_.emplace(config.stopwords_file);
    }
}

std::vector<std::string> TextProcessor::process(const std::string &text) {
    std::vector<std::string> tokens = tokenize(text);

    if (stopword_filter_.has_value()) {
        tokens = stopword_filter_->filter(tokens);
    }

    for (std::string &token : tokens) {
        token = stemmer_.stem(token);
    }

    return tokens;
}
