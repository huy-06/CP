#include <vector>
#include <cassert>

#ifndef CP_DS_QUERY_PREFIX_SUM_2D
#define CP_DS_QUERY_PREFIX_SUM_2D
namespace cp {
namespace ds {

template <typename Tp>
class prefix_sum_2d {
public:
    using value_type = Tp;

    prefix_sum_2d() : n(0), m(0) {}

    template <typename T>
    prefix_sum_2d(const std::vector<std::vector<T>>& data) {
        init(data);
    }

    template <typename T>
    void init(const std::vector<std::vector<T>>& data) {
        n = static_cast<int>(data.size());
        if (n == 0) {
            m = 0;
            return;
        }
        m = static_cast<int>(data[0].size());

        pref.assign((n + 1) * (m + 1), value_type());

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                pref[(i + 1) * m + j + 1] = value_type(data[i][j]) + pref[i * m + j + 1] + pref[(i + 1) * m + j] - pref[i * m + j];
            }
        }
    }

    value_type point_sum(int x, int y) const {
        assert(0 <= x && x < n);
        assert(0 <= y && y < m);
        
        return rect_sum(x, y, x + 1, y + 1);
    }

    value_type rect_sum(int xs, int ys, int xe, int ye) const {
        assert(0 <= xs && xs <= xe && xe <= n);
        assert(0 <= ys && ys <= ye && ye <= m);
        if (xs >= xe || ys >= ye) {
            return value_type();
        }

        return pref[xe* m + ye] - pref[xs * m + ye] - pref[xe * m + ys] + pref[xs * m + ys];
    }

private:
    int n, m;
    std::vector<value_type> pref;
};

} // namespace ds
} // namespace cp
#endif