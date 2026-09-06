#include "hld.hpp" // Đảm bảo đã include HLD

#ifndef CP_DS_TREE_VIRTUAL_TREE
#define CP_DS_TREE_VIRTUAL_TREE
namespace cp {
namespace ds {

template <typename Edge>
class virtual_tree : public graph<Edge> {
public:
    using edge_type = Edge;
    using graph<edge_type>::built;
    using graph<edge_type>::add_edge;
    using graph<edge_type>::num_vertices;

    virtual_tree() : hld_ptr(nullptr) {}

    virtual_tree(hld<edge_type>& tree_ref) {
        init(tree_ref);
    }

    void init(hld<edge_type>& tree_ref) {
        hld_ptr = &tree_ref;
        pos_map.assign(hld_ptr->num_vertices(), -1);
        built = false;
    }

    void build() override {
        assert(false && "Virtual tree must be built with a list of nodes!");
    }

    void build(std::vector<int> nodes) {
        assert(hld_ptr != nullptr);
        
        if (nodes.empty()) {
            graph<edge_type>::init(0, 0);
            ord.clear();
            built = true;
            return;
        }

        auto cmp = [&](int u, int v) { 
            return hld_ptr->input(u) < hld_ptr->input(v);
        };
        
        std::sort(nodes.begin(), nodes.end(), cmp);

        int siz = static_cast<int>(nodes.size());
        for (int i = 0; i < siz - 1; ++i) {
            nodes.push_back(hld_ptr->lca(nodes[i], nodes[i + 1]));
        }

        std::sort(nodes.begin(), nodes.end(), cmp);
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());

        int m = static_cast<int>(nodes.size());
        
        graph<edge_type>::init(m, m - 1);

        for (int i = 0; i < m; ++i) {
            pos_map[nodes[i]] = i;
        }

        std::vector<int> stk;
        stk.reserve(m);
        stk.push_back(nodes[0]);

        for (int i = 1; i < m; ++i) {
            int u = nodes[i];
            int l = hld_ptr->lca(u, stk.back());

            while (stk.size() >= 2 && hld_ptr->depth(stk[stk.size() - 2]) >= hld_ptr->depth(l)) {
                int p = stk[stk.size() - 2];
                int c = stk.back();
                
                edge_type new_e;
                new_e.from = pos_map[p];
                new_e.to   = pos_map[c];
                add_edge(std::move(new_e));
                
                stk.pop_back();
            }

            if (stk.back() != l) {
                int c = stk.back();
                
                edge_type new_e;
                new_e.from = pos_map[l];
                new_e.to   = pos_map[c];
                add_edge(std::move(new_e));
                
                stk.pop_back();
                stk.push_back(l);
            }
            
            stk.push_back(u);
        }

        while (stk.size() >= 2) {
            int p = stk[stk.size() - 2];
            int c = stk.back();
            
            edge_type new_e;
            new_e.from = pos_map[p];
            new_e.to   = pos_map[c];
            add_edge(std::move(new_e));
            
            stk.pop_back();
        }

        graph<edge_type>::build();

        for (int u : nodes) {
            pos_map[u] = -1;
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
    hld<edge_type>*  hld_ptr;
    std::vector<int> pos_map;
    std::vector<int> ord;
};

} // namespace ds
} // namespace cp
#endif