#include <iostream>

#ifndef HUY_DS_GRAPH_EDGE_BASE
#define HUY_DS_GRAPH_EDGE_BASE
namespace huy {
namespace ds {

struct edge {
    int from;
    int to;
 
    edge(int from = 0, int to = 0) : from(from), to(to) {}

    bool operator<(const edge& e) const {
        if (from != e.from) return from < e.from;
        return to < e.to;        
    }
 
    friend std::istream& operator>>(std::istream& is, edge& e) {
        is >> e.from >> e.to;
        return is;
    }
 
    friend std::ostream& operator<<(std::ostream& os, const edge& e) {
        os << "(" << e.from << ", " << e.to << ")";
        return os;
    }
};

} // namespace ds
} // namespace huy
#endif