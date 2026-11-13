# Search Engine V0

## Overview

A search engine built from scratch in C++.

The goal is to understand and implement the fundamental components of an information retrieval system, including:

- Text processing and tokenization
- Inverted indexing
- Ranked retrieval using TF-IDF
- Query processing

## Technologies

- C++17
- CMake
- Catch2 (testing)
- Snowball libstemmer (English Porter2 stemming)
- Snowball English stopword list

## Project Status

- Stop word removal: implemented and tested, pipeline integration pending

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Running Tests

```bash
ctest --test-dir build --output-on-failure
```
