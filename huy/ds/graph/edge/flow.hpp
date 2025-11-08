#include <iostream>
#include <limits>

#ifndef HUY_DS_GRAPH_EDGE_FLOW
#define HUY_DS_GRAPH_EDGE_FLOW

namespace huy {
namespace ds {
    
template <typename Tp>
struct flow_edge {
    using value_type = Tp;

    int from;
    int to;
    value_type cap;
    value_type flow;
    static constexpr value_type inf = std::numeric_limits<value_type>::max() / 2;

    flow_edge(int from = 0, int to = 0, value_type cap = 0, value_type flow = 0)
        : from(from), to(to), cap(cap), flow(flow) {}

    friend std::istream& operator>>(std::istream& is, flow_edge& e) {
        is >> e.from >> e.to >> e.cap;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const flow_edge& e) {
        os << "(" << e.from << ", " << e.to << ", " << e.cap << ", " << e.flow << ")";
        return os;
    }
};
template<typename value_type>
constexpr value_type flow_edge<value_type>::inf;

} // namespace ds
} // namespace huy
#endif