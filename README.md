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
##How to install
> go into the **~(home/username) folder** and type:
- ```text
   git clone 
   cd fym
   chmod +x install.sh
   ./install.sh
  ```

## ⚠️ Platform Support Note
This tool is built natively for **Linux / Unix-based systems (Tested on Arch Linux)**. There is **no official Windows support** right now, as path resolution and multi-threading shells depend on POSIX system behavior.

## File Syntax (`fymfile`)
Create a file named `fymfile` in your project folder. Fields use simple `key: value` syntax wrapped inside `[target: name]` blocks:

```ini
# Global variables can be defined at the very top
COMPILER_FLAGS = -O3 -Wall

[target: hello]
projekt: src
files: {projekt}/*.cpp
executable: build/hello_app
build: g++ {COMPILER_FLAGS} {files} -o {executable}
shell: echo "Target hello successfully compiled!"
```

### Keywords Explained
- `[target: name]`: Defines the start of a clean, isolated build block.
- `projekt:`: Specifies the source directory where your code lives.
- `files:`: Lists the files to compile (supports wildcards like `*.cpp` and variable injection like `{variable}`).
- `executable:`: Sets the final output path/name for your binary.
- `build:`: The exact compilation command passed to the compiler.
- `shell:`: Post-build shell commands executed directly in your system terminal.

## How to Install
You can set up the binary and system icon automatically with a single double-click using the automated installer. 

Simply run the master installation script:
```bash
chmod +x click_to_install.sh
./click_to_install.sh
```
*This will make the secondary scripts executable, register the system MIME-types, and bind the `logo.jpeg` as your native `fymfile` desktop icon.*

## How to Run
Once installed globally, navigate to any directory containing a valid `fymfile` and simply type:
```bash
fym
```

## License & Code Usage
This project is **Source-Available**. You are free to view, inspect, and analyze the source code for educational purposes, and you can use the compiled program free of charge. However, you are **not allowed** to copy, modify, distribute, or steal the source code to republish it under your own name.
