import os
import re
from pathlib import Path

INCLUDE_REGEX = re.compile(r'(#include\s+["<])([^">]+)([">])')

def find_includes(directory, search_text, extensions=("*.hpp", "*.cpp")):
    """Searches for a specific file path inside #include directives."""
    print(f"\n[SEARCH] Looking for includes containing '{search_text}'...")
    found_count = 0
    
    dir_path = Path(directory)
    
    for ext in extensions:
        for filepath in dir_path.rglob(ext):
            try:
                with open(filepath, 'r', encoding='utf-8') as file:
                    for line_num, line in enumerate(file, 1):
                        match = INCLUDE_REGEX.search(line)
                        if match:
                            include_path = match.group(2)
                            if search_text in include_path:
                                print(f"  [+] Match found in: {filepath} (Line {line_num})")
                                print(f"      Code: {line.strip()}")
                                found_count += 1
            except Exception as e:
                print(f"  [!] Error reading {filepath}: {e}")
                
    if found_count == 0:
        print("  [-] No matches found.")
    else:
        print(f"[DONE] Found {found_count} matching include(s).")

def replace_includes(directory, search_text, replace_text, extensions=("*.hpp", "*.cpp")):
    print(f"\n[REPLACE] Replacing includes containing '{search_text}' with '{replace_text}'...")
    modified_files = 0
    
    dir_path = Path(directory)
    
    for ext in extensions:
        for filepath in dir_path.rglob(ext):
            try:
                with open(filepath, 'r', encoding='utf-8') as file:
                    lines = file.readlines()
                
                file_modified = False
                
                for i in range(len(lines)):
                    match = INCLUDE_REGEX.search(lines[i])
                    if match:
                        prefix = match.group(1)
                        include_path = match.group(2)
                        suffix = match.group(3)
                        
                        if search_text in include_path:
                            new_line = f"{prefix}{replace_text}{suffix}\n"
                            
                            leading_whitespace = lines[i][:lines[i].find('#')]
                            lines[i] = leading_whitespace + new_line
                            
                            file_modified = True
                
                if file_modified:
                    with open(filepath, 'w', encoding='utf-8') as file:
                        file.writelines(lines)
                    print(f"  [+] Updated file: {filepath}")
                    modified_files += 1
                    
            except Exception as e:
                print(f"  [!] Error processing {filepath}: {e}")
                
    if modified_files == 0:
        print("  [-] No files were modified.")
    else:
        print(f"[DONE] Successfully updated {modified_files} file(s).")

def main():    
    target_dir = input("Enter target directory path (Press Enter for current directory): ").strip()
    if not target_dir:
        target_dir = "."
        
    if not os.path.isdir(target_dir):
        print("[ERROR] Directory does not exist!")
        return

    while True:
        print("\nOPTIONS:")
        print("1. Find includes")
        print("2. Find and Replace includes")
        print("3. Exit")
        
        choice = input("Enter your choice (1/2/3): ").strip()
        
        if choice == '1':
            search_txt = input("Enter partial path to search: ").strip()
            if search_txt:
                find_includes(target_dir, search_txt)
            
        elif choice == '2':
            search_txt = input("Enter partial path to search: ").strip()
            replace_txt = input("Enter the EXACT NEW PATH: ").strip()
            
            if search_txt and replace_txt:
                confirm = input(f"[WARNING] Replace all include paths containing '{search_txt}' with '{replace_txt}'? (y/n): ")
                if confirm.lower() == 'y':
                    replace_includes(target_dir, search_txt, replace_txt)
                else:
                    print("Operation cancelled.")
                
        elif choice == '3':
            print("Exiting program. Goodbye!")
            break
        else:
            print("Invalid choice, please try again.")

if __name__ == "__main__":
    main()