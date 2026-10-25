#include <iostream>
#include <limits>

#ifndef CP_DS_GRAPH_EDGE_MCF
#define CP_DS_GRAPH_EDGE_MCF

namespace cp {
namespace ds {

template <typename Cap, typename Cost>
struct mcf_edge {
    using cap_type  = Cap;
    using cost_type = Cost;

    int from;
    int to;
    cap_type cap;
    cost_type cost;
    cap_type flow;
    int rev;

    static constexpr cap_type  inf_cap  = std::numeric_limits<cap_type>::max() / 2;
    static constexpr cost_type inf_cost = std::numeric_limits<cost_type>::max() / 2;

    constexpr mcf_edge(int from = 0, int to = 0, cap_type cap = 0, cost_type cost = 0, cap_type flow = 0, int rev = -1) noexcept
        : from(from), to(to), cap(cap), cost(cost), flow(flow), rev(rev) {}

    [[nodiscard]] constexpr cap_type residual_cap() const noexcept {
        return cap - flow;
    }

    friend std::istream& operator>>(std::istream& is, mcf_edge& e) {
        return is >> e.from >> e.to >> e.cap >> e.cost;
    }

    friend std::ostream& operator<<(std::ostream& os, const mcf_edge& e) {
        return os << "(" << e.from << ", " << e.to << ", " << e.cap << ", " << e.cost << ", " << e.flow << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_EDGE_MCF