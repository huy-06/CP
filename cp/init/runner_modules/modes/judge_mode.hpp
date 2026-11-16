#include <iostream>
#include <string>
#include <filesystem>
#include <iomanip>
#include <cstdlib>
#include "../config.hpp"
#include "../style.hpp"
#include "../process.hpp"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <stdlib.h>
#endif

#ifndef CP_INIT_RUNNER_MODES_JUDGE_MODE_HPP
#define CP_INIT_RUNNER_MODES_JUDGE_MODE_HPP

namespace cp {
namespace init {
namespace runner {
namespace modes {

namespace fs = std::filesystem;

inline void set_env_var(const std::string& name, const std::string& value) {
#ifdef _WIN32
    _putenv_s(name.c_str(), value.c_str());
#else
    setenv(name.c_str(), value.c_str(), 1);
#endif
}

inline int run_judge(const fs::path& sol_exe,
                     const fs::path& judge_exe,
                     const std::string& judge_name,
                     int iters = 1) {
    std::cout << style::color_white << "mode: " << style::color_green << "judge" << style::reset << "\n";
    std::cout << style::color_white << "judge: " << style::color_cyan << judge_name << style::reset << "\n";

    set_env_var("CP_SOL_EXE", sol_exe.string());
    set_env_var("CP_TIME_LIMIT", std::to_string(config::time_limit));

    int passed = 0;
    for (int iter = 1; iter <= iters; ++iter) {
        set_env_var("CP_TEST_NUM", std::to_string(iter));

        // Judge chạy tối đa gấp đôi thời gian time_limit
        double judge_timeout = config::time_limit * 2.5;
        run_result j_res = run_with_timeout(judge_exe.string(), std::to_string(iter) + "\n", judge_timeout);

        if (j_res.is_timeout || j_res.exit_code == 3) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "tle " << style::color_white << "[" << style::color_yellow << "> " << std::fixed << std::setprecision(3) << config::time_limit << "s" << style::color_white << "]" << style::reset << "\n";
            if (!j_res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << j_res.error_output << style::reset << (j_res.error_output.back() == '\n' ? "" : "\n");
            }
            return 1;
        }

        if (j_res.exit_code == 0) {
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_green << "ac " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << j_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            if (!j_res.output.empty()) {
                std::cout << j_res.output << (j_res.output.back() == '\n' ? "" : "\n");
            }
            passed++;
        } else if (j_res.exit_code == 1) {
            // WA
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "wa " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << j_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            if (!j_res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << j_res.error_output << style::reset << (j_res.error_output.back() == '\n' ? "" : "\n");
            }
            if (!j_res.output.empty()) {
                std::cout << j_res.output << (j_res.output.back() == '\n' ? "" : "\n");
            }
            return 1;
        } else {
            // RTE (code 2 or crash)
            std::cout << style::color_white << "test " << style::color_blue << iter << style::color_white << ": "
                      << style::color_red << "rte " << style::color_white << "[" << style::color_yellow << std::fixed << std::setprecision(3) << j_res.time_taken << "s" << style::color_white << "]" << style::reset << "\n";
            if (!j_res.error_output.empty()) {
                std::cout << style::color_black << "debug:" << style::reset << "\n";
                std::cout << style::color_yellow << j_res.error_output << style::reset << (j_res.error_output.back() == '\n' ? "" : "\n");
            }
            if (!j_res.output.empty()) {
                std::cout << j_res.output << (j_res.output.back() == '\n' ? "" : "\n");
            }
            return 1;
        }
    }

    if (iters > 1 && passed == iters) {
        std::cout << style::color_green << "all " << iters << " tests passed!" << style::reset << "\n";
    }
    return 0;
}

} // namespace modes
} // namespace runner
} // namespace init
} // namespace cp

#endif // CP_INIT_RUNNER_MODES_JUDGE_MODE_HPP
