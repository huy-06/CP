#include <ext/pb_ds/assoc_container.hpp>

#ifndef HUY_DS_ORDER_SET
#define HUY_DS_ORDER_SET
namespace huy {
namespace ds {

template <typename K, typename V>
using map = __gnu_pbds::tree<K, 
                             V, 
                             std::less<K>,
                             __gnu_pbds::rb_tree_tag,
                             __gnu_pbds::tree_order_statistics_node_update>;

template <typename Tp>
using set = map<Tp, __gnu_pbds::null_type>;                  

} // namespace ds
} // namespace huy
#endif