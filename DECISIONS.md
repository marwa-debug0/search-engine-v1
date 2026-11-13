### Tokenizer  
- Handles Lowercase conversion.
- Handles Punctuation and whitespace.
- Handles Numbers and alphanumeric combinations.
- Handles Email addresses and hyphenated words.
- Handles Empty and punctuation-only input.
- Handles Repeated words while preserving term frequency information.


### Stemming Library

The project uses **Snowball's libstemmer** library for English stemming, specifically the English algorithm (Porter2). We chose this established implementation over writing a stemmer from scratch to benefit from a tested, maintained algorithm while keeping control over our indexing and retrieval architecture. The library is integrated into the C++ project through a small wrapper.



### Connecting tokenization and stemming : 
We will connect tokenizer and stemmer using a pipeline (text_processor.cpp), so when we feed a query to the search engine , it will first go through the tokenizer and then the stemmer.


### Stop word handling : 
We will use an externally maintained stopword dataset. It will be stored locally in a txt file loaded into the search engine at runtime.
Stopword filtering will be implemented and can be configured so that search quality can be evaluated with and without it. The dataset's source and license will be documented in the data/stopwords/README.md.

We will update our pipeline to: Text → Tokenization → Stopword filtering → Stemming
