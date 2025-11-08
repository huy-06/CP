#include <functional>
#include "graph.hpp"

#ifndef HUY_DS_GRAPH_TVCC
#define HUY_DS_GRAPH_TVCC
namespace huy {
namespace ds {

template <typename Edge>
class tvcc : public graph<Edge> {
public:
    using edge_type = Edge;

    tvcc(int n = 0, int m = 0) {
        init(n, m);
    }

    void init(int n = 0, int m = 0) {
        graph<edge_type>::init(n, m);
        ok = false;
    }

    std::vector<bool>& mark() {
        if (!ok) work();
        return mrk;
    }

    std::vector<int>& cuts() {
        if (!ok) work();
        return cts;
    }

private:
    using graph<Edge>::n;

    std::vector<bool> mrk;
    std::vector<int>  cts;
    bool ok;

    void work() {
        mrk.assign(n, false);
        cts.clear();
        cts.reserve(n);
        std::vector<int> in(n, -1), low(n, -1);
        int timer = 0;
        std::function<void(int, int)> dfs = [&](int u, int p) -> void {
            in[u] = low[u] = timer++;
            int c = 0;
            for (const auto& e : (*this)[u]) {
                int v = e.to;
                if (v == p) continue;
                if (in[v] == -1) {
                    dfs(v, u);
                    low[u] = std::min(low[u], low[v]);
                    if (low[v] >= in[u] && p != -1) {
                        mrk[u] = true;
                    }
                    ++c;
                } else {
                    low[u] = std::min(low[u], in[v]);
                }
            }
            if (p == -1 && c > 1) {
                mrk[u] = true;
            }
        };
        for (int u = 0; u < n; ++u) {
            if (in[u] == -1) {
                dfs(u, -1);
            }
        }
        for (int u = 0; u < n; ++u) {
            if (mrk[u]) {
                cts.push_back(u);
            }
        }
        ok = true;
    }
};

} // namespace ds
} // namespace huy
#endif