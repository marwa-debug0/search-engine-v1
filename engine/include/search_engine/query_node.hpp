#pragma once

#include <memory>
#include <string>

enum class QueryNodeType {
    Term,
    And,
    Or,
    Not
};

struct QueryNode {
    QueryNodeType type;
    std::string value;

    std::unique_ptr<QueryNode> left;
    std::unique_ptr<QueryNode> right;
};
