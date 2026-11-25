#pragma once

#include "../search_engine/document.hpp"
#include "../search_engine/text_processor.hpp"
#include "../search_engine/index.hpp"

#include <unordered_set>

class InvertedIndex {
public:
    explicit InvertedIndex(
        const PreprocessingConfig &config = {}
    );

    void add_document(const Document &document);

    const Index &get_index() const;

private:
    TextProcessor text_processor_;
    Index index_;
    std::unordered_set<int> document_ids_;
};
