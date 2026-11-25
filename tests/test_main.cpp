#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "search_engine/document.hpp"
#include "search_engine/posting_list.hpp"
#include "search_engine/index.hpp"
#include "search_engine/tokenizer.hpp"
#include "search_engine/stemmer.hpp"
#include "search_engine/text_processor.hpp"
#include "search_engine/stopword_filter.hpp"
#include "search_engine/inverted_index.hpp"
#include "search_engine/posting_list_ops.hpp"




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


// Stemmer
TEST_CASE("Stemmer reduces related English words") {
    Stemmer stemmer;

    REQUIRE(stemmer.stem("running") == "run");
    REQUIRE(stemmer.stem("runs") == "run");
    REQUIRE(stemmer.stem("studies") == "studi");
    REQUIRE(stemmer.stem("connected") == "connect");
    REQUIRE(stemmer.stem("connections") == "connect");
}


// Processor
TEST_CASE("Text preprocessing tokenizes, removes stopwords and stems") {
    TextProcessor processor;

    std::vector<std::string> result = processor.process("Running connections and studies");

    REQUIRE(result == std::vector<std::string>{"run", "connect","studi"});
}

TEST_CASE("Text processor can disable stopword removal") {
    PreprocessingConfig config;
    config.remove_stopwords = false;

    TextProcessor processor(config);

    std::vector<std::string> result =
        processor.process("Running connections and studies");

    REQUIRE(result == std::vector<std::string>{
        "run", "connect", "and", "studi"});
}

TEST_CASE("TextProcessor can use a custom stopword file") {
    PreprocessingConfig config;
    config.stopwords_file = "tests/fixtures/custom_stopwords.txt";

    TextProcessor processor(config);

    std::vector<std::string> result =
        processor.process("computer running");

    REQUIRE(result == std::vector<std::string>{"run"});
}

// Stopword filter

TEST_CASE("Stopword filter removes English stopwords") {
    StopwordFilter filter("data/stopwords/english.txt");

    std::vector<std::string> tokens = {
        "the", "cat", "and", "dog", "running"
    };

    std::vector<std::string> result = filter.filter(tokens);

    REQUIRE(result == std::vector<std::string>{
        "cat", "dog", "running"
    });
}

TEST_CASE("Stopword filter preserves words not in the dataset") {
    StopwordFilter filter("data/stopwords/english.txt");

    std::vector<std::string> tokens = {
        "computer", "search", "engine"
    };

    REQUIRE(filter.filter(tokens) == tokens);
}

TEST_CASE("Stopword filter handles empty input") {
    StopwordFilter filter("data/stopwords/english.txt");

    std::vector<std::string> tokens;

    REQUIRE(filter.filter(tokens).empty());
}

TEST_CASE("Stopword filter throws when dataset is missing") {
    REQUIRE_THROWS_AS(
        StopwordFilter("data/stopwords/nonexistent.txt"),
        std::runtime_error
    );
}

// Inverted index
TEST_CASE("InvertedIndex indexes terms and counts their frequencies") {
    InvertedIndex inverted_index;

    Document document{
        1,
        "Animals",
        "cat dog cat cat"
    };

    inverted_index.add_document(document);

    const Index &index = inverted_index.get_index();

    REQUIRE(index.size() == 2);

    REQUIRE(index.at("cat").size() == 1);
    REQUIRE(index.at("cat")[0].document_id == 1);
    REQUIRE(index.at("cat")[0].term_frequency == 3);

    REQUIRE(index.at("dog").size() == 1);
    REQUIRE(index.at("dog")[0].document_id == 1);
    REQUIRE(index.at("dog")[0].term_frequency == 1);
}

TEST_CASE("InvertedIndex stores postings across multiple documents") {
    InvertedIndex inverted_index;

    Document first{
        1,
        "First",
        "cat dog cat"
    };

    Document second{
        2,
        "Second",
        "dog bird"
    };

    inverted_index.add_document(first);
    inverted_index.add_document(second);

    const Index &index = inverted_index.get_index();

    REQUIRE(index.at("cat").size() == 1);
    REQUIRE(index.at("dog").size() == 2);
    REQUIRE(index.at("bird").size() == 1);

    REQUIRE(index.at("dog")[0].document_id == 1);
    REQUIRE(index.at("dog")[0].term_frequency == 1);

    REQUIRE(index.at("dog")[1].document_id == 2);
    REQUIRE(index.at("dog")[1].term_frequency == 1);
}

