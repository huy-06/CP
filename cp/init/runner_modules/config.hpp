#include <string>
#include <vector>
#include <cstddef>
#include <limits>

#ifndef CP_INIT_RUNNER_CONFIG_HPP
#define CP_INIT_RUNNER_CONFIG_HPP

namespace cp {
namespace init {
namespace runner {
namespace config {

inline double time_limit = 5.0;
inline long double float_eps = 1e-6L;
inline std::string compiler_cmd = "g++ -std=c++23 -O2";
inline bool enable_bundle = true;
inline bool bundle_only = false;
inline bool save_failed = false;
inline bool append_failed_to_src = true; 
inline std::string failed_save_dir = "wa_tests";
inline std::size_t preview_max_tokens = 40;
inline std::size_t preview_max_token_len = 100;
inline std::size_t debug_max_tokens = 80;
inline std::string submit_path = "";
inline std::vector<std::string> bundle_dirs = {"init", "alg", "ds", "cst", "dbg", "cppx"};
inline std::vector<std::string> bundle_skip = {"judge.hpp"};
inline bool show_debug = true;
inline bool show_log = true;
inline bool remove_emojis = true;
inline bool strict_ascii_only = false;

// Token overrides cho từng phần:
// 0: dùng preview_max_tokens (hoặc debug_max_tokens cho debug)
// n > 0: in tối đa n tokens
inline std::size_t in_max_tokens = 0;
inline std::size_t out_max_tokens = 0;
inline std::size_t actual_max_tokens = 0;
inline std::size_t custom_debug_tokens = 0;

// Smart mismatch diff
inline bool show_diff = false;
inline std::size_t diff_context_radius = 4;

inline std::size_t get_in_tokens() noexcept {
    return in_max_tokens > 0 ? in_max_tokens : preview_max_tokens;
}

inline std::size_t get_out_tokens() noexcept {
    return out_max_tokens > 0 ? out_max_tokens : preview_max_tokens;
}

inline std::size_t get_actual_tokens() noexcept {
    if (actual_max_tokens > 0) return actual_max_tokens;
    if (out_max_tokens > 0) return out_max_tokens;
    return preview_max_tokens;
}

inline std::size_t get_debug_tokens() noexcept {
    return custom_debug_tokens > 0 ? custom_debug_tokens : debug_max_tokens;
}

} // namespace config
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_CONFIG_HPP
