#pragma once

#include <string>
#include <vector>

#include "../search_engine/inverted_index.hpp"

struct SearchResult {
    int document_id;
    double score;
};

class TfidfScorer {
public:
    explicit TfidfScorer(const InvertedIndex &index);

    std::vector<SearchResult> search(
        const std::vector<std::string> &processed_query_terms,
        std::size_t top_k
    );

private:
    const InvertedIndex &index_;
};
