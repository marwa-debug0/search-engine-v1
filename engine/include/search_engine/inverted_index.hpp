#pragma once

#include "../search_engine/document.hpp"
#include "../search_engine/text_processor.hpp"
#include "../search_engine/index.hpp"

#include <unordered_set>
#include <vector>

class InvertedIndex {
public:
    explicit InvertedIndex(
        const PreprocessingConfig &config = {}
    );

    void add_document(const Document &document);

    const Index &get_index() const;

    std::vector<int> get_all_document_ids() const;
    std::vector<int> get_document_ids_for_term(const std::string &term);
    const PostingList &get_postings_for_processed_term(const std::string &processed_term) const;


private:
    TextProcessor text_processor_;
    Index index_;
    std::unordered_set<int> document_ids_;
};
