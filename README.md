# Dynamic Loader

A lightweight dynamic loader that searches through the Process Environment Block (PEB) to locate and load functions from loaded modules without relying on WinAPI calls. This loader is designed to be less detectable by antivirus programs and cheat detection systems.

## Features

- **PEB-based Function Loading**: Instead of using standard WinAPI functions like `GetProcAddress`, this loader directly interacts with the PEB to find and load functions.
- **No WinAPI Calls**: The loader avoids common API calls that are often monitored by antivirus and anti-cheat software, reducing the likelihood of detection.
- **Cross-Platform**: This loader can be used in environments where WinAPI is not available, as long as the PEB structure is accessible.

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
