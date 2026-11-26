#pragma once

#include <vector>
#include <memory>

#include "../search_engine/query_node.hpp"
#include "../search_engine/inverted_index.hpp"

class BooleanQueryExecutor {
public:
    explicit BooleanQueryExecutor(InvertedIndex &index);

    std::vector<int> execute(const QueryNode &root);

private:
    InvertedIndex &index_;

    std::vector<int> evaluate(const QueryNode &node);
};
