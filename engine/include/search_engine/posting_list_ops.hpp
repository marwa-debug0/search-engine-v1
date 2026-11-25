#pragma once

#include <vector>
#include "../search_engine/posting_list.hpp"

std::vector<int> intersect_posting_lists(
    const PostingList &first,
    const PostingList &second
);

std::vector<int> union_posting_lists(
    const PostingList &first,
    const PostingList &second
);


std::vector<int> difference_posting_lists(
    const PostingList &first,
    const PostingList &second
);
