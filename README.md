# ⚡ fymbuild

```text
  ██████╗ ██╗   ██╗███╗   ███╗
  ██╔═══╝ ╚██╗ ██╔╝████╗ ████║
  █████╗   ╚████╔╝ ██╔████╔██║
  ██╔══╝    ╚██╔╝  ██║╚██╔╝██║
  ██║        ██║   ██║ ╚═╝ ██║
  ╚═╝        ╚═╝   ╚═╝     ╚═╝
```

> **FYM** stands for **"F*ck You Make"**. It is a minimal, lightning-fast build system written in C++20, designed as a modern, parallel, and order-independent alternative to over-complicated legacy Makefiles.

## What it does
- **Order-Independent Parsing**: Fields inside target configuration blocks can be written in absolutely any order.
- **Asynchronous Multithreading**: Targets compile concurrently in the background using C++ `std::async`.
- **Wildcard Resolution**: Dynamically resolves source files like `*.cpp` inside specified project directories.

## How to install
> go into the **~(home/username) folder** and type:
- ```text
   git clone https://github.com/Leonski2026/fym.git
   cd fym
   chmod +x install.sh
   ./install.sh
  ```
  **in the Terminal(bash)**
  

## ⚠️ Platform Support Note
This tool is built natively for **Linux / Unix-based systems (Tested on Arch/Fedora Linux)**. There is **no official Windows support** right now, as path resolution and multi-threading shells depend on POSIX system behavior.

## File Syntax (`fymfile`)
Create a file named `fymfile` in your project folder. Fields use simple `key: value` syntax wrapped inside `[target: name]` blocks:

```ini
# Global variables can be defined at the very top
COMPILER_FLAGS = -O3 -Wall

[target: hello]
projekt: src
files: {projekt}/*.cpp
executable: build/hello_app
COMPILER_FLAGS = -Wall -O3 -std=c++20
build: g++ {COMPILER_FLAGS} {files} -o {executable}
shell: echo "Target hello successfully compiled!"
```
*COMPILER_FLAGS is a example for a var,you can make your own!*

### Keywords Explained
- `[target: name]`: Defines the start of a clean, isolated build block.
- `projekt:`: Specifies the source directory where your code lives.
- `files:`: Lists the files to compile (supports wildcards like `*.cpp` and variable injection like `{variable}`).
- `executable:`: Sets the final output path/name for your binary.
- `build:`: The exact compilation command passed to the compiler.
- `shell:`: Post-build shell commands executed directly in your system terminal.
- `NAME = VALUE` : a var in fym



## How to Run
Once installed globally, navigate to any directory containing a valid `fymfile` and simply type:
```bash
fym
```

## License & Code Usage
This project is **Source-Available**. You are free to view, inspect, and analyze the source code for educational purposes, and you can use the compiled program free of charge. However, you are **not allowed** to copy, modify, distribute, or steal the source code to republish it under your own name.
