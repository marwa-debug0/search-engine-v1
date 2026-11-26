#include "../include/search_engine/posting_list_ops.hpp"

std::vector<int> intersect_posting_lists(
    const std::vector<int> &first,
    const std::vector<int> &second
) {
    std::vector<int> result;
    auto a = first.begin();
    auto b = second.begin();

    while (a != first.end() && b != second.end()) {
        if (*a == *b) {
            result.push_back(*a);
            ++a;
            ++b;
        } else if (*a < *b) {
            ++a;
        } else {
            ++b;
        }
    }
    return result;
}

std::vector<int> union_posting_lists(
    const std::vector<int> &first,
    const std::vector<int> &second
) {
    std::vector<int> result;
    auto a = first.begin();
    auto b = second.begin();

    while (a != first.end() && b != second.end()) {
        if (*a == *b) {
            result.push_back(*a);
            ++a;
            ++b;
        } else if (*a < *b) {
            result.push_back(*a++);
        } else {
            result.push_back(*b++);
        }
    }

    result.insert(result.end(), a, first.end());
    result.insert(result.end(), b, second.end());

    return result;
}

std::vector<int> difference_posting_lists(
    const std::vector<int> &first,
    const std::vector<int> &second
) {
    std::vector<int> result;
    auto a = first.begin();
    auto b = second.begin();

    while (a != first.end() && b != second.end()) {
        if (*a == *b) {
            ++a;
            ++b;
        } else if (*a < *b) {
            result.push_back(*a++);
        } else {
            ++b;
        }
    }

    result.insert(result.end(), a, first.end());
    return result;
}
