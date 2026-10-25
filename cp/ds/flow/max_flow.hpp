#include "../edge/mf_edge.hpp"
#include "../graph/graph.hpp"

#ifndef CP_DS_FLOW_MAX_FLOW
#define CP_DS_FLOW_MAX_FLOW

namespace cp {
namespace ds {

template <typename Edge>
class max_flow : public graph<Edge> {
public:
    using edge_type       = Edge;
    using cap_type        = typename edge_type::cap_type;
    using neighbors       = typename graph<edge_type>::neighbors;
    using const_neighbors = typename graph<edge_type>::const_neighbors;

    using graph<edge_type>::init;
    using graph<edge_type>::num_edges;
    using graph<edge_type>::num_vertices;
    using graph<edge_type>::adj;

    explicit max_flow(int n = 0, int m = 0) {
        init(n, m);
    }

    void build() override {
        if (built) return;

        head.assign(n + 1, 0);
        indeg_cnt.assign(n, 0);

        for (const auto& e : edge_list) {
            ++head[e.from + 1];
            ++indeg_cnt[e.to];
        }
        for (int i = 1; i <= n; ++i) {
            head[i] += head[i - 1];
        }

        std::vector<int> cur = head;
        std::vector<int> pos(edge_list.size());
        std::vector<edge_type> sorted(edge_list.size());

        for (size_t i = 0; i < edge_list.size(); ++i) {
            pos[i] = cur[edge_list[i].from]++;
            sorted[pos[i]] = edge_list[i];
        }

        is_fwd.assign(edge_list.size(), false);
        rev_idx.assign(edge_list.size(), 0);

        fwd_ids.clear();
        fwd_ids.reserve(edge_list.size() / 2);

        for (size_t i = 0; i < edge_list.size(); i += 2) {
            int id_fwd = pos[i];
            int id_rev = pos[i + 1];

            rev_idx[id_fwd] = id_rev;
            rev_idx[id_rev] = id_fwd;

            sorted[id_fwd].rev = id_rev;
            sorted[id_rev].rev = id_fwd;

            is_fwd[id_fwd] = true;
            fwd_ids.push_back(id_fwd);
        }

        edge_list = std::move(sorted);
        built = true;
    }

    void add_edge(const edge_type& e) override {
        graph<edge_type>::add_edge(e);
        edge_type rev_e = e;
        std::swap(rev_e.from, rev_e.to);
        rev_e.cap = 0;
        rev_e.flow = 0;
        graph<edge_type>::add_edge(rev_e);
    }

    void add_edge(int from, int to, cap_type cap) {
        add_edge(edge_type(from, to, cap, 0));
    }

    void add_unedge(const edge_type& e) override {
        edge_type rev_e = e;
        std::swap(rev_e.from, rev_e.to);
        graph<edge_type>::add_edge(e);
        graph<edge_type>::add_edge(rev_e);
    }

    bool change_edge(const edge_type& e) {
        build();
        int u = e.from;
        for (int i = head[u]; i < head[u + 1]; ++i) {
            if (!is_fwd[i]) continue;
            edge_type& ce = edge_list[i];
            if (ce.to == e.to) {
                ce.cap = e.cap;
                return true;
            }
        }
        return false;
    }

    [[nodiscard]] std::vector<edge_type> get_edges() const override {
        if (!built) const_cast<max_flow*>(this)->build();

        std::vector<edge_type> res;
        res.reserve(fwd_ids.size());
        for (int id : fwd_ids) {
            res.push_back(edge_list[id]);
        }
        return res;
    }

    cap_type flow(int s, int t, cap_type limit = edge_type::inf_cap) {
        assert(0 <= s && s < num_vertices());
        assert(0 <= t && t < num_vertices());

        build();

        ptr.assign(n, 0);
        lvl.assign(n, -1);
        que.reserve(n);

        cap_type ans = 0;
        while (ans < limit && bfs(s, t)) {
            std::copy(head.begin(), head.begin() + n, ptr.begin());
            while (ans < limit) {
                cap_type pushed = dfs(s, t, limit - ans);
                if (pushed == 0) break;
                ans += pushed;
            }
        }
        return ans;
    }

    [[nodiscard]] std::vector<bool> min_cut(int s) {
        build();
        std::vector<bool> vis(n, false);
        std::vector<int> q;
        q.reserve(n);

        vis[s] = true;
        q.push_back(s);

        for (size_t i = 0; i < q.size(); ++i) {
            int u = q[i];
            for (const auto& e : adj(u)) {
                int v = e.to;
                if (!vis[v] && e.residual_cap() > 0) {
                    vis[v] = true;
                    q.push_back(v);
                }
            }
        }
        return vis;
    }

private:
    using graph<edge_type>::n;
    using graph<edge_type>::head;
    using graph<edge_type>::built;
    using graph<edge_type>::indeg_cnt;
    using graph<edge_type>::edge_list;

    std::vector<int> ptr;
    std::vector<int> lvl;
    std::vector<int> que;
    std::vector<bool> is_fwd;
    std::vector<int> fwd_ids;
    std::vector<int> rev_idx;

    bool bfs(int s, int t) {
        std::fill(lvl.begin(), lvl.end(), -1);
        que.clear();

        lvl[s] = 0;
        que.push_back(s);

        for (size_t i = 0; i < que.size(); ++i) {
            int u = que[i];
            for (int e_idx = head[u]; e_idx < head[u + 1]; ++e_idx) {
                const auto& e = edge_list[e_idx];
                int v = e.to;
                if (lvl[v] == -1 && e.residual_cap() > 0) {
                    lvl[v] = lvl[u] + 1;
                    if (v == t) return true;
                    que.push_back(v);
                }
            }
        }
        return lvl[t] != -1;
    }

    cap_type dfs(int u, int t, cap_type pushed) {
        if (pushed == 0 || u == t) return pushed;

        for (int& i = ptr[u]; i < head[u + 1]; ++i) {
            edge_type& e = edge_list[i];
            int v = e.to;

            if (lvl[v] != lvl[u] + 1) continue;

            cap_type rem = e.residual_cap();
            if (rem <= 0) continue;

            cap_type tr = dfs(v, t, std::min(pushed, rem));
            if (tr > 0) {
                e.flow += tr;
                edge_list[rev_idx[i]].flow -= tr;
                return tr;
            }
        }
        return 0;
    }
};

} // namespace ds
} // namespace cp

#endif // CP_DS_FLOW_MAX_FLOW