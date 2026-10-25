#include <iostream>
#include <limits>

#ifndef CP_DS_GRAPH_EDGE_MF
#define CP_DS_GRAPH_EDGE_MF

namespace cp {
namespace ds {

template <typename Cap>
struct mf_edge {
    using cap_type = Cap;

    int from;
    int to;
    cap_type cap;
    cap_type flow;
    int rev;

    static constexpr cap_type inf_cap = std::numeric_limits<cap_type>::max() / 2;

    constexpr mf_edge(int from = 0, int to = 0, cap_type cap = 0, cap_type flow = 0, int rev = -1) noexcept
        : from(from), to(to), cap(cap), flow(flow), rev(rev) {}

    [[nodiscard]] constexpr cap_type residual_cap() const noexcept {
        return cap - flow;
    }

    friend std::istream& operator>>(std::istream& is, mf_edge& e) {
        return is >> e.from >> e.to >> e.cap;
    }

    friend std::ostream& operator<<(std::ostream& os, const mf_edge& e) {
        return os << "(" << e.from << ", " << e.to << ", " << e.cap << ", " << e.flow << ")";
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_EDGE_MF