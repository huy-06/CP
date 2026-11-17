#include <string>
#include <vector>
#include <cstddef>

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
inline std::string submit_path = "";
inline std::vector<std::string> bundle_dirs = {"init", "alg", "ds", "cst", "dbg", "cppx"};
inline std::vector<std::string> bundle_skip = {"judge.hpp"};
inline bool show_debug = true;
inline bool show_log = true;
inline bool remove_emojis = true;
inline bool strict_ascii_only = false;

} // namespace config
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_CONFIG_HPP
