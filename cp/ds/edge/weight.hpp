#include <iostream>
#include <limits>
#include <compare>

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

    static constexpr value_type inf = std::numeric_limits<value_type>::max() / value_type(2);

    constexpr weight_edge(int from = 0, int to = 0, value_type weight = value_type{}) noexcept
        : from(from), to(to), weight(weight) {}

    constexpr auto operator<=>(const weight_edge& rhs) const noexcept {
        return weight <=> rhs.weight;
    }

    constexpr bool operator==(const weight_edge& rhs) const noexcept {
        return from == rhs.from && to == rhs.to && weight == rhs.weight;
    }

    friend std::istream& operator>>(std::istream& is, weight_edge& e) {
        return is >> e.from >> e.to >> e.weight;
    }

    friend std::ostream& operator<<(std::ostream& os, const weight_edge& e) {
        return os << "(" << e.from << ", " << e.to << ", " << e.weight << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_EDGE_WEIGHT