#include <vector>
#include <queue>
#include "../ds/set/order_set.hpp"
#include "../ds/hash/hash_map.hpp"

#ifndef CP_CPPX_UTILS
#define CP_CPPX_UTILS

namespace cp {    

using std::pair;

using str = std::string;

using i128 = __int128;
using u128 = unsigned __int128;

using pii = std::pair<int64_t, int64_t>;

using vs = std::vector<str>;
using vb = std::vector<bool>;
using vi = std::vector<int64_t>;
using vf = std::vector<long double>;
using vii = std::vector<pii>;
using vvi = std::vector<vi>;
using vvs = std::vector<vs>;
using vvf = std::vector<vf>;
using vvb = std::vector<vb>;
using vvii = std::vector<vii>;
using vvvi = std::vector<vvi>;
using viii = std::vector<std::array<int64_t, 3>>;

using sei = ds::set<int64_t>;
using ses = ds::set<str>;

using mii = ds::unordered_map<int64_t, int64_t>;
using mis = ds::unordered_map<int64_t, str>;
using msi = ds::unordered_map<str, int64_t>;
using mss = ds::unordered_map<str, str>;

using qi = std::queue<int64_t>;
using qii = std::queue<pii>;

using dqi = std::deque<int64_t>;
using dqii = std::deque<std::pair<int64_t, int64_t>>;

using pqminii = std::priority_queue<pii, vii, std::greater<pii>>;

using pvv = std::pair<vi, vi>;
using pmm = std::pair<mii, mii>;

constexpr int64_t inf = std::numeric_limits<int64_t>::max() >> 4;

template <typename Tp>
using vec = std::vector<Tp>;

template <typename Tp, size_t N>
using arr = std::array<Tp, N>;

} // namespace cp

#endif