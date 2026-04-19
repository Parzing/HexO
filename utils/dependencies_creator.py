import os
import re
import sys
from collections import defaultdict
from pathlib import Path

def main():
    # 1. Check for command line argument
    if len(sys.argv) < 2:
        print("Error: Please provide a target directory.")
        print("Usage: python3 dependency_creator.py <path_to_folder>")
        sys.exit(1)

    # 2. Setup paths based on input
    folder_input = sys.argv[1]
    target_folder = Path(folder_input).resolve()

    if not target_folder.is_dir():
        print(f"Error: The path '{folder_input}' is not a valid directory.")
        sys.exit(1)

    output_file = target_folder / "dependencies.txt"
    include_pattern = re.compile(r'#include\s+"([^"]+)"')

    # Group dependencies by top-level folder
    folder_deps = defaultdict(dict)

    # 3. Scan the directory
    for root, _, files in os.walk(target_folder):
        for file in files:
            if file.endswith(('.c', '.h')):
                path = os.path.join(root, file)
                rel_path = os.path.relpath(path, target_folder)
                
                # Determine top folder name for grouping
                parts = rel_path.split(os.sep)
                top_folder = parts[0].upper() if len(parts) > 1 else "ROOT"
                
                try:
                    with open(path, "r", encoding="utf-8") as f:
                        content = f.read()
                    includes = include_pattern.findall(content)
                    folder_deps[top_folder][rel_path] = includes
                except Exception as e:
                    print(f"Could not read {rel_path}: {e}")

    # 4. Write the output
    with open(output_file, "w", encoding="utf-8") as f:
        for folder in sorted(folder_deps.keys()):
            f.write("-" * 28 + "\n")
            f.write(f"{folder}\n")
            f.write("-" * 28 + "\n\n")
            for file, includes in sorted(folder_deps[folder].items()):
                f.write(f"{file}:\n")
                for inc in sorted(set(includes)): # sorted/unique for cleaner output
                    f.write(f'#include "{inc}"\n')
                f.write("\n")

    print(f"Dependencies written to: {output_file}")

if __name__ == "__main__":
    main()