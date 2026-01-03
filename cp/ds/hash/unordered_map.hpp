
#include <ext/pb_ds/assoc_container.hpp>
#include "custom_hash.hpp"

#ifndef CP_DS_HASH_UNORDERED_MAP
#define CP_DS_HASH_UNORDERED_MAP
namespace cp {
namespace ds {

template <class K, class V>
using unordered_map = __gnu_pbds::gp_hash_table<
    K,
    V,
    internal::custom_hash,
    std::equal_to<K>,
    __gnu_pbds::direct_mask_range_hashing<>,
    __gnu_pbds::linear_probe_fn<>,
    __gnu_pbds::hash_standard_resize_policy<
        __gnu_pbds::hash_exponential_size_policy<>,
        __gnu_pbds::hash_load_check_resize_trigger<>,
        true // external size access
    >
>;

} // namespace ds
} // namespace cp
#endif