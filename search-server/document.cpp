#include "document.h"

using namespace std;

Document::Document(int id, double relevance, int rating): id(id), relevance(relevance), rating(rating) {}

ostream& operator<<(ostream& output, const Document& value) {
    output << "{ document_id = "s << value.id << ", relevance = "s << value.relevance << ", rating = "s << value.rating << " }"s;
    return output;
}