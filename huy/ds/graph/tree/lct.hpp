#include <vector>
#include <array>

#ifndef HUY_DS_LINK_CUT_TREE
#define HUY_DS_LINK_CUT_TREE

namespace huy {
namespace ds {

template <typename Tp>
class link_cut_tree {
public:
    using value_type = Tp;

    std::vector<std::array<int, 2>> chi;
    std::vector<int> par;
    std::vector<int> rev;

    std::vector<int> siz;
    std::vector<value_type> val;
    std::vector<value_type> sum;
    std::vector<value_type> add;

    link_cut_tree(int n = 0) {
        init(n);
    }

    void init(int n) {
        this->n = n;
        chi.assign(n, {-1, -1});
        par.assign(n, -1);
        rev.assign(n, 0);
        val.assign(n, 0);
        sum.assign(n, 0);
        add.assign(n, 0);
        siz.assign(n, 1);
    }

    // Kiểm tra xem x có phải là root của auxiliary tree không
    inline bool is_root(int x) {
        int p = par[x];
        if (p == -1) return true;
        return !(chi[p][0] == x || chi[p][1] == x);
    }

    // Cập nhật thông tin nút x từ con
    inline void pull(int x) {
        siz[x] = 1;
        sum[x] = val[x];
        for (int d = 0; d < 2; ++d) {
            int c = chi[x][d];
            if (c != -1) {
                siz[x] += siz[c];
                sum[x] += sum[c];
            }
        }
    }

    // Áp lazy add lên nút x
    inline void apply_add(int x, int v) {
        if (x == -1) return;
        add[x] += v;
        val[x] += v;
        sum[x] += v * siz[x];
    }

    // Áp reverse lên nút x (đánh dấu đảo)
    inline void apply_rev(int x) {
        if (x == -1) return;
        rev[x] ^= 1;
        std::swap(chi[x][0], chi[x][1]);
    }

    // Đẩy lazy từ x xuống con
    inline void push(int x) {
        if (x == -1) return;
        if (rev[x]) {
            if (chi[x][0] != -1) apply_rev(chi[x][0]);
            if (chi[x][1] != -1) apply_rev(chi[x][1]);
            rev[x] = 0;
        }
        if (add[x] != 0) {
            int v = add[x];
            if (chi[x][0] != -1) apply_add(chi[x][0], v);
            if (chi[x][1] != -1) apply_add(chi[x][1], v);
            add[x] = 0;
        }
    }

    /*
     Xoay tại x

         z
         |
         y
        / \
        a   x   <-- x đang là con phải (dx = 1)
       / \
      b   c <-- b là con trái của x (chi[x][dx^1] hay chi[x][0])

         z
         |
         x   <-- x lên làm cha y
        / \
       y   c
      / \
     a   b   <-- b bị đẩy sang làm con phải của y
    */

    void rotate(int x) {
        int y = par[x];
        int z = par[y];
        int dx = (chi[y][1] == x); // là node bên phải
        int b = chi[x][dx ^ 1];

        if (!is_root(y)) {
            if (chi[z][0] == y) chi[z][0] = x;
            else if (chi[z][1] == y) chi[z][1] = x;
        }
        par[x] = z;

        if (b != -1) par[b] = y;
        chi[y][dx] = b;

        chi[x][dx ^ 1] = y;
        par[y] = x;

        pull(y);
        pull(x);
    }

    // Đẩy các cờ từ gốc của aux-tree tới x
    void push_path(int x) {
        static std::vector<int> stk;
        stk.clear();
        int u = x;
        stk.push_back(u);
        while (!is_root(u)) {
            u = par[u];
            stk.push_back(u);
        }
        for (int i = int(stk.size()) - 1; i >= 0; --i) push(stk[i]);
    }

    // Splay x lên đỉnh auxiliary tree
    void splay(int x) {
        push_path(x);
        while (!is_root(x)) {
            int y = par[x];
            int z = par[y];
            if (!is_root(y)) {
                if ((chi[y][0] == x) ^ (chi[z][0] == y)) rotate(x);
                else rotate(y);
            }
            rotate(x);
        }
    }

    // Access: đưa x lên đỉnh đường preferred; trả về last
    int access(int x) {
        int last = -1;
        for (int v = x; v != -1; v = par[v]) {
            splay(v);
            chi[v][1] = last;
            pull(v);
            last = v;
        }
        return last;
    }

    // Đặt x làm root của tree chứa nó
    void make_root(int x) {
        access(x);
        splay(x);
        apply_rev(x);
    }

    // Tìm root thực sự của tree chứa x
    int find_root(int x) {
        access(x);
        splay(x);
        while (true) {
            push(x);
            if (chi[x][0] == -1) break;
            x = chi[x][0];
        }
        splay(x);
        return x;
    }

    // Kiểm tra 2 đỉnh cùng cây không
    bool connected(int x, int y) {
        if (x == y) return true;
        return find_root(x) == find_root(y);
    }

    // Nối u - v (chỉ gọi khi chưa kết nối)
    bool link(int u, int v) {
        make_root(u);
        if (find_root(v) == u) return false; // đã cùng cây
        par[u] = v;
        return true;
    }

    // Cắt u - v (nếu có cạnh)
    bool cut(int u, int v) {
        make_root(u);
        access(v);
        splay(v);
        if (chi[v][0] == u && chi[u][1] == -1) {
            chi[v][0] = -1;
            par[u] = -1;
            pull(v);
            return true;
        }
        make_root(v);
        access(u);
        splay(u);
        if (chi[u][0] == v && chi[v][1] == -1) {
            chi[u][0] = -1;
            par[v] = -1;
            pull(u);
            return true;
        }
        return false;
    }

    // LCA động (nếu khác cây có thể trả về -1)
    int lca(int u, int v) {
        if (!connected(u, v)) return -1;
        access(u);
        return access(v);
    }

    // Đặt giá trị nguyên tại 1 node
    void set_value(int x, const value_type& v) {
        access(x);
        splay(x);
        val[x] = v;
        pull(x);
    }

    // Cộng delta cho toàn bộ node trên đường u..v
    void add_path(int u, int v, const value_type& delta) {
        make_root(u);
        access(v);
        splay(v);
        apply_add(v, delta);
    }

    // Tổng giá trị trên đường u..v
    value_type query_path_sum(int u, int v) {
        make_root(u);
        access(v);
        splay(v);
        return sum[v];
    }

private:
    int n;
};

} // namespace ds
} // namespace huy

#endif