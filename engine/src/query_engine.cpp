#include "../include/search_engine/query_engine.hpp"

#include "../include/search_engine/query_tokenizer.hpp"
#include "../include/search_engine/query_parser.hpp"
#include "../include/search_engine/boolean_query_executor.hpp"

std::vector<int> execute_boolean_query(
    const std::string &query,
    InvertedIndex &index
) {
    std::vector<QueryToken> tokens = tokenize_query(query);

    QueryParser parser(tokens);

    std::unique_ptr<QueryNode> root = parser.parse();

    BooleanQueryExecutor executor(index);

    return executor.execute(*root);
}
