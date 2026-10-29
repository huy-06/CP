#include "graph.hpp"

#ifndef CP_DS_GRAPH_SCC
#define CP_DS_GRAPH_SCC

namespace cp {
namespace ds {

template <typename Edge>
class scc_graph : public graph<Edge> {
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

        std::vector<int> in(n, -1);
        std::vector<int> low(n, -1);
        std::vector<int> stk;
        stk.reserve(n);
        int timer = 0;

        std::vector<std::pair<int, int>> frames;
        frames.reserve(n);

        for (int src = 0; src < n; ++src) {
            if (in[src] != -1) continue;

            frames.push_back({src, 0});
            in[src] = low[src] = timer++;
            stk.push_back(src);

            while (!frames.empty()) {
                auto& [u, i] = frames.back();
                auto nbrs = adj(u);

                if (i < static_cast<int>(nbrs.size())) {
                    int v = nbrs[i].to;
                    ++i;

                    if (in[v] == -1) {
                        in[v] = low[v] = timer++;
                        stk.push_back(v);
                        frames.push_back({v, 0});
                    } else if (comp[v] == -1) {
                        low[u] = std::min(low[u], in[v]);
                    }
                } else {
                    if (low[u] == in[u]) {
                        while (true) {
                            int v = stk.back();
                            stk.pop_back();
                            comp[v] = num;
                            if (v == u) break;
                        }
                        ++num;
                    }
                    frames.pop_back();
                    if (!frames.empty()) {
                        int p = frames.back().first;
                        low[p] = std::min(low[p], low[u]);
                    }
                }
            }
        }

        for (int& i : comp) {
            i = (num - 1) - i;
        }

        built = true;
    }

    std::pair<int, std::vector<int>> scc() {
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

    graph<edge_type> compress(bool self_loop = false) {
        if (!built) build();

        std::vector<edge_type> new_edges;
        new_edges.reserve(edge_list.size());

        for (const auto& e : edge_list) {
            int u = comp[e.from];
            int v = comp[e.to];
            if (!self_loop && u == v) continue;

            edge_type e_new = e;
            e_new.from = u;
            e_new.to   = v;
            new_edges.push_back(std::move(e_new));
        }

        std::sort(new_edges.begin(), new_edges.end());
        new_edges.erase(std::unique(new_edges.begin(), new_edges.end(),
            [](const edge_type& a, const edge_type& b) {
                return a.from == b.from && a.to == b.to;
            }),
            new_edges.end()
        );

        return graph<edge_type>(new_edges);
    }

    [[nodiscard]] int num_components() {
        if (!built) build();
        return num;
    }

    [[nodiscard]] int operator[](int u) {
        if (!built) build();
        return comp[u];
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::built;

    int num = 0;
    std::vector<int> comp;
};

} // namespace ds
} // namespace cp

#endif // CP_DS_GRAPH_SCC