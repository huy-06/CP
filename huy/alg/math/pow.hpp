#ifndef HUY_ALG_POW
#define HUY_ALG_POW
namespace huy {
namespace alg {

template <typename Tp, typename Int>
constexpr Tp pow(Tp a, Int b, Tp res = 1) noexcept {
   for (; b; b >>= 1, a *= a) {
       if (b & 1)
           res *= a;
   }
   return res;
}

} // namespace alg
} // namespace huy
#endif