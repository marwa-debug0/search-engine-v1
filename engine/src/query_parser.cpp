#include "../include/search_engine/query_parser.hpp"

#include <stdexcept>
#include <utility>

QueryParser::QueryParser(const std::vector<QueryToken> &tokens)
    : tokens_(tokens) {}

std::unique_ptr<QueryNode> QueryParser::parse() {
    if (tokens_.empty()) {
        throw std::runtime_error("Cannot parse an empty query");
    }

    auto result = parse_or_expression();

    if (position_ != tokens_.size()) {
        throw std::runtime_error("Unexpected token at end of query");
    }

    return result;
}

std::unique_ptr<QueryNode> QueryParser::parse_or_expression() {
    // Returns a term node
    auto left = parse_and_expression();

    // Checks whether the current node is OR
    while (match(QueryTokenType::Or)) {
        auto right = parse_and_expression();

        auto node = std::make_unique<QueryNode>();
        node->type = QueryNodeType::Or;
        node->left = std::move(left);
        node->right = std::move(right);

        // We construct our small tree and make it left to continue looking
        left = std::move(node);
    }

    return left;
}

std::unique_ptr<QueryNode> QueryParser::parse_and_expression() {
    auto left = parse_unary_expression();

    while (match(QueryTokenType::And)) {
        auto right = parse_unary_expression();

        auto node = std::make_unique<QueryNode>();
        node->type = QueryNodeType::And;
        node->left = std::move(left);
        node->right = std::move(right);

        left = std::move(node);
    }

    return left;
}

std::unique_ptr<QueryNode> QueryParser::parse_unary_expression() {
    if (match(QueryTokenType::Not)) {
        auto node = std::make_unique<QueryNode>();
        node->type = QueryNodeType::Not;
        node->left = parse_unary_expression();

        return node;
    }

    return parse_primary();
}

std::unique_ptr<QueryNode> QueryParser::parse_primary() {
    // peek - returns a pointer to the current token
    const QueryToken *token = peek();

    if (token == nullptr) {
        throw std::runtime_error("Unexpected end of query");
    }

    if (match(QueryTokenType::Term)) {
        auto node = std::make_unique<QueryNode>();
        node->type = QueryNodeType::Term;
        node->value = token->value;

        return node;
    }

    if (match(QueryTokenType::LeftParen)) {
        auto node = parse_or_expression();

        if (!match(QueryTokenType::RightParen)) {
            throw std::runtime_error("Missing closing parenthesis");
        }

        return node;
    }

    throw std::runtime_error("Expected a term or opening parenthesis");
}

bool QueryParser::match(QueryTokenType type) {
    if (position_ >= tokens_.size()) {
        return false;
    }

    if (tokens_[position_].type != type) {
        return false;
    }

    ++position_;
    return true;
}

const QueryToken *QueryParser::peek() const {
    if (position_ >= tokens_.size()) {
        return nullptr;
    }

    return &tokens_[position_];
}
