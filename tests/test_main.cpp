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
#include "search_engine/query_tokenizer.hpp"
#include "search_engine/query_parser.hpp"
#include "search_engine/query_tokenizer.hpp"
#include "search_engine/boolean_query_executor.hpp"
#include "search_engine/query_engine.hpp"
#include "search_engine/tfidf_scorer.hpp"






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

TEST_CASE("Find document IDs for an existing term") {
    InvertedIndex index;

    index.add_document({1, "First", "running quickly"});
    index.add_document({2, "Second", "running slowly"});
    index.add_document({3, "Third", "jumping high"});

    REQUIRE(
        index.get_document_ids_for_term("running")
        == std::vector<int>{1, 2}
    );
}

TEST_CASE("Unknown term returns empty results") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});

    REQUIRE(
        index.get_document_ids_for_term("spaceship").empty()
    );
}

TEST_CASE("Stopword returns empty results") {
    InvertedIndex index;

    index.add_document({1, "First", "the computer is useful"});

    REQUIRE(
        index.get_document_ids_for_term("the").empty()
    );
}




// Query tokenizer
TEST_CASE("QueryTokenizer recognizes terms and AND") {
    auto tokens = tokenize_query("computer AND science");

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].type == QueryTokenType::Term);
    REQUIRE(tokens[0].value == "computer");

    REQUIRE(tokens[1].type == QueryTokenType::And);

    REQUIRE(tokens[2].type == QueryTokenType::Term);
    REQUIRE(tokens[2].value == "science");
}

TEST_CASE("QueryTokenizer recognizes OR") {
    auto tokens = tokenize_query("music OR art");

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].type == QueryTokenType::Term);
    REQUIRE(tokens[0].value == "music");

    REQUIRE(tokens[1].type == QueryTokenType::Or);

    REQUIRE(tokens[2].type == QueryTokenType::Term);
    REQUIRE(tokens[2].value == "art");
}

TEST_CASE("QueryTokenizer recognizes NOT") {
    auto tokens = tokenize_query("NOT python");

    REQUIRE(tokens.size() == 2);

    REQUIRE(tokens[0].type == QueryTokenType::Not);

    REQUIRE(tokens[1].type == QueryTokenType::Term);
    REQUIRE(tokens[1].value == "python");
}

TEST_CASE("QueryTokenizer recognizes parentheses") {
    auto tokens = tokenize_query("(computer OR music)");

    REQUIRE(tokens.size() == 5);

    REQUIRE(tokens[0].type == QueryTokenType::LeftParen);

    REQUIRE(tokens[1].type == QueryTokenType::Term);
    REQUIRE(tokens[1].value == "computer");

    REQUIRE(tokens[2].type == QueryTokenType::Or);

    REQUIRE(tokens[3].type == QueryTokenType::Term);
    REQUIRE(tokens[3].value == "music");

    REQUIRE(tokens[4].type == QueryTokenType::RightParen);
}

TEST_CASE("QueryTokenizer handles uppercase and lowercase operators") {
    auto tokens = tokenize_query("COMPUTER and SCIENCE");

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].type == QueryTokenType::Term);
    REQUIRE(tokens[0].value == "computer");

    REQUIRE(tokens[1].type == QueryTokenType::And);

    REQUIRE(tokens[2].type == QueryTokenType::Term);
    REQUIRE(tokens[2].value == "science");
}

TEST_CASE("QueryTokenizer handles an empty query") {
    auto tokens = tokenize_query("");

    REQUIRE(tokens.empty());
}

TEST_CASE("QueryTokenizer handles whitespace") {
    auto tokens = tokenize_query("   computer    AND    science   ");

    REQUIRE(tokens.size() == 3);

    REQUIRE(tokens[0].value == "computer");
    REQUIRE(tokens[1].type == QueryTokenType::And);
    REQUIRE(tokens[2].value == "science");
}

// Parser
TEST_CASE("QueryParser parses a single term") {
    auto tokens = tokenize_query("computer");
    QueryParser parser(tokens);

    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->type == QueryNodeType::Term);
    REQUIRE(ast->value == "computer");
    REQUIRE(ast->left == nullptr);
    REQUIRE(ast->right == nullptr);
}

TEST_CASE("QueryParser parses an AND expression") {
    auto tokens = tokenize_query("computer AND science");
    QueryParser parser(tokens);

    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->type == QueryNodeType::And);

    REQUIRE(ast->left != nullptr);
    REQUIRE(ast->left->type == QueryNodeType::Term);
    REQUIRE(ast->left->value == "computer");

    REQUIRE(ast->right != nullptr);
    REQUIRE(ast->right->type == QueryNodeType::Term);
    REQUIRE(ast->right->value == "science");
}

