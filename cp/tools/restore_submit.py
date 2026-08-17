import os
import re

CP_LIB_DIR = r"E:\Code\CP\Tasks\CPP\cp"

INPUT_FILE = r"E:\Code\CP\Tasks\CPP\submit\submit.cpp"

OUTPUT_FILE = r"E:\Code\CP\Tasks\CPP\submit\unbundled.cpp"

DEFAULT_BASE_HEADER = "cp/init/library.hpp"

def resolve_include_path(inc_raw, current_dir, base_dir, cp_dir):
    possible_paths = [
        os.path.normpath(os.path.join(current_dir, inc_raw)),
        os.path.normpath(os.path.join(base_dir, inc_raw)),
        os.path.normpath(os.path.join(cp_dir, inc_raw)),
    ]
    for p in possible_paths:
        if os.path.isfile(p):
            rel = os.path.relpath(p, base_dir).replace('\\', '/')
            return rel
    return None

def scan_library(cp_dir):
    macro_to_path = {}
    dependencies = {}
    
    norm_cp_dir = os.path.normpath(cp_dir)
    base_dir = os.path.dirname(norm_cp_dir)

    for root, _, files in os.walk(norm_cp_dir):
        for file in files:
            if file.endswith(('.hpp', '.h')):
                filepath = os.path.join(root, file)
                rel_path = os.path.relpath(filepath, base_dir).replace('\\', '/')
                dependencies[rel_path] = set()

                try:
                    with open(filepath, 'r', encoding='utf-8') as f:
                        content = f.read()

                        match = re.search(r'#ifndef\s+([A-Za-z0-9_]+)', content)
                        if match:
                            macro = match.group(1)
                            macro_to_path[macro] = rel_path

                        raw_includes = re.findall(r'#include\s+"([^"]+)"', content)
                        for inc in raw_includes:
                            resolved = resolve_include_path(inc, root, base_dir, norm_cp_dir)
                            if resolved:
                                dependencies[rel_path].add(resolved)

                except Exception as e:
                    print(f"Error reading {filepath}: {e}")

    return macro_to_path, dependencies

def get_all_deep_dependencies(target_path, dependencies, visited=None):
    if visited is None:
        visited = set()

    if target_path not in dependencies:
        return visited

    for dep in dependencies[target_path]:
        if dep not in visited:
            visited.add(dep)
            get_all_deep_dependencies(dep, dependencies, visited)

    return visited

def unbundle(input_path, output_path, macro_to_path, dependencies):
    if not os.path.exists(input_path):
        print(f"Cannot find input file: {input_path}")
        return

    with open(input_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    found_paths_in_order = []
    seen_paths = set()
    code_lines = []
    header_comments = []

    i = 0
    n = len(lines)
    in_header = True

    while i < n:
        line = lines[i]

        if in_header and line.strip().startswith('///'):
            header_comments.append(line)
            i += 1
            continue
        elif line.strip() != "":
            in_header = False

        if re.match(r'^\s*#include\s+<.*>', line):
            i += 1
            continue

        match_ifndef = re.match(r'^\s*#ifndef\s+([A-Za-z0-9_]+)', line)
        if match_ifndef:
            macro = match_ifndef.group(1)
            
            if macro in macro_to_path:
                path = macro_to_path[macro]
                if path not in seen_paths:
                    seen_paths.add(path)
                    found_paths_in_order.append(path)

            depth = 1
            i += 1
            while i < n and depth > 0:
                curr_line = lines[i]
                if re.match(r'^\s*#if', curr_line):
                    depth += 1
                elif re.match(r'^\s*#endif', curr_line):
                    depth -= 1
                i += 1
            continue

        if not in_header:
            code_lines.append(line)
        i += 1

    minimal_paths = set(found_paths_in_order)

    if DEFAULT_BASE_HEADER in dependencies:
        base_deps = get_all_deep_dependencies(DEFAULT_BASE_HEADER, dependencies)
        minimal_paths -= base_deps

    for path in found_paths_in_order:
        if path in minimal_paths:
            child_deps = get_all_deep_dependencies(path, dependencies)
            minimal_paths -= child_deps

    out_lines = []
    out_lines.extend(header_comments)
    
    out_lines.append(f'#include "{DEFAULT_BASE_HEADER}"\n')

    for path in found_paths_in_order:
        if path in minimal_paths:
            out_lines.append(f'#include "{path}"\n')

    out_lines.append('\n')
    out_lines.extend(code_lines)

    result_text = "".join(out_lines)
    result_text = re.sub(r'\n{3,}', '\n\n', result_text)

    with open(output_path, 'w', encoding='utf-8') as f:
        f.write(result_text.strip() + '\n')

    print(f"Successfully unbundled! Minimal includes generated in: {output_path}")

if __name__ == "__main__":
    print("Scanning CP library and resolving dependencies...")
    macro_to_path, dependencies = scan_library(CP_LIB_DIR)
    print(f"Mapped {len(macro_to_path)} macros across library files.")

    print("Unbundling file...")
    unbundle(INPUT_FILE, OUTPUT_FILE, macro_to_path, dependencies)