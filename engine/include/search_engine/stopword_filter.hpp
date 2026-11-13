#pragma once

#include <string>
#include <unordered_set>
#include <vector>

class StopwordFilter {
public:
    explicit StopwordFilter(const std::string &file_path);

    std::vector<std::string> filter(
        const std::vector<std::string> &tokens
    ) const; // It won't act on the stopwords_

private:
    std::unordered_set<std::string> stopwords_;
};
