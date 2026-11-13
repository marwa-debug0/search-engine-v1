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
