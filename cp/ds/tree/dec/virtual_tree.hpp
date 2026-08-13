#include "hld.hpp"

#ifndef CP_DS_TREE_VIRTUAL
#define CP_DS_TREE_VIRTUAL
namespace cp {
namespace ds {

template <typename Edge>
class virtual_tree : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::built;
    using graph<edge_type>::add_edge;
    using graph<edge_type>::num_vertices;

    virtual_tree() : hld(nullptr) {}

    virtual_tree(hld<edge_type>& hld_ref) {
        init(hld_ref);
    }

    void init(hld<edge_type>& hld_ref) {
        this->hld = &hld_ref;
        map.assign(hld_ref.num_vertices(), -1);
        built = false;
    }

    void build(std::vector<int> nodes) {
        assert(hld != nullptr);
        if (nodes.empty()) {
            graph<edge_type>::init(0, 0);
            ord.clear();
            built = true;
            return;
        }

        auto cmp = [this](int u, int v) { 
            return hld->input(u) < hld->input(v);
        };
        std::sort(nodes.begin(), nodes.end(), cmp);

        int siz = static_cast<int>(nodes.size());
        for (int i = 0; i < siz - 1; ++i) {
            nodes.push_back(hld->lca(nodes[i], nodes[i + 1]));
        }

        std::sort(nodes.begin(), nodes.end(), cmp);
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());

        int m = static_cast<int>(nodes.size());
        graph<edge_type>::init(m, m - 1);

        for (int i = 0; i < m; ++i) {
            map[nodes[i]] = i;
        }

        std::vector<int> stk = {nodes[0]};
        for (int i = 1; i < m; ++i) {
            int u = nodes[i], l = hld->lca(u, stk.back());

            while (stk.size() >= 2 && hld->depth(stk[stk.size() - 2]) >= hld->depth(l)) {
                add_edge(edge_type{map[stk[stk.size() - 2]], map[stk.back()]});
                stk.pop_back();
            }
            if (stk.back() != l) {
                add_edge(edge_type{map[l], map[stk.back()]});
                stk.pop_back();
                stk.push_back(l);
            }
            stk.push_back(u);
        }
        while (stk.size() >= 2) {
            add_edge(edge_type{map[stk[stk.size() - 2]], map[stk.back()]});
            stk.pop_back();
        }

        graph<edge_type>::build();

        for (int u : nodes) {
            map[u] = -1;
        }
        ord = std::move(nodes);
        built = true;
    }

    // Lấy lại đỉnh gốc từ ID nén
    int original(int u) const {
        assert(0 <= u && u < num_vertices());
        return ord[u];
    }

    // Danh sách các đỉnh gốc trên cây ảo
    const std::vector<int>& tour() const {
        return ord;
    }

private:
    hld<edge_type>* hld;
    std::vector<int> map;
    std::vector<int> ord;
};

} // namespace ds
} // namespace cp
#endif