#pragma once

#include <string>
#include <vector>

#include "../search_engine/stemmer.hpp"

std::vector<std::string> preprocess_text(
    const std::string& text,
    Stemmer &stemmer
);
// We don't want to recreate the stemmer everytime so we pass it as a parameter.
