#pragma once

#include <string>
#include <unordered_map>

#include "posting_list.hpp"

using Index = std::unordered_map<std::string, PostingList>;
