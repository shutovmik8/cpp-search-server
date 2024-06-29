#pragma once
#include <deque>
#include <string>
#include <vector>
#include "search_server.h"
#include "document.h"

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer&);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        std::vector<Document> ret = server.FindTopDocuments(raw_query, document_predicate);
        current_time += 1;
        bool empty = ret.empty();
        if (empty) {
            ++quant_no_result;
        }
        requests_.push_back({empty, current_time});
        if (current_time > min_in_day_) {
            if (requests_.front().no_result) {
                --quant_no_result;
            }
            requests_.pop_front();
        }
        return ret;
    }
    
    std::vector<Document> AddFindRequest(const std::string&, DocumentStatus); 
    std::vector<Document> AddFindRequest(const std::string&);
    int GetNoResultRequests() const;
    
private:
    struct QueryResult {
        bool no_result;
        int time;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    const SearchServer& server;
    int current_time = 0;
    int quant_no_result = 0;
};