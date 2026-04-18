import os
import re
from collections import defaultdict
from pathlib import Path

# Change this to the folder you want to scan
FOLDER_PATH = "../src"
OUTPUT_FILE_PATH = FOLDER_PATH + "/dependencies.txt"
FOLDER = Path(__file__).parent / FOLDER_PATH
OUTPUT_FILE = Path(__file__).parent / OUTPUT_FILE_PATH

include_pattern = re.compile(r'#include\s+"([^"]+)"')

# Group dependencies by top-level folder
folder_deps = defaultdict(dict)

for root, _, files in os.walk(FOLDER):
    for file in files:
        if file.endswith(('.c', '.h')):
            path = os.path.join(root, file)
            rel_path = os.path.relpath(path, FOLDER)
            top_folder = rel_path.split(os.sep)[0].upper()  # CLIENT, COMMON, etc.
            with open(path, "r", encoding="utf-8") as f:
                content = f.read()
            includes = include_pattern.findall(content)
            folder_deps[top_folder][rel_path] = includes

with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
    for folder in sorted(folder_deps.keys()):
        f.write("-" * 28 + "\n")
        f.write(f"{folder}\n")
        f.write("-" * 28 + "\n\n")
        for file, includes in sorted(folder_deps[folder].items()):
            f.write(f"{file}:\n")
            for inc in includes:
                f.write(f'#include "{inc}"\n')
            f.write("\n")

print(f"Dependencies written to {OUTPUT_FILE}")