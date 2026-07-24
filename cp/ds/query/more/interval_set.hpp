#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>

#ifndef CP_DS_INTERVAL_SET
#define CP_DS_INTERVAL_SET

namespace cp {
namespace ds {

/**
 * @brief Interval Set (Old Driver Tree / Color Set)
 * Quản lý các đoạn giá trị trên trục số lớn.
 * Hỗ trợ gán giá trị cho đoạn [l, r) và tính tổng.
 * 
 * @tparam Tp Kiểu dữ liệu của giá trị (long long, int, etc.)
 * Yêu cầu: Tp phải hỗ trợ operator+, -, * (với long long độ dài).
 */
template <typename Tp>
class interval_set {
public:
    using value_type = Tp;

    interval_set() : n(0), current_sum(Tp()) {}

    interval_set(long long n, const value_type& val = value_type()) {
        init(n, val);
    }

    void init(long long n, const value_type& val = value_type()) {
        this->n = n;
        st.clear();
        st.insert({0, n, val});
        current_sum = val * n; 
    }

    // Gán giá trị val cho đoạn [l, r)
    void assign(long long l, long long r, const value_type& val) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return;

        auto itr = split(r);
        auto itl = split(l);

        auto it = itl;
        while (it != itr) {
            current_sum = current_sum - (it->v * (it->r - it->l));
            it = st.erase(it);
        }

        st.insert({l, r, val});
        current_sum = current_sum + (val * (r - l));
    }

    // Lấy giá trị tại điểm p
    value_type point_query(long long p) {
        assert(0 <= p && p < n);
        auto it = st.upper_bound({p, 0, Tp()});
        --it;
        return it->v;
    }

    // Lấy tổng giá trị trong đoạn [l, r)
    // Lưu ý: Độ phức tạp phụ thuộc số lượng đoạn bị chia cắt trong range
    value_type range_query(long long l, long long r) {
        assert(0 <= l && l <= r && r <= n);
        if (l == r) return value_type();

        auto itr = split(r);
        auto itl = split(l);

        value_type res = value_type();
        for (auto it = itl; it != itr; ++it) {
            res = res + (it->v * (it->r - it->l));
        }
        return res;
    }

    // Lấy tổng toàn bộ dải [0, n) - O(1)
    value_type all_query() const {
        return current_sum;
    }
    
    std::vector<std::tuple<long long, long long, Tp>> get_intervals() {
        std::vector<std::tuple<long long, long long, Tp>> res;
        for (auto& x : st) res.emplace_back(x.l, x.r, x.v);
        return res;
    }

private:
    struct node {
        long long l, r;
        mutable Tp v;

        bool operator<(const node& other) const {
            return l < other.l;
        }
    };

    long long n;
    Tp current_sum;
    std::set<node> st;

    // Tách đoạn chứa x thành [l, x) và [x, r). Trả về iterator bắt đầu tại x.
    auto split(long long x) {
        auto it = st.lower_bound({x, 0, Tp()});
        if (it != st.end() && it->l == x) return it;

        --it;
        long long l = it->l, r = it->r;
        Tp v = it->v;

        st.erase(it);
        st.insert({l, x, v});
        return st.insert({x, r, v}).first;
    }
};

} // namespace ds
} // namespace cp
#endif