TEST_CASE("QueryParser parses an OR expression") {
    auto tokens = tokenize_query("computer OR music");
    QueryParser parser(tokens);

    auto ast = parser.parse();

    REQUIRE(ast != nullptr);
    REQUIRE(ast->type == QueryNodeType::Or);

    REQUIRE(ast->left->value == "computer");
    REQUIRE(ast->right->value == "music");
}

TEST_CASE("QueryParser respects AND precedence over OR") {
    auto tokens = tokenize_query("computer OR music AND science");
    QueryParser parser(tokens);

    auto ast = parser.parse();

    REQUIRE(ast->type == QueryNodeType::Or);

    // Left side: computer
    REQUIRE(ast->left->type == QueryNodeType::Term);
    REQUIRE(ast->left->value == "computer");

    // Right side: music AND science
    REQUIRE(ast->right->type == QueryNodeType::And);
    REQUIRE(ast->right->left->value == "music");
    REQUIRE(ast->right->right->value == "science");
}

TEST_CASE("QueryParser parses NOT expressions") {
    auto tokens = tokenize_query("NOT computer");
    QueryParser parser(tokens);

    auto ast = parser.parse();

    REQUIRE(ast->type == QueryNodeType::Not);
    REQUIRE(ast->left != nullptr);
    REQUIRE(ast->left->type == QueryNodeType::Term);
    REQUIRE(ast->left->value == "computer");
}

TEST_CASE("QueryParser parses expressions inside parentheses") {
    auto tokens = tokenize_query("(computer OR music) AND science");
    QueryParser parser(tokens);

    auto ast = parser.parse();

    REQUIRE(ast->type == QueryNodeType::And);

    REQUIRE(ast->left->type == QueryNodeType::Or);
    REQUIRE(ast->left->left->value == "computer");
    REQUIRE(ast->left->right->value == "music");

    REQUIRE(ast->right->type == QueryNodeType::Term);
    REQUIRE(ast->right->value == "science");
}

TEST_CASE("QueryParser rejects an empty query") {
    std::vector<QueryToken> tokens;
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("QueryParser rejects an incomplete AND expression") {
    auto tokens = tokenize_query("computer AND");
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("QueryParser rejects unmatched parentheses") {
    auto tokens = tokenize_query("(computer OR music");
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("QueryParser rejects consecutive operators") {
    auto tokens = tokenize_query("computer OR OR music");
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("QueryParser rejects adjacent terms without an operator") {
    auto tokens = tokenize_query("computer science");
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("QueryParser rejects empty parentheses") {
    auto tokens = tokenize_query("()");
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

TEST_CASE("QueryParser rejects NOT without an operand") {
    auto tokens = tokenize_query("NOT");
    QueryParser parser(tokens);

    REQUIRE_THROWS_AS(parser.parse(), std::runtime_error);
}

// Executor
TEST_CASE("Boolean executor evaluates a term") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "music theory"});
    index.add_document({3, "Third", "computer programming"});

    BooleanQueryExecutor executor(index);

    QueryNode term;
    term.type = QueryNodeType::Term;
    term.value = "computer";

    REQUIRE(executor.execute(term) == std::vector<int>{1, 3});
}

TEST_CASE("Boolean executor evaluates AND") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});
    index.add_document({4, "Fourth", "computer science programming"});

    BooleanQueryExecutor executor(index);

    QueryNode left;
    left.type = QueryNodeType::Term;
    left.value = "computer";

    QueryNode right;
    right.type = QueryNodeType::Term;
    right.value = "science";

    QueryNode root;
    root.type = QueryNodeType::And;
    root.left = std::make_unique<QueryNode>(std::move(left));
    root.right = std::make_unique<QueryNode>(std::move(right));

    REQUIRE(executor.execute(root) == std::vector<int>{1, 4});
}
TEST_CASE("Boolean executor evaluates OR") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});
    index.add_document({4, "Fourth", "science programming"});

    BooleanQueryExecutor executor(index);

    QueryNode left;
    left.type = QueryNodeType::Term;
    left.value = "computer";

    QueryNode right;
    right.type = QueryNodeType::Term;
    right.value = "science";

    QueryNode root;
    root.type = QueryNodeType::Or;
    root.left = std::make_unique<QueryNode>(std::move(left));
    root.right = std::make_unique<QueryNode>(std::move(right));

    REQUIRE(executor.execute(root) == std::vector<int>{1, 2, 4});
}

