#ifndef CP_ALG_POW
#define CP_ALG_POW
namespace cp {
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
} // namespace cp
#endif