# Search Engine V0

## Overview

A search engine built from scratch in C++.

The goal is to understand and implement the fundamental components of an information retrieval system, including:

- Text processing and tokenization
- Inverted indexing
- Boolean query processing
- Ranked retrieval using TF-IDF
- Efficient document retrieval

## Technologies

- C++17
- CMake
- Catch2 (testing)
- Snowball libstemmer (English Porter2 stemming)
- Snowball English stopword list

## Architecture

### Text Processing

The text preprocessing pipeline is implemented through a reusable `TextProcessor` class.

Processing order:

1. Tokenization
2. Optional stopword filtering
3. Stemming

Stopword filtering can be enabled or disabled through `PreprocessingConfig`, and the stopword dataset can be selected using a file path.

The same preprocessing pipeline is used for document indexing and query-term lookup to ensure consistent term representation.

### Inverted Index

The inverted index maps each processed term to a posting list containing the document IDs and term frequencies of documents in which the term appears.

Each document ID can only be indexed once. Duplicate document IDs are rejected.

The index also provides access to all indexed document IDs, which is necessary for evaluating `NOT` queries.

### Posting List Operations

The engine supports three Boolean operations on sorted document ID vectors:

- Intersection: retrieves document IDs present in both lists.
- Union: combines document IDs from both lists without duplicates.
- Difference: retrieves document IDs present in the first list but not the second.

These operations use two-pointer algorithms with O(n + m) time complexity.

### Boolean Query Processing

The engine supports Boolean queries using:

- `AND`
- `OR`
- `NOT`
- Parentheses for explicit grouping

Operators are case-insensitive reserved keywords.

Operator precedence:

1. `NOT`
2. `AND`
3. `OR`

A recursive-descent parser converts queries into an Abstract Syntax Tree (AST). A separate Boolean query executor evaluates the AST against the inverted index.

`NOT` retrieves the complement of a result set relative to all indexed document IDs.

The complete query pipeline is:

1. Query tokenization
2. AST construction through parsing
3. Recursive Boolean expression evaluation
4. Return of matching document IDs

The query engine exposes `execute_boolean_query
