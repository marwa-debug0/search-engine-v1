#pragma once

#include <string>

class Stemmer {
public:
    Stemmer();
    ~Stemmer();

    Stemmer(const Stemmer &) = delete;
    Stemmer &operator=(const Stemmer &) = delete;

    std::string stem(const std::string &word);

    private:
        struct sb_stemmer *stemmer_;

};
