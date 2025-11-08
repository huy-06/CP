#include <iostream>
#include <limits>

#ifndef HUY_DS_GRAPH_EDGE_WEIGHT
#define HUY_DS_GRAPH_EDGE_WEIGHT

namespace huy {
namespace ds {

template <typename Tp>
struct weight_edge {
    using value_type = Tp;

    int from;
    int to;
    value_type weight;
    static constexpr value_type inf = std::numeric_limits<value_type>::max() / 2;

    weight_edge(int from = 0, int to = 0, value_type weight = 0) 
        : from(from), to(to), weight(weight) {}

    bool operator<(const weight_edge<value_type>& other) const {
        return weight < other.weight;
    }

    bool operator>(const weight_edge<value_type>& other) const {
        return weight > other.weight;
    }

    friend std::istream& operator>>(std::istream& is, weight_edge& e) {
        is >> e.from >> e.to >> e.weight;
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const weight_edge& e) {
        os << "(" << e.from << ", " << e.to << ", " << e.weight << ")";
        return os;
    }
};
template<typename value_type>
constexpr value_type weight_edge<value_type>::inf;

} // namespace ds
} // namespace huy
#endif