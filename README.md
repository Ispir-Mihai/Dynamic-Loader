# Dynamic Loader

A lightweight dynamic loader that searches through the Process Environment Block (PEB) to locate and load functions from loaded modules without relying on WinAPI calls. This loader is designed to be less detectable by antivirus programs.

## Features

- **PEB-based Function Loading**: Instead of using standard WinAPI functions like `GetModuleHandle` and `GetProcAddress`, this loader directly interacts with the PEB to find and load functions.
- **No WinAPI Calls**: The loader avoids common API calls that are often monitored by antivirus, reducing the likelihood of detection.
- **Cross-Architecture**: Works on both 32-bit (x86) and 64-bit (x64) Windows systems.

## How It Works

The loader operates by parsing the **PEB** of the current process to retrieve information about loaded modules and their export tables. It then searches for the specified function name within the export table of each loaded module and returns the address of the function if found.

### Process Flow:
1. **PEB Parsing**: The loader first retrieves the base address of the PEB.
2. **Export Table Access**: It accesses the export table of each loaded module by parsing the PEB.
3. **Function Search**: The loader searches for the specified function name in the export directory.
4. **Function Call**: Once the function is found, the loader retrieves the address and calls the function with the provided arguments.

### TODO
- Add usage docs in README.md
- Add exemples in README.md
