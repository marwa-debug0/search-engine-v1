#include "../include/search_engine/stopword_filter.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>


StopwordFilter::StopwordFilter(const std::string &file_path) {
    std::ifstream file(file_path);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open stopword file: " + file_path);
    }

    std::string line;

    while (std::getline(file, line)) {
        std::istringstream line_stream(line);
        std::string word;

        if (!(line_stream >> word)) {
            continue;
        }

        if (word[0] == '|') {
            continue;
        }

        stopwords_.insert(word);
    }
}


std::vector<std::string> StopwordFilter::filter(
    const std::vector<std::string> &tokens
) const {
    std::vector<std::string> filtered_tokens;

    for(const std::string &token : tokens) {
        if (stopwords_.find(token) == stopwords_.end()) {
            filtered_tokens.push_back(token);
        }
    }
    return filtered_tokens;
}
