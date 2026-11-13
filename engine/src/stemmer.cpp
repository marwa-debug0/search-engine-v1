#include "../include/search_engine/stemmer.hpp"
#include <libstemmer.h>
#include <stdexcept>

Stemmer::Stemmer() {
    stemmer_ = sb_stemmer_new("english", nullptr);

    if (stemmer_ == nullptr) {
        throw std::runtime_error("Failed to create Snowball stemmer");
    }

}

Stemmer::~Stemmer() {
    sb_stemmer_delete(stemmer_);
}

std::string Stemmer::stem(const std::string &word) {
    const sb_symbol *result = sb_stemmer_stem(
        stemmer_,
        reinterpret_cast<const sb_symbol *>(word.data()),
        static_cast<int>(word.size())
    );

    if (result == nullptr) {
        throw std::runtime_error("Snowball stemming failed");
    }

    return std::string(
        reinterpret_cast<const char *>(result),
        sb_stemmer_length(stemmer_)
    );
}
