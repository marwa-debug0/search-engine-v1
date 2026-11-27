
#include "../include/search_engine/tfidf_scorer.hpp"

#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>

TfidfScorer::TfidfScorer(const InvertedIndex &index)
    : index_(index) {}

std::vector<SearchResult> TfidfScorer::search(
    const std::vector<std::string> &processed_query_terms,
    std::size_t top_k
) {
    if (top_k == 0 || processed_query_terms.empty()) {
        return {};
    }

    const double total_documents =
        static_cast<double>(index_.get_all_document_ids().size());

    if (total_documents == 0) {
        return {};
    }

    // Count query term frequencies.
    std::unordered_map<std::string, int> query_frequencies;

    for (const std::string &term : processed_query_terms) {
        ++query_frequencies[term];
    }

    // Calculate document vector norms.
    std::unordered_map<int, double> document_norms;

    for (const auto &[term, postings] : index_.get_index()) {
        const double df = static_cast<double>(postings.size());

        const double idf =
            std::log((total_documents + 1.0) / (df + 1.0)) + 1.0;

        for (const Posting &posting : postings) {
            const double tf =
                1.0 + std::log(static_cast<double>(posting.term_frequency));

            const double weight = tf * idf;

            document_norms[posting.document_id] += weight * weight;
        }
    }

    // Calculate query norm and document-query dot products.
    std::unordered_map<int, double> dot_products;
    double query_norm_squared = 0.0;

    for (const auto &[term, frequency] : query_frequencies) {
        const PostingList &postings =
            index_.get_postings_for_processed_term(term);

        if (postings.empty()) {
            continue;
        }

        const double df = static_cast<double>(postings.size());

        const double idf =
            std::log((total_documents + 1.0) / (df + 1.0)) + 1.0;

        const double query_tf =
            1.0 + std::log(static_cast<double>(frequency));

        const double query_weight = query_tf * idf;

        query_norm_squared += query_weight * query_weight;

        for (const Posting &posting : postings) {
            const double document_tf =
                1.0 + std::log(
                    static_cast<double>(posting.term_frequency)
                );

            const double document_weight = document_tf * idf;

            dot_products[posting.document_id] +=
                document_weight * query_weight;
        }
    }

    if (query_norm_squared == 0.0) {
        return {};
    }

    // Keep only the top-k results using a min-heap.
    struct MinScoreFirst {
        bool operator()(
            const SearchResult &a,
            const SearchResult &b
        ) const {
            if (a.score != b.score) {
                return a.score > b.score;
            }
            // For equal scores, the larger ID is the worse result.
            return a.document_id < b.document_id;
        }
    };

    std::priority_queue<
        SearchResult,
        std::vector<SearchResult>,
        MinScoreFirst
    > top_results;

    const double query_norm = std::sqrt(query_norm_squared);

    for (const auto &[document_id, dot_product] : dot_products) {
        const double document_norm =
            std::sqrt(document_norms.at(document_id));

        const double score =
            dot_product / (query_norm * document_norm);

        top_results.push({document_id, score});

        if (top_results.size() > top_k) {
            top_results.pop();
        }
    }

    // Return results in descending score order.
    std::vector<SearchResult> results;

    while (!top_results.empty()) {
        results.push_back(top_results.top());
        top_results.pop();
    }

    std::sort(
        results.begin(),
        results.end(),
        [](const SearchResult &a, const SearchResult &b) {
            if (a.score != b.score) {
                return a.score > b.score;
            }
            return a.document_id < b.document_id;
        }
    );

    return results;
}
