#include "../include/search_engine/inverted_index.hpp"

#include <stdexcept>
#include <algorithm>

InvertedIndex::InvertedIndex(const PreprocessingConfig &config)
    : text_processor_(config) {}


void InvertedIndex::add_document(const Document &document) {
    if (document_ids_.find(document.id) != document_ids_.end()) {
        throw std::invalid_argument("Document ID already exists");
    }

    std::vector<std::string> tokens = text_processor_.process(document.content);

    std::unordered_map<std::string, int> term_frequencies;

    for (const std::string &token : tokens) {
        term_frequencies[token]++;
    }

    for (const auto &[term, frequency] : term_frequencies) {
    auto &postings = index_[term];

    auto position = std::lower_bound(
        postings.begin(),
        postings.end(),
        document.id,
        [](const Posting &posting, int id) {
            return posting.document_id < id;
        }
    );

    postings.insert(position, {document.id, frequency});
}

    document_ids_.insert(document.id);
}

const Index &InvertedIndex::get_index() const {
    return index_;
}
