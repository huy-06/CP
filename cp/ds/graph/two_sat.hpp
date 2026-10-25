#include "scc.hpp"

#ifndef CP_DS_GRAPH_TWO_SAT
#define CP_DS_GRAPH_TWO_SAT

namespace cp {
namespace ds {

template <typename Edge = edge>
class two_sat {
public:
    using edge_type = Edge;

    explicit two_sat(int n = 0) {
        init(n);
    }

    void init(int n = 0) {
        this->n = n;
        scc.init(2 * n);
        assignment.assign(n, false);
    }

    /// @brief Thêm mệnh đề: (u == f) OR (v == g)
    void add_clause(int u, bool f, int v, bool g) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        scc.add_edge(edge_type(2 * u + (f ? 0 : 1), 2 * v + (g ? 1 : 0)));
        scc.add_edge(edge_type(2 * v + (g ? 0 : 1), 2 * u + (f ? 1 : 0)));
    }

    /// @brief Ép biến u phải nhận giá trị val
    void set_value(int u, bool val) {
        add_clause(u, val, u, val);
    }

    [[nodiscard]] bool satisfiable() {
        auto [num_comp, comp] = scc.scc();
        for (int i = 0; i < n; ++i) {
            if (comp[2 * i] == comp[2 * i + 1]) {
                return false;
            }
            assignment[i] = (comp[2 * i] < comp[2 * i + 1]);
        }
        return true;
    }

    [[nodiscard]] const std::vector<bool>& answer() const {
        return assignment;
    }

    [[nodiscard]] bool operator[](int i) const {
        assert(0 <= i && i < n);
        return assignment[i];
    }

    [[nodiscard]] int size() const noexcept {
        return n;
    }

private:
    int n = 0;
    scc_graph<edge_type> scc;
    std::vector<bool> assignment;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_TWO_SAT