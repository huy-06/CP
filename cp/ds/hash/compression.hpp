#include <vector>
#include <algorithm>
#include "hash_map.hpp"

#ifndef CP_DS_HASH_COMPRESS
#define CP_DS_HASH_COMPRESS
namespace cp {
namespace ds {

template <typename Tp>
class compression {
public:
    using value_type = Tp;

    compression() = default;

    compression(std::vector<value_type> a) {
        init(std::move(a));
    }

    void init(std::vector<value_type> a) {
        vals = std::move(a);
        std::sort(vals.begin(), vals.end());
        vals.erase(std::unique(vals.begin(), vals.end()), vals.end());

        for (int i = 0; i < (int) vals.size(); ++i) {
            f[vals[i]] = i;
        }
    }

    int get(const value_type& v) const {
        auto it = f.find(v);
        if (it != f.end()) {
            return it->second;
        }
        return -1;
    }

    int operator[](const value_type& v) const {
        return get(v);
    }

    value_type original(int p) const {
        return vals[p];
    }

    int size() const {
        return (int) vals.size();
    }

private:
    std::vector<value_type> vals;
    hash_map<value_type, int> f;
};

} // namespace ds
} // namespace cp
#endif