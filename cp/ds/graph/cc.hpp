#include <vector>
#include <algorithm>
#include "graph.hpp"

#ifndef CP_DS_GRAPH_CC
#define CP_DS_GRAPH_CC

namespace cp {
namespace ds {

template <typename Edge>
class cc_graph : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::adj;
    using graph<edge_type>::edge_list;
    using graph<edge_type>::num_vertices;
    using graph<edge_type>::graph;
    using graph<edge_type>::init;

    void build() override {
        if (built) return;
        graph<edge_type>::build();

        num = 0;
        comp.assign(n, -1);

        std::vector<int> q;
        q.reserve(n);

        for (int src = 0; src < n; ++src) {
            if (comp[src] != -1) continue;

            int current_id = num++;
            q.clear();
            q.push_back(src);
            comp[src] = current_id;

            int head = 0;
            while (head < static_cast<int>(q.size())) {
                int u = q[head++];
                for (const auto& e : adj(u)) {
                    int v = e.to;
                    if (comp[v] == -1) {
                        comp[v] = current_id;
                        q.push_back(v);
                    }
                }
            }
        }

        built = true;
    }

    std::pair<int, std::vector<int>> cc() {
        if (!built) build();
        return { num, comp };
    }

    std::vector<std::vector<int>> groups() {
        if (!built) build();

        std::vector<int> cnt(num, 0);
        for (int i : comp) {
            ++cnt[i];
        }

        std::vector<std::vector<int>> g(num);
        for (int i = 0; i < num; ++i) {
            g[i].reserve(cnt[i]);
        }
        for (int u = 0; u < n; ++u) {
            g[comp[u]].push_back(u);
        }

        return g;
    }

    [[nodiscard]] int num_components() {
        if (!built) build();
        return num;
    }

    [[nodiscard]] int operator[](int u) {
        assert(0 <= u && u < n);
        if (!built) build();
        return comp[u];
    }

    [[nodiscard]] bool same(int u, int v) {
        if (!built) build();
        return comp[u] == comp[v];
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::built;

    int num = 0;
    std::vector<int> comp;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_CC