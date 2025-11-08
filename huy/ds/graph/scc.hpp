#include <functional>
#include "graph.hpp"

#ifndef HUY_DS_GRAPH_SCC
#define HUY_DS_GRAPH_SCC
namespace huy {
namespace ds {

template <typename Edge>
class scc_graph : public graph<Edge> {
public:
    using edge_type = Edge;

    scc_graph(int n = 0, int m = 0) {
        init(n, m);
    }

    void init(int n = 0, int m = 0) {
        graph<edge_type>::init(n, m);
        num = 0;
        comp.assign(n, -1);
        built = false;
    }

    std::pair<int, std::vector<int>> scc() {
        if (!built) build();
        return { num, comp };
    }

    std::vector<std::vector<int>> groups() {
        if (!built) build();

        std::vector<int> cnt(num, 0);
        for (int id : comp) {
            ++cnt[id];
        }

        std::vector<std::vector<int>> g(num);
        for (int i = 0; i < num; ++i) {
            g[i].reserve(cnt[i]);
        }
        for (int u = 0; u < n; ++u) {
            int id = comp[u];
            g[id].push_back(u);
        }

        return g;
    }

    graph<edge_type> compress(bool self_loop = false) {
        if (!built) build();

        graph<edge_type> g(num);
        std::vector<int> mark(num, -1);

        for (const auto& e : edge_list) {
            int u = comp[e.from];
            int v = comp[e.to];
            if (!self_loop && u == v) continue;
            if (mark[v] != u) {
                g.add_edge(edge_type(u, v));
                mark[v] = u;
            }
        }

        return g;
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::edge_list;

    int num;
    bool built;
    std::vector<int> comp;

    void build() {
        std::vector<int> in (n, -1);
        std::vector<int> low(n, -1);
        std::vector<int> stk;
        stk.reserve(n);
        int timer = 0;

        std::function<void(int)> dfs = [&](int u) -> void {
            in[u] = low[u] = timer++;
            stk.push_back(u);

            for (const auto& e : (*this)[u]) {
                int v = e.to;
                if (in[v] == -1) {
                    dfs(v);
                    low[u] = std::min(low[u], low[v]);
                } else if (comp[v] == -1) {
                    low[u] = std::min(low[u], in[v]);
                }
            }

            if (low[u] == in[u]) {
                while (true) {
                    int v = stk.back();
                    stk.pop_back();
                    comp[v] = num;
                    if (v == u) {
                        break;
                    }
                }
                ++num;
            }
        };

        for (int u = 0; u < n; ++u) {
            if (in[u] == -1) {
                dfs(u);
            }
        }

        for (int& id : comp) {
            id = (num - 1) - id;
        }

        built = true;
    }
};

} // namespace ds
} // namespace huy
#endif