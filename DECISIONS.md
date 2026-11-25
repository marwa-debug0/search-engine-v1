
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