TEST_CASE("InvertedIndex rejects duplicate document IDs") {
    InvertedIndex inverted_index;

    Document first{1, "First", "cat dog"};
    Document duplicate{1, "Second", "bird fish"};

    inverted_index.add_document(first);

    REQUIRE_THROWS_AS(
        inverted_index.add_document(duplicate),
        std::invalid_argument
    );

    const Index &index = inverted_index.get_index();

    REQUIRE(index.count("cat") == 1);
    REQUIRE(index.count("bird") == 0);
}

TEST_CASE("InvertedIndex keeps postings sorted by document ID") {
    InvertedIndex inverted_index;

    inverted_index.add_document({5, "Fifth", "cat"});
    inverted_index.add_document({1, "First", "cat"});
    inverted_index.add_document({3, "Third", "cat"});

    const PostingList &postings =
        inverted_index.get_index().at("cat");

    REQUIRE(postings.size() == 3);
    REQUIRE(postings[0].document_id == 1);
    REQUIRE(postings[1].document_id == 3);
    REQUIRE(postings[2].document_id == 5);
}

// Intersection
TEST_CASE("Intersection finds common document IDs") {
    PostingList first = {
        {1, 2},
        {3, 1},
        {5, 4},
        {8, 2}
    };

    PostingList second = {
        {2, 1},
        {3, 2},
        {5, 1},
        {9, 3}
    };

    REQUIRE(
        intersect_posting_lists(first, second)
        == std::vector<int>{3, 5}
    );
}

TEST_CASE("Intersection returns empty when there are no matches") {
    PostingList first = {{1, 1}, {3, 2}};
    PostingList second = {{2, 1}, {4, 1}};

    REQUIRE(intersect_posting_lists(first, second).empty());
}

TEST_CASE("Intersection handles empty posting lists") {
    PostingList first;
    PostingList second = {{1, 1}, {2, 1}};

    REQUIRE(intersect_posting_lists(first, second).empty());
}


// Union
TEST_CASE("Union combines document IDs without duplicates") {
    PostingList first = {
        {1, 2},
        {3, 1},
        {5, 4},
        {8, 2}
    };

    PostingList second = {
        {2, 1},
        {3, 2},
        {5, 1},
        {9, 3}
    };

    REQUIRE(
        union_posting_lists(first, second)
        == std::vector<int>{1, 2, 3, 5, 8, 9}
    );
}

TEST_CASE("Union handles disjoint posting lists") {
    PostingList first = {{1, 1}, {3, 1}};
    PostingList second = {{2, 1}, {4, 1}};

    REQUIRE(
        union_posting_lists(first, second)
        == std::vector<int>{1, 2, 3, 4}
    );
}

TEST_CASE("Union handles an empty posting list") {
    PostingList first;
    PostingList second = {{1, 1}, {3, 2}};

    REQUIRE(
        union_posting_lists(first, second)
        == std::vector<int>{1, 3}
    );
}


// Difference
TEST_CASE("Difference excludes IDs present in the second list") {
    PostingList first = {
        {1, 2},
        {3, 1},
        {5, 4},
        {8, 2}
    };

    PostingList second = {
        {2, 1},
        {3, 2},
        {5, 1},
        {9, 3}
    };

    REQUIRE(
        difference_posting_lists(first, second)
        == std::vector<int>{1, 8}
    );
}

TEST_CASE("Difference returns all IDs when lists are disjoint") {
    PostingList first = {{1, 1}, {3, 1}};
    PostingList second = {{2, 1}, {4, 1}};

    REQUIRE(
        difference_posting_lists(first, second)
        == std::vector<int>{1, 3}
    );
}

TEST_CASE("Difference returns empty when the first list is empty") {
    PostingList first;
    PostingList second = {{1, 1}, {2, 1}};

    REQUIRE(difference_posting_lists(first, second).empty());
}

TEST_CASE("Difference returns empty when both lists are identical") {
    PostingList first = {{1, 1}, {3, 2}};
    PostingList second = {{1, 4}, {3, 1}};

    REQUIRE(difference_posting_lists(first, second).empty());
}
