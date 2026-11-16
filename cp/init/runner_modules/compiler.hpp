#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <utility>
#include <future>
#include <mutex>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <atomic>
#include <chrono>

#include "config.hpp"
#include "style.hpp"

#ifndef CP_INIT_RUNNER_COMPILER_HPP
#define CP_INIT_RUNNER_COMPILER_HPP

namespace cp {
namespace init {
namespace runner {

namespace fs = std::filesystem;

struct compile_result {
    bool        ok = false;
    std::string error_msg;
    fs::path    exe_path;
};

namespace detail {

inline std::string make_compile_temp_file() {
    static std::atomic<uint64_t> counter{0};
    auto now = std::chrono::steady_clock::now().time_since_epoch().count();
    uint64_t c = counter.fetch_add(1);
    std::error_code ec;
    fs::path p = fs::temp_directory_path(ec);
    if (ec) {
        p = fs::current_path();
    }
    p /= "cp_compile_err_" + std::to_string(now) + "_" + std::to_string(c) + ".txt";
    return p.string();
}

inline std::mutex& get_cerr_mutex() {
    static std::mutex mtx;
    return mtx;
}

} // namespace detail

// Compile src_file → exe_path dùng config::compiler_cmd
// extra_flags: e.g. "-I cp/init" cho judge mode
// In error ra stderr nếu thất bại
inline compile_result compile_target(
    const fs::path& src_file,
    const fs::path& exe_path,
    std::string_view extra_flags = ""
) {
    compile_result res;
    res.exe_path = exe_path;

    std::error_code ec;
    if (exe_path.has_parent_path()) {
        fs::create_directories(exe_path.parent_path(), ec);
    }

    std::string cmd = config::compiler_cmd;
    if (!extra_flags.empty()) {
        cmd += " ";
        cmd += extra_flags;
    }
    cmd += " \"" + src_file.string() + "\" -o \"" + exe_path.string() + "\"";

    std::string err_file = detail::make_compile_temp_file();
    std::string full_cmd = cmd + " 2> \"" + err_file + "\"";

    int ret = std::system(full_cmd.c_str());

    std::string err_output;
    {
        std::ifstream f(err_file, std::ios::binary);
        if (f) {
            std::ostringstream ss;
            ss << f.rdbuf();
            err_output = ss.str();
        }
    }
    fs::remove(err_file, ec);

    if (ret == 0 && fs::exists(exe_path, ec)) {
        res.ok = true;
        res.error_msg.clear();
    } else {
        res.ok = false;
        res.error_msg = err_output;
        {
            std::lock_guard<std::mutex> lock(detail::get_cerr_mutex());
            std::cerr << style::color_red << "compile error: " << style::reset 
                      << src_file.filename().string() << "\n" 
                      << res.error_msg;
            if (!res.error_msg.empty() && res.error_msg.back() != '\n') {
                std::cerr << "\n";
            }
        }
    }

    return res;
}

// Compile nhiều file song song dùng std::async
// Trả về vector<compile_result> cùng thứ tự với sources
inline std::vector<compile_result> compile_parallel(
    const std::vector<std::pair<fs::path, fs::path>>& src_exe_pairs,
    std::string_view extra_flags = ""
) {
    std::vector<std::future<compile_result>> futures;
    futures.reserve(src_exe_pairs.size());

    for (const auto& [src, exe] : src_exe_pairs) {
        futures.push_back(std::async(std::launch::async, [src, exe, extra_flags]() {
            return compile_target(src, exe, extra_flags);
        }));
    }

    std::vector<compile_result> results;
    results.reserve(futures.size());
    for (auto& fut : futures) {
        results.push_back(fut.get());
    }

    return results;
}

} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_COMPILER_HPP
