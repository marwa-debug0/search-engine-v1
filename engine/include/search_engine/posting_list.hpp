#pragma once

#include <vector>

struct Posting {
    int document_id;
    int term_frequency;
};

using PostingList = std::vector<Posting>;
