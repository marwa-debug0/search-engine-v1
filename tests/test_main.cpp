#include <catch2/catch_test_macros.hpp>

#include "search_engine/document.hpp"
#include "search_engine/posting_list.hpp"
#include "search_engine/index.hpp"
#include "search_engine/tokenizer.hpp"



TEST_CASE("Documents are created correctly") {
    Document doc{1, "Title", "Content"};
    REQUIRE(doc.id == 1);
    REQUIRE(doc.title == "Title");
    REQUIRE(doc.content == "Content");
}

TEST_CASE("PostingList stores multiple postings") {
    PostingList cat = {
        {0, 2},
        {3, 1},
        {7, 3}
    };

    REQUIRE(cat.size() == 3);
    REQUIRE(cat[0].document_id == 0);
    REQUIRE(cat[0].term_frequency == 2);
    REQUIRE(cat[1].document_id == 3);
    REQUIRE(cat[1].term_frequency == 1);
    REQUIRE(cat[2].document_id == 7);
    REQUIRE(cat[2].term_frequency == 3);

}

TEST_CASE("Index maps terms to posting lists") {
    Index index;
    index["computer"] = {{0, 2}, {1, 1}};

    REQUIRE(index.size() == 1);
    REQUIRE(index["computer"].size() == 2);
    REQUIRE(index["computer"][0].document_id == 0);
    REQUIRE(index["computer"][0].term_frequency == 2);

}

TEST_CASE("Tokenizer converts to lowercase") {
    auto tokens = tokenize("Hello WORLD");
    REQUIRE(tokens == std::vector<std::string>{"hello", "world"});
}


TEST_CASE("Tokenizer splits on punctuation") {
    auto tokens = tokenize("hello,world!search-engine");

    REQUIRE(tokens == std::vector<std::string>{
        "hello", "world", "search", "engine"
    });
}

TEST_CASE("Tokenizer handles numbers") {
    auto tokens = tokenize("C++ 2026");

    REQUIRE(tokens == std::vector<std::string>{"c", "2026"});
}
TEST_CASE("Tokenizer handles empty input") {
    auto tokens = tokenize("");

    REQUIRE(tokens.empty());
}

TEST_CASE("Tokenizer handles whitespaces") {
    auto tokens = tokenize("apple  banana   orange");
    REQUIRE(tokens == std::vector<std::string>{"apple", "banana", "orange"});
}

TEST_CASE("Tokenizer handles mixed content") {
    auto tokens = tokenize("Data, Science & AI 2026!");
    REQUIRE(tokens == std::vector<std::string>{
        "data", "science", "ai", "2026"
    });
}

TEST_CASE("Tokenizer handles email addresses") {
    auto tokens = tokenize("user@example.com");
    REQUIRE(tokens == std::vector<std::string>{"user", "example", "com"});
}

TEST_CASE("Tokenizer splits hyphenated words") {
    auto tokens = tokenize("well-being");
    REQUIRE(tokens == std::vector<std::string>{"well", "being"});
}

TEST_CASE("Tokenizer handles punctuation") {
    auto tokens = tokenize("!&@#()[]{}<>");
    REQUIRE(tokens.empty());
}

TEST_CASE("Tokenizer handles repeated words") {
    auto tokens = tokenize("apple banana apple orange banana apple");
    REQUIRE(tokens == std::vector<std::string>{"apple", "banana", "apple", "orange", "banana", "apple"});
}
