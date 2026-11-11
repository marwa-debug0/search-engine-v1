#include <catch2/catch_test_macros.hpp>

#include "search_engine/document.hpp"
#include "search_engine/posting_list.hpp"
#include "search_engine/index.hpp"



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
