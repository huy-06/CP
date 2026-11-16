#include <filesystem>
#include <string>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <process.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
#endif

#ifndef CP_INIT_RUNNER_PLATFORM_HPP
#define CP_INIT_RUNNER_PLATFORM_HPP

namespace cp {
namespace init {
namespace runner {
namespace platform {

namespace fs = std::filesystem;

inline void enable_vt_processing() {
#ifdef _WIN32
    HANDLE h_out = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h_out != INVALID_HANDLE_VALUE) {
        DWORD dw_mode = 0;
        if (GetConsoleMode(h_out, &dw_mode)) {
            dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(h_out, dw_mode);
        }
    }
    HANDLE h_err = GetStdHandle(STD_ERROR_HANDLE);
    if (h_err != INVALID_HANDLE_VALUE) {
        DWORD dw_mode = 0;
        if (GetConsoleMode(h_err, &dw_mode)) {
            dw_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(h_err, dw_mode);
        }
    }
#endif
}

inline int get_pid() {
#ifdef _WIN32
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(getpid());
#endif
}

inline fs::path get_temp_dir() {
    std::error_code ec;
    fs::path p = fs::temp_directory_path(ec);
    if (ec) {
#ifdef _WIN32
        p = "C:\\Windows\\Temp";
#else
        p = "/tmp";
#endif
    }
    return p;
}

} // namespace platform
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_PLATFORM_HPP
