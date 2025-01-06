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

### Usage

## **Loading a Module**
- **Constructor**: `DynamicLoader(const std::string& moduleName)`
  - **Purpose**: Loads the specified module.
  - **Parameters**: 
    - `moduleName`: The name of the module to load.

## **Calling a Function**
- **Method**: `template<typename T, typename ...Args> decltype(auto) CallFunction(const char* funcName, Args&& ...args)`
  - **Purpose**: Calls a function from the loaded module by its name.
  - **Parameters**: 
    - `funcName`: The name of the function to call.
    - `args`: The arguments to pass to the function.
  - **Return**: Returns the result of the function call, with the return type specified by `T`.

### Examples
```cpp
#include "DynamicLoader.h"

int main()
{
    DynamicLoader kernel32("kernel32.dll");

    // If other dlls are required and not found in the PEB they can be loaded like so:
    kernel32.CallFunction<HMODULE>("LoadLibraryA", "user32.dll");

	  DynamicLoader user32("user32.dll");
	  user32.CallFunction<int>("MessageBoxA", nullptr, "Hello World", "Hello", MB_OK);

    return 0;
}
```
Or inherit the `DynamicLoader` for a specific module for more organized and controlled code (still not sure about this)
```cpp
#include "DynamicLoader.h"
#include <iostream>

struct Kernel32 : public DynamicLoader
{
	Kernel32() : DynamicLoader("kernel32.dll") {}

	HMODULE LoadLibraryA(LPCSTR lpLibFileName)
	{
		return CallFunction<HMODULE>("LoadLibraryA", *lpLibFileName);
	}
};

struct User32 : public DynamicLoader
{
	User32() : DynamicLoader("user32.dll") {}

	int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT* uType)
	{
		return CallFunction<int>("MessageBoxA", *hWnd, *lpText, *lpCaption, *uType);
	}
};

int main()
{
	Kernel32 kernel32;
	kernel32.LoadLibraryA("user32.dll");

	User32 user32;
	user32.MessageBoxA(nullptr, "Hello, World!", "Hello", MB_OK);

    return 0;
}
```

