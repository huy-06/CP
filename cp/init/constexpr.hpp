#ifndef CP_CONSTEXPR
#define CP_CONSTEXPR

#if __cplusplus >= 202002L
#define cp_constexpr constexpr
#else
#define cp_constexpr
#endif

#if __cplusplus >= 202002L
#define cp_if_constexpr if constexpr
#else
#define cp_if_constexpr if
#endif


#endif