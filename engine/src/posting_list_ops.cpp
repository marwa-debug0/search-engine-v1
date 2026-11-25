#include "../include/search_engine/posting_list_ops.hpp"

std::vector<int> intersect_posting_lists(
    const PostingList &first,
    const PostingList &second
) {
    std::vector<int> result;

    auto a = first.begin();
    auto b = second.begin();

    while (a != first.end() && b != second.end()) {

        if (a->document_id == b->document_id) {
            result.push_back(a->document_id);
            ++a;
            ++b;
        }
        else if (a->document_id < b->document_id) {
            ++a;
        }
        else {
            ++b;
        }
    }

    return result;
}


std::vector<int> union_posting_lists(
    const PostingList &first,
    const PostingList &second
) {
    std::vector<int> result;

    auto a = first.begin();
    auto b = second.begin();

    while (a != first.end() && b != second.end()) {

        if (a->document_id == b->document_id) {
            result.push_back(a->document_id);
            ++a;
            ++b;
        }
        else if (a->document_id < b->document_id) {
            result.push_back(a->document_id);
            ++a;
        }
        else {
            result.push_back(b->document_id);
            ++b;
        }
    }

    while (a != first.end()) {
        result.push_back(a->document_id);
        ++a;
    }

    while (b != second.end()) {
        result.push_back(b->document_id);
        ++b;
    }

    return result;
}


std::vector<int> difference_posting_lists(
    const PostingList &first,
    const PostingList &second
) {
    std::vector<int> result;

    auto a = first.begin();
    auto b = second.begin();

    while (a != first.end() && b != second.end()) {

        if (a->document_id == b->document_id) {
            ++a;
            ++b;
        }
        else if (a->document_id < b->document_id) {
            result.push_back(a->document_id);
            ++a;
        }
        else {
            ++b;
        }
    }

    while (a != first.end()) {
        result.push_back(a->document_id);
        ++a;
    }

    return result;
}
