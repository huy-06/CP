#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <vector>
#include <filesystem>
#include <functional>
#include <cctype>

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<std::string> keep_tokens = {
        "@author",
        "@since",
        "namespace"
    };

    std::vector<std::pair<std::string, std::string>> replacements = {
        {"🍀", "tc"},
        {"🌸", "t"},
        {"🌻🌻🌻", "solve"}
    };

    std::vector<std::string> remove_call_patterns = {
        "debug(",
    };

    std::unordered_set<std::string> seen;
    seen.reserve(256);

    std::string root_path_str;
    if (!std::getline(std::cin, root_path_str) || root_path_str.empty()) {
        std::cerr << "No input file provided\n";
        return 1;
    }

    std::string submit_path = "E:\\Code\\CP\\Tasks\\CPP\\submit\\submit.cpp";
    std::ofstream fout(submit_path, std::ios::trunc);
    if (!fout) {
        std::cerr << "Can not open output file: " << submit_path << '\n';
        return 1;
    }

    auto is_all_ws = [](const std::string& s) -> bool {
        for (char c : s) if (!std::isspace(static_cast<unsigned char>(c))) return false;
        return true;
    };

    auto trim = [](const std::string& s) -> std::string {
        size_t a = 0;
        while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
        size_t b = s.size();
        while (b > a && std::isspace(static_cast<unsigned char>(s[b-1]))) --b;
        return s.substr(a, b - a);
    };

    auto trim_trailing_ws = [](std::string& s) {
        while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
    };

    auto trim_leading_ws = [](std::string& s) {
        size_t i = 0;
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        if (i) s.erase(0, i);
    };

    auto replace_all = [&](std::string& line, const std::string& key, const std::string& val) {
        if (key.empty()) return;
        size_t pos = 0;
        while ((pos = line.find(key, pos)) != std::string::npos) {
            line.replace(pos, key.size(), val);
            pos += val.size();
        }
    };

    auto is_comment_only_marker = [&](const std::string& trimmed) -> bool {
        if (trimmed.empty()) return false;
        if (trimmed.rfind("//", 0) == 0) return true;
        if (trimmed.rfind("/*", 0) == 0) return true;
        if (trimmed.rfind("*/", 0) == 0) return true;
        return false;
    };

    auto is_call_only = [&](const std::string& trimmed) -> bool {
        if (trimmed.empty()) return false;
        for (const auto& pat : remove_call_patterns) {
            if (trimmed.rfind(pat, 0) == 0) {
                size_t pos_open = trimmed.find('(', pat.size() - 1);
                if (pos_open == std::string::npos) continue;
                int depth = 0;
                bool started = false;
                size_t j = pos_open;
                for (; j < trimmed.size(); ++j) {
                    char c = trimmed[j];
                    if (c == '(') { ++depth; started = true; }
                    else if (c == ')') {
                        if (started) {
                            --depth;
                            if (depth == 0) { ++j; break; }
                        }
                    }
                }
                if (j <= pos_open) continue;
                size_t k = j;
                while (k < trimmed.size() && std::isspace(static_cast<unsigned char>(trimmed[k]))) ++k;
                if (k < trimmed.size() && trimmed[k] == ';') ++k;
                while (k < trimmed.size() && std::isspace(static_cast<unsigned char>(trimmed[k]))) ++k;
                if (k == trimmed.size()) return true;
            }
        }
        return false;
    };

    auto remove_calls_in_line = [&](std::string& line) {
        if (line.empty()) return;
        for (const auto& pattern : remove_call_patterns) {
            size_t pos_search = 0;
            while ((pos_search = line.find(pattern, pos_search)) != std::string::npos) {
                size_t pos_open = line.find('(', pos_search + pattern.size() - 1);
                if (pos_open == std::string::npos) {
                    break;
                }
                int depth = 0;
                bool started = false;
                size_t j = pos_open;
                for (; j < line.size(); ++j) {
                    char c = line[j];
                    if (c == '(') { ++depth; started = true; }
                    else if (c == ')') {
                        if (started) {
                            --depth;
                            if (depth == 0) {
                                ++j;
                                break;
                            }
                        }
                    }
                }

                size_t end_pos = std::string::npos;
                if (j <= pos_open) {
                    end_pos = line.size();
                } else {
                    size_t k = j;
                    while (k < line.size() && std::isspace(static_cast<unsigned char>(line[k]))) ++k;
                    if (k < line.size() && line[k] == ';') {
                        ++k;
                        while (k < line.size() && std::isspace(static_cast<unsigned char>(line[k]))) ++k;
                        end_pos = k;
                    } else {
                        end_pos = j;
                    }
                }

                size_t start_erase = pos_search;
                while (start_erase > 0 && std::isspace(static_cast<unsigned char>(line[start_erase - 1]))) --start_erase;

                if (end_pos == std::string::npos || end_pos > line.size()) end_pos = line.size();

                if (start_erase == 0 && end_pos >= line.size()) {
                    line.clear();
                    return;
                }

                if (end_pos > start_erase) {
                    line.erase(start_erase, end_pos - start_erase);
                    pos_search = start_erase;
                } else {
                    pos_search += pattern.size();
                }
            }
        }
    };

    std::function<void(const std::filesystem::path&, std::vector<std::string>&)> process_file;
    process_file = [&](const std::filesystem::path& p, std::vector<std::string>& out_lines) {
        std::filesystem::path abs = std::filesystem::absolute(p).lexically_normal();
        std::string key = abs.string();
        if (!seen.insert(key).second) return;

        std::ifstream fin(abs);
        if (!fin) {
            std::cerr << "Can not open file: " << abs << '\n';
            return;
        }

        std::string line;
        std::filesystem::path parent = abs.parent_path();

        bool skipping_angle_block = false;
        bool in_block_comment = false;

        while (std::getline(fin, line)) {
            std::string original = line;
            std::string trimmed_original = trim(original);

            bool original_was_blank = trimmed_original.empty();

            if (in_block_comment) {
                size_t pos_end = line.find("*/");
                if (pos_end == std::string::npos) {
                    continue;
                } else {
                    std::string rest = line.substr(pos_end + 2);
                    line = rest;
                    in_block_comment = false;
                }
            }

            if (skipping_angle_block) {
                size_t pos_close = line.find("//>");
                if (pos_close == std::string::npos) {
                    continue;
                } else {
                    std::string after = line.substr(pos_close + 3);
                    if (trim(after).empty()) {
                        skipping_angle_block = false;
                        continue;
                    } else {
                        line = after;
                        skipping_angle_block = false;
                    }
                }
            }

            size_t pos_comment_all = line.find("//");
            size_t pos_include = line.find("#include \"");
            if (pos_include != std::string::npos &&
                (pos_comment_all == std::string::npos || pos_include < pos_comment_all)) {
                size_t first_q = line.find('"', pos_include);
                size_t second_q = (first_q == std::string::npos) ? std::string::npos : line.find('"', first_q + 1);
                if (first_q != std::string::npos && second_q != std::string::npos && second_q > first_q + 1) {
                    std::string inc_rel = line.substr(first_q + 1, second_q - first_q - 1);
                    std::filesystem::path inc_path = parent / inc_rel;
                    process_file(inc_path, out_lines);
                }
                continue;
            }

            bool resumed_processing = true;
            while (resumed_processing) {
                resumed_processing = false;
                size_t pos_block = line.find("/*");
                if (pos_block != std::string::npos) {
                    size_t pos_block_end = line.find("*/", pos_block + 2);
                    if (pos_block_end != std::string::npos) {
                        line.erase(pos_block, pos_block_end + 2 - pos_block);
                        resumed_processing = true;
                        continue;
                    } else {
                        std::string prefix = line.substr(0, pos_block);
                        trim_trailing_ws(prefix);
                        if (trim(prefix).empty()) {
                            in_block_comment = true;
                            line.clear();
                            break;
                        } else {
                            line = prefix;
                            in_block_comment = true;
                            break;
                        }
                    }
                }
            }
            if (in_block_comment && trim(line).empty()) {
                if (original_was_blank) out_lines.push_back(std::string());
                continue;
            }

            pos_comment_all = line.find("//");
            if (pos_comment_all != std::string::npos) {
                std::string comment_part = line.substr(pos_comment_all);
                bool keep_this_comment = false;
                for (const auto& tok : keep_tokens) {
                    if (comment_part.find(tok) != std::string::npos) {
                        keep_this_comment = true;
                        break;
                    }
                }
                if (keep_this_comment) {
                    // ...
                } else {
                    size_t pos_bang = line.find("//!");
                    if (pos_bang != std::string::npos) {
                        std::string prefix = line.substr(0, pos_bang);
                        std::string tail = (pos_bang + 3 < line.size()) ? line.substr(pos_bang + 3) : std::string();
                        size_t j = 0;
                        while (j < tail.size() && (tail[j] == '!' || tail[j] == ' ' || tail[j] == '\t')) ++j;
                        std::string out_tail = (j < tail.size()) ? tail.substr(j) : std::string();
                        bool prefix_ends_ws = !prefix.empty() &&
                                              (prefix.back() == ' ' || prefix.back() == '\t');
                        std::string merged;
                        if (!out_tail.empty() && !prefix.empty() && !prefix_ends_ws) {
                            merged = prefix + ' ' + out_tail;
                        } else {
                            merged = prefix + out_tail;
                        }
                        line = merged;
                    } else {
                        size_t pos_langle = line.find("//<");
                        if (pos_langle != std::string::npos) {
                            size_t pos_rangle = line.find("//>");
                            std::string prefix = line.substr(0, pos_langle);
                            trim_trailing_ws(prefix);
                            if (pos_rangle != std::string::npos) {
                                continue;
                            } else {
                                if (trim(prefix).empty()) {
                                    skipping_angle_block = true;
                                    continue;
                                } else {
                                    out_lines.push_back(prefix);
                                    skipping_angle_block = true;
                                    continue;
                                }
                            }
                        } else {
                            std::string prefix = line.substr(0, pos_comment_all);
                            if (trim(prefix).empty()) {
                                continue;
                            } else {
                                line.erase(pos_comment_all);
                                trim_trailing_ws(line);
                            }
                        }
                    }
                }
            }

            std::string processed = line;
            std::string trimmed_processed = trim(processed);

            remove_calls_in_line(processed);

            for (const auto& kv : replacements) {
                replace_all(processed, kv.first, kv.second);
            }

            std::string final_trim = trim(processed);
            if (final_trim.empty()) {
                if (original_was_blank) {
                    out_lines.push_back(std::string());
                } else if (is_comment_only_marker(trimmed_original)) {
                    continue;
                } else if (is_call_only(trimmed_original)) {
                    continue;
                } else {
                    out_lines.push_back(std::string());
                }
            } else {
                out_lines.push_back(processed);
            }
        }
    };

    std::vector<std::string> out_lines;
    process_file(std::filesystem::path(root_path_str), out_lines);

    for (size_t i = 0; i < out_lines.size(); ++i) {
        fout << out_lines[i];
        if (i + 1 < out_lines.size()) fout << '\n';
    }

    fout.close();
    return 0;
}