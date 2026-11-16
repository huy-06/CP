#include <filesystem>
#include <string>
#include <string_view>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <atomic>
#include "platform.hpp"

#ifndef CP_INIT_RUNNER_TEMP_HPP
#define CP_INIT_RUNNER_TEMP_HPP

namespace cp {
namespace init {
namespace runner {

namespace fs = std::filesystem;

class temp_session {
public:
    fs::path dir;

    temp_session() {
        auto now = std::chrono::steady_clock::now().time_since_epoch().count();
        int pid = platform::get_pid();
        std::string folder_name = "cp_runner_" + std::to_string(pid) + "_" + std::to_string(now);
        dir = platform::get_temp_dir() / folder_name;
        
        std::error_code ec;
        fs::create_directories(dir, ec);

        setup_cleanup_handlers();
    }

    ~temp_session() {
        clean();
    }

    // Không copy
    temp_session(const temp_session&) = delete;
    temp_session& operator=(const temp_session&) = delete;

    fs::path exe(std::string_view stem) const {
#ifdef _WIN32
        return dir / (std::string(stem) + ".exe");
#else
        return dir / std::string(stem);
#endif
    }

    fs::path temp_file(std::string_view prefix, std::string_view ext = "") const {
        static std::atomic<uint64_t> counter{0};
        uint64_t id = ++counter;
        std::string filename = std::string(prefix) + "_" + std::to_string(id) + std::string(ext);
        return dir / filename;
    }

    void clean() {
        if (!dir.empty()) {
            std::error_code ec;
            fs::remove_all(dir, ec);
        }
    }

    static temp_session& instance() {
        static temp_session s;
        return s;
    }

private:
    static void signal_handler(int sig) {
        instance().clean();
        std::signal(sig, SIG_DFL);
        std::raise(sig);
    }

    static void atexit_handler() {
        instance().clean();
    }

    void setup_cleanup_handlers() {
        static bool setup = false;
        if (!setup) {
            setup = true;
            std::atexit(atexit_handler);
            std::signal(SIGINT, signal_handler);
            std::signal(SIGTERM, signal_handler);
#ifndef _WIN32
            std::signal(SIGQUIT, signal_handler);
            std::signal(SIGHUP, signal_handler);
#endif
        }
    }
};

} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_TEMP_HPP
