#include <iostream>
#include <limits>

#ifndef CP_DS_GRAPH_EDGE_FLOW_COST
#define CP_DS_GRAPH_EDGE_FLOW_COST

namespace cp {
namespace ds {
    
template <typename Tp1, typename Tp2>
struct flow_cost_edge {
    using value_type1 = Tp1;
    using value_type2 = Tp2;

    int from;
    int to;
    value_type1 cap;
    value_type2 cost;
    value_type1 flow;
    static constexpr value_type1 inf1 = std::numeric_limits<value_type1>::max() / 2;
    static constexpr value_type2 inf2 = std::numeric_limits<value_type2>::max() / 2;

    flow_cost_edge(int from = 0, int to = 0, value_type1 cap = 0, value_type2 cost = 0, value_type1 flow = 0)
        : from(from), to(to), cap(cap), cost(cost), flow(flow) {}

    friend std::istream& operator>>(std::istream& is, flow_cost_edge& e) {
        is >> e.from >> e.to >> e.cap >> e.cost;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const flow_cost_edge& e) {
        os << "(" << e.from << ", " << e.to << ", " << e.cap << ", " << e.cost << ", " << e.flow << ")";
        return os;
    }
};
template<typename Tp1, typename Tp2>
constexpr Tp1 flow_cost_edge<Tp1, Tp2>::inf1;
template<typename Tp1, typename Tp2>
constexpr Tp2 flow_cost_edge<Tp1, Tp2>::inf2;

} // namespace ds
} // namespace cp
#endif