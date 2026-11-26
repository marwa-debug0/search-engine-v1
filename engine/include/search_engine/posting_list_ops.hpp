#pragma once

#include <vector>
#include "../search_engine/posting_list.hpp"

std::vector<int> intersect_posting_lists(
    const std::vector<int> &first,
    const std::vector<int> &second
);

std::vector<int> union_posting_lists(
    const std::vector<int> &first,
    const std::vector<int> &second
);

std::vector<int> difference_posting_lists(
    const std::vector<int> &first,
    const std::vector<int> &second
);
