#pragma once

#include <string>
#include <vector>

#include "../search_engine/query_token.hpp"

std::vector<QueryToken> tokenize_query(const std::string &query);
