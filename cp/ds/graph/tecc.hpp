#include <queue>
#include <unordered_set>
#include <functional>
#include "graph.hpp"

#ifndef CP_DS_GRAPH_TECC
#define CP_DS_GRAPH_TECC
namespace cp {
namespace ds {

template <typename Edge>
class tecc : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::num_edges;
    using graph<edge_type>::index;
    

    tecc(int n = 0, int m = 0) { 
        init(n, m);
    }
 
    void init(int n = 0, int m = 0) {
        graph<edge_type>::init(n, m);
        cid.clear();
        brs.clear();
        mrk.clear();
        cnt = 0;
        ok1 = false;
        ok2 = false;
    }
 
    const std::vector<bool>& mark() {
        if (!ok1) work1();
        return mrk;
    }
 
    const std::vector<edge_type>& bridges() {
        if (!ok1) work1();
        return brs;
    }
 
    const std::pair<int, std::vector<int>&> comp() {
        if (!ok2) work2();
        return { cnt, cid };
    }
 
    graph<edge_type> compress() {
        if (!ok2) work2();
        graph<edge_type> g(cnt);
        for (int i = 0; i < num_edges(); i += 2) {
            if (mrk[i >> 1]) {
                int u = cid[edge_list[i].from];
                int v = cid[edge_list[i].to];
                if (u != v) {
                    g.add_unedge(edge_type(u, v));
                }
            }
        }
        g.build();
        return g;
    }
 
private:
    using graph<edge_type>::n;
    using graph<edge_type>::edge_list;

    std::vector<int>  cid;
    std::vector<bool> mrk;
    std::vector<edge_type> brs;
    int  cnt = 0;
    bool ok1 = false;
    bool ok2 = false;
 
    void work1() {
        mrk.assign(num_edges() >> 1, false);
        brs.clear();
        brs.reserve(num_edges() >> 1);

        std::vector<int> in(n, -1), low(n, -1);
        int timer = 0; cnt = 0;
        
        std::function<void(int, int)> dfs = [&](int u, int pid) {
            in[u] = low[u] = timer++;
            for (const auto& e : (*this)[u]) {
                int v = e.to;
                int did = index(e);
                int uid = did >> 1;
                if (uid == pid) continue;
                if (in[v] == -1) {
                    dfs(v, uid);
                    low[u] = std::min(low[u], low[v]);
                    if (low[v] > in[u]) {
                        mrk[uid] = true;
                    }
                } else {
                    low[u] = std::min(low[u], in[v]);
                }
            }
        };

        for (int u = 0; u < n; ++u) {
            if (in[u] == -1) {
                dfs(u, -1); 
            }
        }

        auto key = [&](int u, int v) {
            return static_cast<long long>(u) * n + v;
        };
        
        std::unordered_set<long long> seen;
        seen.reserve(num_edges() >> 1);
        for (int i = 0; i < num_edges(); i += 2) {
            if (mrk[i >> 1]) {
                int u = edge_list[i].from;
                int v = edge_list[i].to;
                if (seen.insert(key(std::min(u, v), std::max(u, v))).second) {
                    brs.push_back(edge_type(u, v));
                }
            }
        }
        ok1 = true;
    }
 
    void work2() {
        if (!ok1) work1();
        cid.assign(n, -1);
        cnt = 0;
        for (int i = 0; i < n; ++i) {
            if (cid[i] != -1) continue;
            std::queue<int> q;
            q.push(i);
            cid[i] = cnt;
            while (!q.empty()) {
                int u = q.front(); 
                q.pop();
                for (const auto& e : (*this)[u]) {
                    int v = e.to;
                    int uid = index(e) >> 1;
                    if (mrk[uid]) continue;
                    if (cid[v] == -1) {
                        cid[v] = cnt;
                        q.push(v);
                    }
                }
            }
            ++cnt;
        }
        ok2 = true;
    }
};

} // namespace ds
} // namespace cp
#endif