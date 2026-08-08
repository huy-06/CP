#include <vector>
#include <cassert>
#include <algorithm>

#ifndef CP_DS_TREE_DSU_ROLLBACK
#define CP_DS_TREE_DSU_ROLLBACK
namespace cp {
namespace ds {

class dsu_rollback {
public:
    dsu_rollback(int n = 0) : n(n), comp(n), f(n, -1) {}

    bool merge(int a, int b) {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        a = leader(a);
        b = leader(b);
        if (a == b) return false;
        
        if (-f[a] < -f[b]) std::swap(a, b);
        
        // Lưu lại trạng thái trước khi thay đổi
        // a là root mới (sẽ bị thay đổi size)
        // b là root cũ (sẽ bị gán parent là a)
        history.push_back({a, f[a], b, f[b]});
        
        f[a] += f[b];
        f[b] = a;
        --comp;
        return true;
    }

    bool same(int a, int b) const {
        assert(0 <= a && a < n);
        assert(0 <= b && b < n);
        return leader(a) == leader(b);
    }

    int find(int a) const {
        assert(0 <= a && a < n);
        return leader(a);
    }

    int operator[](int a) const {
        return find(a);
    }

    int size(int a) const {
        assert(0 <= a && a < n);
        return -f[leader(a)];
    }

    // Trả về số lượng thành phần liên thông hiện tại
    int comps() const {
        return comp;
    }

    // Lấy thời điểm hiện tại (số lượng thao tác đã thực hiện)
    int time() const {
        return static_cast<int>(history.size());
    }

    // Hoàn tác 1 bước merge gần nhất
    void rollback() {
        if (history.empty()) return;
        state s = history.back();
        history.pop_back();
        
        f[s.u] = s.fu;
        f[s.v] = s.fv;
        ++comp;
    }

    // Hoàn tác về thời điểm t
    void rollback(int t) {
        assert(0 <= t && t <= time());
        while (time() > t) {
            rollback();
        }
    }

    // Hàm snapshot (tùy chọn): Giống việc lấy time() để dùng sau này
    int snapshot() const {
        return time();
    }

    std::vector<std::vector<int>> groups() const {
        std::vector<std::vector<int>> res(n);
        for (int i = 0; i < n; ++i) {
            res[leader(i)].push_back(i);
        }
        res.erase(
            std::remove_if(res.begin(), res.end(), [](const std::vector<int>& v) { 
                return v.empty();
            }), 
            res.end()
        );
        return res;
    }

private:
    int n;
    int comp;
    std::vector<int> f;

    struct state {
        int u, fu; // u là node cha sau khi gộp, fu là giá trị f[u] cũ
        int v, fv; // v là node con sau khi gộp, fv là giá trị f[v] cũ
    };
    std::vector<state> history;

    int leader(int a) const {
        while (f[a] >= 0) {
            a = f[a];
        }
        return a;
    }
};

} // namespace ds
} // namespace cp
#endif