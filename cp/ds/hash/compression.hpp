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
        built = false;
    }

    void build() const {
        if (built) return;

        std::sort(vals.begin(), vals.end());
        vals.erase(std::unique(vals.begin(), vals.end()), vals.end());

        f.clear();
        for (int i = 0; i < (int) vals.size(); ++i) {
            f[vals[i]] = i;
        }
        built = true;
    }

    void add(const value_type& v) {
        vals.push_back(v);
        built = false;
    }

    void add(value_type&& v) {
        vals.push_back(std::move(v));
        built = false;
    }

    int get(const value_type& v) const {
        build();
        auto it = f.find(v);
        if (it != f.end()) {
            return it->second;
        }
        return -1;
    }

    int operator[](const value_type& v) const {
        return get(v);
    }

    int lower_bound(const value_type& v) const {
        build();
        return (int) std::distance(vals.begin(), std::lower_bound(vals.begin(), vals.end(), v));
    }

    int upper_bound(const value_type& v) const {
        build();
        return (int) std::distance(vals.begin(), std::upper_bound(vals.begin(), vals.end(), v));
    }

    value_type original(int p) const {
        build();
        return vals[p];
    }

    int size() const {
        build();
        return (int) vals.size();
    }

    void clear() {
        vals.clear();
        f.clear();
        built = true;
    }

private:
    mutable bool built = true;
    mutable hash_map<value_type, int> f;
    mutable std::vector<value_type> vals;
};

} // namespace ds
} // namespace cp
#endif