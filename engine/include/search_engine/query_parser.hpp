#pragma once

#include <memory>
#include <vector>

#include "../search_engine/query_node.hpp"
#include "../search_engine/query_token.hpp"

class QueryParser {
public:
    explicit QueryParser(const std::vector<QueryToken> &tokens);

    std::unique_ptr<QueryNode> parse();

private:
    // A reference to our tokens since we dont need to copy them
    const std::vector<QueryToken> &tokens_;
    std::size_t position_ = 0;

    std::unique_ptr<QueryNode> parse_or_expression();
    std::unique_ptr<QueryNode> parse_and_expression();
    std::unique_ptr<QueryNode> parse_unary_expression();
    std::unique_ptr<QueryNode> parse_primary();

    bool match(QueryTokenType type);
    const QueryToken *peek() const;
};
