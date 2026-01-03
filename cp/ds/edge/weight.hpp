#include <iostream>
#include <limits>

#ifndef CP_DS_GRAPH_EDGE_WEIGHT
#define CP_DS_GRAPH_EDGE_WEIGHT

namespace cp {
namespace ds {

template <typename Tp>
struct weight_edge {
    using value_type = Tp;

    int from;
    int to;
    value_type weight;
    
#if __cplusplus >= 201703L
    inline static constexpr value_type inf = std::numeric_limits<value_type>::max() / value_type(2);
#else
    static const value_type inf;
#endif

    weight_edge(int from = 0, int to = 0, value_type weight = value_type()) 
        : from(from), to(to), weight(weight) {}

    bool operator<(const weight_edge<value_type>& rhs) const {
        return weight < rhs.weight;
    }

    bool operator>(const weight_edge<value_type>& rhs) const {
        return weight > rhs.weight;
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

#if __cplusplus < 201703L
template<typename Tp>
const typename weight_edge<Tp>::value_type weight_edge<Tp>::inf = std::numeric_limits<Tp>::max() / value_type(2);
#endif

} // namespace ds
} // namespace cp
#endif