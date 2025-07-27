# 🐚 My Own Shell – CodeCrafters Challenge

This project is my implementation of a Unix-style shell, built step-by-step as part of the [CodeCrafters](https://codecrafters.io) "Build Your Own Shell" challenge.

## Features Implemented

### Core Stages
- [x] **Print a prompt**  
- [x] **Handle invalid commands**
- [x] **REPL loop** (read-eval-print loop)
- [x] **`exit` builtin**
- [x] **`echo` builtin**
- [x] **`type` builtin for shell builtins**
- [x] **`type` builtin for executables**
- [x] **Execute external programs**


### Navigation
- [x] **`pwd` builtin** – prints current working directory
- [x] **`cd` with absolute paths**
- [x] **`cd` with relative paths**
- [x] **`cd` with no args → goes to `$HOME`**

### Quoting
- [x] **Single quotes** `'...'` – preserves all characters literally
- [x] **Double quotes** `"..."` – allows variable expansion and escape sequences (in later stages)
- [x] **Backslash outside quotes** – escapes the next character
- [x] **Backslash within single quotes** – treated literally (no escape)
- [x] **Backslash within double quotes** – escapes `"`, `\`, `$`, and `` ` ``
- [x] **Quoted executable names**

---

## Setup Instructions

In order to compile the shell, you need to have a C compiler and CMake installed.

On Ubuntu/Debian:

```bash
sudo apt update
sudo apt install build-essential
sudo apt install cmake
```

On MacOS:

```bash
xcode-select --install
brew install cmake
```

Additionally, you need to install vcpkg by running the setup.sh script in the repository root and following the instructions to set up the VCPKG_ROOT environment variable:
