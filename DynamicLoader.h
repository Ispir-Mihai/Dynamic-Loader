#pragma once

#include <string>
#include <Windows.h>
#include <stdexcept>

// Forward declarations
struct _PEB;
typedef struct _PEB *PPEB;

/// <summary>
/// Dynamic loads a module and its functions without using generic WinApi functions (LoadLibrary, GetProcAddress)
/// </summary>
class DynamicLoader
{
public:
	/// <summary>
	/// Load a module
	/// </summary>
	/// <param name="moduleName">The name of the module</param>
	DynamicLoader(const std::string& moduleName);
    DynamicLoader() = default;
	virtual ~DynamicLoader() = default;

	/// <summary>
	/// Call a function in the module by name
	/// </summary>
	/// <param name="funcName">The name of the function</param>
	/// <param name="args">The arguments to pass to the function</param>
	/// <returns>The return value of the function</returns>
	template<typename T, typename ...Args>
	decltype(auto) CallFunction(const char* funcName, Args&& ...args)
	{
		// Get the address of the function
		FARPROC funcAddr = GetProcAddress(funcName);
		if (!funcAddr)
			throw std::runtime_error("Failed to get function address");

		// Call the function
		return reinterpret_cast<T(__stdcall*)(Args...)>(funcAddr)(std::forward<Args>(args)...);
	}

private:
	PVOID hModule = nullptr;
    
	/// <summary>
	/// Get the Process Environment Block (PEB) for x86 and x64
	/// </summary>
	/// <returns>The PEB</returns>
	PPEB GetPEB() const;

    /// <summary>
    /// Get the address of a function in the module
    /// </summary>
    /// <param name="funcName">The function name</param>
    /// <returns>The address of the function</returns>
    FARPROC GetProcAddress(const char* funcName) const;
};