#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

using namespace std;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result = 0;
    cin >> result;
    ReadLine();
    return result;
}

vector<string> SplitIntoWords(const string& text) {
    vector<string> words;
    string word;
    for (const char c : text) {
        if (c == ' ') {
            if (!word.empty()) {
                words.push_back(word);
                word.clear();
            }
        } else {
            word += c;
        }
    }
    if (!word.empty()) {
        words.push_back(word);
    }
    return words;
}

struct Document {
    int id;
    double relevance;
};

class SearchServer {
public:
    void SetStopWords(const string& text) {
        for (const string& word : SplitIntoWords(text)) {
            stop_words_.insert(word);
        }
    }

    void AddDocument(int document_id, const string& document) {
        const vector<string> words = SplitIntoWordsNoStop(document);
        int size = words.size();
        for (const string& word: words) {
                documents_[word][document_id] += 1. / size;
        }
        ++document_count_;    
    }

    vector<Document> FindTopDocuments(const string& raw_query) const {
        const Query query = ParseQuery(raw_query);
        vector<Document> matched_documents = FindAllDocuments(query);

        sort(matched_documents.begin(), matched_documents.end(), 
            [](const Document& lhs, const Document& rhs) { return lhs.relevance > rhs.relevance; });
        
        if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
            matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
        }
        return matched_documents;
    }

private:

    struct QueryWord {
        string data;
        bool is_minus;
        bool is_stop;
    };
    
    struct Query {
        set<string> plus;
        set<string> minus;
    };

    map<string, map<int, double>> documents_;
    int document_count_ = 0;
    set<string> stop_words_;

    bool IsStopWord(const string& word) const {
        return stop_words_.count(word) > 0;
    }

    vector<string> SplitIntoWordsNoStop(const string& text) const {
        vector<string> words;
        for (const string& word : SplitIntoWords(text)) {
            if (!IsStopWord(word)) {
                words.push_back(word);
            }
        }
        return words;
    }

    QueryWord ParseQueryWord(string text) const {
        bool is_minus = false;
        if (text[0] == '-') {
            is_minus = true;
            text = text.substr(1);
        }
        return {text, is_minus, IsStopWord(text)};
    }
    
    Query ParseQuery(const string& text) const {
        Query query;
        for (const string& word : SplitIntoWords(text)) {
            const QueryWord query_word = ParseQueryWord(word);
            if (!query_word.is_stop) {
                if (query_word.is_minus) {
                    query.minus.insert(query_word.data);
                } 
                else {
                    query.plus.insert(query_word.data);
                }
            }
        }
        return query;
    }

    vector<Document> FindAllDocuments(const Query& query_words) const {
        map<int, double> tmp_document_relevance;
        vector<Document> document_relevance;

        for (const string& word : query_words.plus) {
            if (documents_.count(word)) {
                int size = documents_.at(word).size();
                double word_idf = log(static_cast<double>(document_count_) / size);
                
                for (const auto& doc_id_with_tf : documents_.at(word)) {                     
                    if (tmp_document_relevance.count(doc_id_with_tf.first)) {
                        tmp_document_relevance[doc_id_with_tf.first] += doc_id_with_tf.second * word_idf;
                    }
                    else {
                        tmp_document_relevance.insert({doc_id_with_tf.first, doc_id_with_tf.second * word_idf});
                    }
                }
            }
        }

        for (const string& word : query_words.minus) {
            if (documents_.count(word)) {
                for (const auto& doc_id_with_tf : documents_.at(word)) {
                    if (tmp_document_relevance.count(doc_id_with_tf.first)) {
                        tmp_document_relevance.erase(doc_id_with_tf.first);
                    }
                }    
            }
        }
        

        for (const auto& [key, value] : tmp_document_relevance){
            document_relevance.push_back({key, value});
        }
        return document_relevance;
    }
};

SearchServer CreateSearchServer() {
    SearchServer search_server;
    search_server.SetStopWords(ReadLine());
    const int document_count = ReadLineWithNumber();

    for (int document_id = 0; document_id < document_count; ++document_id) {
        search_server.AddDocument(document_id, ReadLine());
    }
    return search_server;
}

int main() {
    const SearchServer search_server = CreateSearchServer();
    const string query = ReadLine();
    
    for (const auto& [document_id, relevance] : search_server.FindTopDocuments(query)) {
        cout << "{ document_id = "s << document_id << ", "
             << "relevance = "s << relevance << " }"s << endl;
    }
    
}
