#pragma once
#include <iostream>
#include <iterator>
#include <vector>

template <typename Iterator>
class IteratorRange {
    Iterator begin_;
    Iterator end_;
    
public: 
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {}
    
    Iterator begin() const { 
        return begin_; 
    }
    
    Iterator end() const { 
        return end_; 
    }
};

std::ostream& operator<<(std::ostream& output, const IteratorRange<auto>& value) {
    for (auto page = value.begin(); page != value.end(); ++page) {
        output << *page;
    }
    return output;
}

template <typename Iterator>
class Paginator {
    std::vector<IteratorRange<Iterator>> pages;
public:
    Paginator(Iterator begin, Iterator end, int size) {
        while (std::distance(begin, end) >= size) {
            pages.push_back(IteratorRange(begin, next(begin, size)));
            std::advance(begin, size);
        }
        if (begin != end) {
            pages.push_back(IteratorRange(begin, end));
        }
    }
    
    auto begin() const { 
        return pages.begin(); 
    }
    
    auto end() const { 
        return pages.end(); 
    }
    
    int size() { 
        return std::distance(begin(pages), end(pages)); 
    }
}; 

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}