#pragma once

#include <string>
#include <vector>

#include "../search_engine/inverted_index.hpp"

std::vector<int> execute_boolean_query(
    const std::string &query,
    InvertedIndex &index
);
