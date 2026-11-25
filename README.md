
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

The text preprocessing pipeline is implemented through a reusable `TextProcessor` class.

Processing order:

1. Tokenization
2. Optional stopword filtering
3. Stemming

Stopword filtering can be enabled or disabled through `PreprocessingConfig`, and the stopword dataset can be selected using a file path.


The inverted index maps each processed term to a posting list containing
the document IDs and term frequencies of documents in which the term appears.

Each document ID can only be indexed once. Duplicate document IDs are rejected.

## Project Status

### Milestone 0 — Project Foundations
- Python prototype
- C++ project setup
- CMake build system
- Catch2 testing
- Docker-based build environment
- GitHub Actions CI

### Milestone 1 — Core Search Engine

- [x] T1.1 Core data structures
- [x] T1.2 Tokenizer
- [x] T1.3 Stemming
- [x] T1.4 Configurable stopword filtering and text preprocessing
- [x] T1.5 Inverted index
- [ ] T1.6 Posting list operations
- [ ] T1.7 Boolean query parser and executor
- [ ] T1.8 Ranked retrieval (TF-IDF)
- [ ] T1.9 Property testing
- [ ] T1.10 JSONL loader and CLI
- [ ] T1.11 Profiling

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Running Tests

```bash
ctest --test-dir build --output-on-failure
```
