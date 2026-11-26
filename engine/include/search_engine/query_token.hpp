#pragma once

#include <string>

enum class QueryTokenType {
    Term,
    And,
    Or,
    Not,
    LeftParen,
    RightParen
};

struct QueryToken {
    QueryTokenType type;
    std::string value;
};
