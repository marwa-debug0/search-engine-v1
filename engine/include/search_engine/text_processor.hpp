
#pragma once

#include <string>
#include <vector>
#include <optional>

#include "../search_engine/stemmer.hpp"
#include "../search_engine/stopword_filter.hpp"

struct PreprocessingConfig {
    bool remove_stopwords = true;
    std::string stopwords_file = "data/stopwords/english.txt";
};

class TextProcessor {
public:
    explicit TextProcessor(
        const PreprocessingConfig &config = {}
    );

    std::vector<std::string> process(const std::string &text);

private:
    Stemmer stemmer_;
    std::optional<StopwordFilter> stopword_filter_;
};
