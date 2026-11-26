#include "../include/search_engine/boolean_query_executor.hpp"
#include "../include/search_engine/posting_list_ops.hpp"

#include <stdexcept>

BooleanQueryExecutor::BooleanQueryExecutor(InvertedIndex &index)
    : index_(index) {}

std::vector<int> BooleanQueryExecutor::execute(const QueryNode &root) {
    return evaluate(root);
}

std::vector<int> BooleanQueryExecutor::evaluate(const QueryNode &node) {
    switch (node.type) {
        case QueryNodeType::Term:
            if (node.left || node.right || node.value.empty()) {
                throw std::logic_error("Invalid Term node");
            }

            return index_.get_document_ids_for_term(node.value);

        case QueryNodeType::And: {
            if (!node.left || !node.right) {
                throw std::logic_error("Invalid AND node");
            }

            auto left_results = evaluate(*node.left);
            auto right_results = evaluate(*node.right);

            return intersect_posting_lists(left_results, right_results);
        }

        case QueryNodeType::Or: {
            if (!node.left || !node.right) {
                throw std::logic_error("Invalid OR node");
            }

            auto left_results = evaluate(*node.left);
            auto right_results = evaluate(*node.right);

            return union_posting_lists(left_results, right_results);
        }

        case QueryNodeType::Not: {
            if (!node.left || node.right) {
                throw std::logic_error("Invalid NOT node");
            }

            auto all_documents = index_.get_all_document_ids();
            auto excluded_documents = evaluate(*node.left);

            return difference_posting_lists(
                all_documents,
                excluded_documents
            );
        }

        default:
            throw std::logic_error("Unknown query node type");
    }
}
