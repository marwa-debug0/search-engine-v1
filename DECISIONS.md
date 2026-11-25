
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
