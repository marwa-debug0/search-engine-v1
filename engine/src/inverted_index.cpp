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

// We need the id getter for the NOT query that needs to access all documents
std::vector<int> InvertedIndex::get_all_document_ids() const {
    std::vector<int> ids(
        document_ids_.begin(),
        document_ids_.end()
    );

    std::sort(ids.begin(), ids.end());

    return ids;
}

std::vector<int> InvertedIndex::get_document_ids_for_term(
    const std::string &term
) {
    std::vector<std::string> processed_terms = text_processor_.process(term);

    if (processed_terms.empty()) {
        return {};
    }

    const std::string &processed_term = processed_terms.front();

    auto it = index_.find(processed_term);

    if (it == index_.end()) {
        return {};
    }

    std::vector<int> document_ids;

    for (const Posting &posting : it->second) {
        document_ids.push_back(posting.document_id);
    }

    return document_ids;
}

const PostingList &InvertedIndex::get_postings_for_processed_term(
    const std::string &term
) const {
    static const PostingList empty_postings;

    auto it = index_.find(term);

    if (it == index_.end()) {
        return empty_postings;
    }

    return it->second;
}
