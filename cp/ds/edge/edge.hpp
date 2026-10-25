#include <iostream>
#include <compare>

#ifndef CP_DS_GRAPH_EDGE_BASE
#define CP_DS_GRAPH_EDGE_BASE

namespace cp {
namespace ds {

struct edge {
    int from;
    int to;

    constexpr edge(int from = 0, int to = 0) noexcept : from(from), to(to) {}

    constexpr auto operator<=>(const edge& other) const noexcept = default;

    friend std::istream& operator>>(std::istream& is, edge& e) {
        return is >> e.from >> e.to;
    }

    friend std::ostream& operator<<(std::ostream& os, const edge& e) {
        return os << "(" << e.from << ", " << e.to << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_EDGE_BASE