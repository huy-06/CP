#include <functional>
#include "scc.hpp"

#ifndef HUY_DS_GRAPH_TWO_SAT
#define HUY_DS_GRAPH_TWO_SAT
namespace huy {
namespace ds {

template <typename Edge>
class two_sat {
public:
    using edge_type = Edge;

    two_sat(int n = 0) {
        init(n);
    }

    void init(int n = 0) {
        this->n = n;
        scc.init(n << 1);
        assignment.assign(n, false);
    }

    void add_clause(int u, bool f, int v, bool g) {
        assert(0 <= u && u < n);
        assert(0 <= v && v < n);
        scc.add_edge(edge_type(u << 1 | f, v << 1 | !g));
        scc.add_edge(edge_type(v << 1 | g, u << 1 | !f));
    }

    bool satisfiable() {
        auto comp = scc.scc().second;
        for (int i = 0; i < n; ++i) {
            if (comp[i << 1] == comp[i << 1 | 1]) {
                return false;
            }
            assignment[i] = comp[i << 1] > comp[i << 1 | 1];
        }
        return true;
    }

    bool operator[](int i) const {
        return assignment[i];
    }

private:
    int n;
    ds::scc_graph<edge_type> scc;
    std::vector<bool> assignment;
};

} // namespace ds
} // namespace huy
#endif