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

## Project Status

Currently setting up the development environment and project foundations.

## Building

```bash
cmake -S . -B build
cmake --build build
```

## Running Tests

```bash
ctest --test-dir build --output-on-failure
```
