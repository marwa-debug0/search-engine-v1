
### Text Processing Architecture

The project uses a reusable `TextProcessor` class to coordinate text preprocessing.

The processing pipeline follows this order:

1. Tokenization
2. Optional stopword filtering
3. Stemming

The processor owns and reuses its `Stemmer` and optional `StopwordFilter` instances rather than reconstructing them for every document or query. This avoids unnecessary initialization and keeps preprocessing behavior consistent.

### Configurable Stopword Handling

The project uses the Snowball English stopword list as an external linguistic resource. The list is stored locally and loaded by our own C++ implementation rather than relying on an additional NLP library.

Stopword filtering is configurable through `PreprocessingConfig`:

- `remove_stopwords` controls whether filtering is enabled.
- `stopwords_file` specifies the stopword dataset to load.

The `StopwordFilter` stores loaded stopwords in an `std::unordered_set` for efficient membership checks.

We chose this approach to keep the preprocessing implementation under our control while avoiding manually maintaining a stopword collection. Configurability also allows us to evaluate retrieval quality with and without stopword removal.

The original dataset source and licensing information are retained alongside the dataset in `data/stopwords/`.


### Inverted Index Architecture

The inverted index uses an `std::unordered_map<std::string, PostingList>`.

Each term maps to a posting list containing:
- The document ID.
- The term frequency within that document.

Documents are processed through the existing `TextProcessor` before indexing.
Term frequencies are aggregated per document before postings are inserted.
This ensures that each term has at most one posting per document.

### Document ID Uniqueness

The inverted index rejects documents whose IDs have already been indexed.

An `std::unordered_set<int>` tracks indexed document IDs. Duplicate IDs
cause `std::invalid_argument` to be thrown.

This prevents duplicate postings and protects the consistency of the index.


### Posting List Ordering

Posting lists are maintained in ascending document ID order.

The inverted index uses `std::lower_bound` to find the correct insertion
position. This invariant allows efficient two-pointer operations such
as intersection, union, and difference.

### Posting List Operations

Boolean posting-list operations return `std::vector<int>` containing
document IDs rather than `PostingList` objects.

This separates Boolean retrieval results from term-frequency information,
which remains associated with individual terms and is needed for ranked retrieval.

The engine implements:

- Intersection (A AND B)
- Union (A OR B)
- Difference (A NOT B)

All operations use two-pointer algorithms and assume sorted posting lists
with unique document IDs.

Their time complexity is O(n + m), where n and m are the sizes of the
input posting lists.


### Boolean Query Language

The search engine supports Boolean queries using `AND`, `OR`, and `NOT`, as well as parentheses for explicit grouping.

These operators are case-insensitive reserved keywords. Consequently, the literal words `and`, `or`, and `not` cannot currently be searched as ordinary terms.

Parentheses allow users to override the default operator precedence.

The precedence rules are:

1. `NOT`
2. `AND`
3. `OR`

This follows conventional Boolean expression evaluation and makes query interpretation predictable.

### Query Tokenization

A dedicated query tokenizer converts raw query strings into typed tokens.

The tokenizer recognizes:

- Ordinary search terms
- `AND`
- `OR`
- `NOT`
- Left parentheses
- Right parentheses

Operators are recognized regardless of capitalization.

Parentheses are treated as structural tokens rather than ordinary punctuation.

### Recursive-Descent Query Parser

A recursive-descent parser converts query tokens into an Abstract Syntax Tree (AST).

The grammar is structured around three levels of precedence:

- OR expressions
- AND expressions
- Unary expressions, including NOT
- Primary expressions, including terms and parenthesized expressions

This structure enforces the precedence rules without requiring a separate operator-precedence algorithm.

The parser is responsible only for syntax analysis and AST construction. Query execution is handled separately.

### AST Ownership and Representation

The AST uses a `QueryNode` structure with four node types:

- Term
- AND
- OR
- NOT

Child nodes are owned through `std::unique_ptr`.

AND and OR nodes contain left and right children. NOT nodes contain one left child. Term nodes contain a search-term value.

Exclusive ownership through `std::unique_ptr` provides automatic memory management and represents the tree structure clearly.

### Boolean Query Execution

A dedicated `BooleanQueryExecutor` evaluates AST nodes recursively.

Each node type maps to an operation:

- Term: retrieves matching document IDs from the inverted index.
- AND: intersects the results of its two children.
- OR: unions the results of its two children.
- NOT: calculates the difference between all indexed document IDs and the results of its child.

The executor reuses the existing document ID operations rather than implementing separate Boolean algorithms.

### Document Universe and NOT Semantics

The `NOT` operator requires a defined universe of documents.

The engine defines this universe as all document IDs currently present in the inverted index.

Therefore:

`NOT A` = `All indexed documents - A`

The inverted index exposes `get_all_document_ids()` to provide this universe in ascending order.

This makes complement operations deterministic and compatible with the sorted two-pointer difference algorithm.

### Consistent Query Preprocessing

Query terms must undergo the same preprocessing as document content.

The inverted index exposes `get_document_ids_for_term()`, which uses its existing `TextProcessor` to tokenize, optionally filter stopwords, and stem query terms before lookup.

This avoids inconsistencies between indexed terms and query terms.

The executor delegates term lookup to the inverted index rather than duplicating preprocessing logic.

### Boolean Operations on Document IDs

Boolean operations accept and return `std::vector<int>` rather than `PostingList`.

Posting lists contain term-frequency information associated with individual terms, while Boolean operations only need document IDs.

Separating these representations keeps Boolean retrieval independent of term-frequency data, which remains necessary for ranked retrieval.

All three operations use two-pointer algorithms and assume sorted input vectors containing unique document IDs.

Their time complexity is O(n + m).

### Query Engine Orchestration

The `execute_boolean_query()` function provides a single entry point for executing raw Boolean query strings.

It connects the query tokenizer, parser, and Boolean executor:

1. Tokenize the query.
2. Parse the tokens into an AST.
3. Evaluate the AST against the inverted index.
4. Return the matching document IDs.

Keeping these responsibilities in separate components improves modularity and makes individual stages easier to test.

### AST Validation

The Boolean executor validates AST node structure before evaluating it.

Malformed nodes, such as an AND node without both children or a NOT node without its required child, cause `std::logic_error` to be thrown.

This prevents invalid manually constructed ASTs from causing undefined behavior through null-pointer dereferencing.

## Ranked Retrieval (TF-IDF)

- TF uses logarithmic weighting: 1 + ln(tf).
- IDF uses smoothing: ln((N + 1) / (df + 1)) + 1.
- Documents are ranked using cosine similarity.
- Document vector norms include all indexed terms, not only query terms.
- Top-k results are maintained using a min-heap.
- Equal scores are resolved by ascending document ID.
- TfidfScorer accepts preprocessed query terms; query preprocessing belongs to the query layer.