TEST_CASE("Boolean executor evaluates NOT") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "music theory"});
    index.add_document({3, "Third", "computer programming"});
    index.add_document({4, "Fourth", "art history"});

    BooleanQueryExecutor executor(index);

    QueryNode term;
    term.type = QueryNodeType::Term;
    term.value = "computer";

    QueryNode root;
    root.type = QueryNodeType::Not;
    root.left = std::make_unique<QueryNode>(std::move(term));

    REQUIRE(executor.execute(root) == std::vector<int>{2, 4});
}

TEST_CASE("Boolean executor evaluates nested expressions") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});
    index.add_document({4, "Fourth", "science programming"});

    BooleanQueryExecutor executor(index);

    QueryNode computer;
    computer.type = QueryNodeType::Term;
    computer.value = "computer";

    QueryNode music;
    music.type = QueryNodeType::Term;
    music.value = "music";

    QueryNode not_music;
    not_music.type = QueryNodeType::Not;
    not_music.left = std::make_unique<QueryNode>(std::move(music));

    QueryNode root;
    root.type = QueryNodeType::And;
    root.left = std::make_unique<QueryNode>(std::move(computer));
    root.right = std::make_unique<QueryNode>(std::move(not_music));

    REQUIRE(executor.execute(root) == std::vector<int>{1});
}

// Query engine
TEST_CASE("Query engine executes a complete Boolean query") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});
    index.add_document({4, "Fourth", "science programming"});

    REQUIRE(
        execute_boolean_query("computer AND NOT music", index)
        == std::vector<int>{1}
    );
}
TEST_CASE("Query engine respects operator precedence") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});
    index.add_document({4, "Fourth", "science programming"});

    REQUIRE(
        execute_boolean_query("computer OR music AND science", index)
        == std::vector<int>{1, 2}
    );
}
TEST_CASE("Query engine respects parentheses") {
    InvertedIndex index;

    index.add_document({1, "First", "computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});
    index.add_document({4, "Fourth", "science programming"});

    REQUIRE(
        execute_boolean_query("(computer OR music) AND science", index)
        == std::vector<int>{1}
    );
}

// Return postings
TEST_CASE("InvertedIndex returns postings for a processed term") {
    InvertedIndex index;

    index.add_document({1, "First", "computer computer science"});
    index.add_document({2, "Second", "computer music"});
    index.add_document({3, "Third", "music theory"});

    TextProcessor processor;
    auto processed_terms = processor.process("computer");

    REQUIRE(processed_terms.size() == 1);

    const PostingList &postings =
        index.get_postings_for_processed_term(processed_terms.front());

    REQUIRE(postings.size() == 2);
    REQUIRE(postings[0].document_id == 1);
    REQUIRE(postings[0].term_frequency == 2);
    REQUIRE(postings[1].document_id == 2);
    REQUIRE(postings[1].term_frequency == 1);
}

TEST_CASE("InvertedIndex returns empty postings for unknown term") {
    InvertedIndex index;

    REQUIRE(
        index.get_postings_for_processed_term("unknown").empty()
    );
}


// TF-IDF scoring
TEST_CASE("TF-IDF ranks the most relevant document first") {
    InvertedIndex index;

    index.add_document({1, "First", "computer computer science"});
    index.add_document({2, "Second", "computer"});
    index.add_document({3, "Third", "music theory"});

    TextProcessor processor;
    auto query = processor.process("computer");

    TfidfScorer scorer(index);
    auto results = scorer.search(query, 10);

    REQUIRE(results.size() == 2);
    REQUIRE(results[0].document_id == 2);
    REQUIRE(results[1].document_id == 1);
    REQUIRE(results[0].score > results[1].score);
}

TEST_CASE("TF-IDF returns empty results for unknown terms") {
    InvertedIndex index;
    index.add_document({1, "First", "computer science"});

    TextProcessor processor;
    auto query = processor.process("astronaut");

    TfidfScorer scorer(index);
    REQUIRE(scorer.search(query, 10).empty());
}

TEST_CASE("TF-IDF respects top-k limit") {
    InvertedIndex index;

    index.add_document({1, "First", "computer"});
    index.add_document({2, "Second", "computer"});
    index.add_document({3, "Third", "computer"});

    TextProcessor processor;
    auto query = processor.process("computer");

    TfidfScorer scorer(index);
    auto results = scorer.search(query, 2);

    REQUIRE(results.size() == 2);
}
