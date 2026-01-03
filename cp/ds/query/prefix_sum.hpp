#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_PREFIX_SUM
#define CP_DS_QUERY_PREFIX_SUM
namespace cp {
namespace ds {

template <typename Tp>
class prefix_sum {
public:
    using value_type = Tp;

    prefix_sum() {}

    template <typename U>
    prefix_sum(const std::vector<U>& data) {
        init(data);
    }

    template <typename U>
    void init(const std::vector<U>& data) {
        n = static_cast<int>(data.size());

        pref.assign(n, value_type());
        pref[0] = value_type(data[0]);
        
        for (int i = 1; i < n; ++i) {
            pref[i] = pref[i - 1] + value_type(data[i]);
        }
    }

    value_type point_sum(int p) const {
        assert(0 <= p && p < n);

        return sum(p + 1) - sum(p);
    }

    // range [l, r)
    value_type range_sum(int l, int r) const {
        assert(0 <= l && l <= r && r <= n);

        return sum(r) - sum(l);
    }

    /// @brief Tìm vị trí p nhỏ nhất sao cho prefix_sum(p) >= k
    int min_left(value_type k) const {
        if (n == 0 || sum(n) < k) {
            return n + 1;
        }

        int p = 1;
        while ((p << 1) <= n) p <<= 1;

        int pos = 0;
        for (int i = p; i > 0; i >>= 1) {
            if (pos + i <= n && sum(pos + i) < k) {
                pos += i;
            }
        }
        return pos + 1;
    }

    /// @brief  Tìm vị trí p lớn nhất sao cho prefix_sum(p) <= k
    int max_right(value_type k) const {
        if (n == 0) return 0;
        
        int p = 1;
        while ((p << 1) <= n) p <<= 1;

        int pos = 0;
        for (int i = p; i > 0; i >>= 1) {
            if (pos + i <= n && sum(pos + i) <= k) {
                pos += i;
            }
        }
        return pos;
    }

private:
    int n;
    std::vector<value_type> pref;

    value_type sum(int p) const {
        if (p == 0) return value_type();
        return pref[p - 1];
    }
};

} // namespace ds
} // namespace cp
#endif