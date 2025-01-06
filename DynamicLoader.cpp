#include "DynamicLoader.h"

#include <locale>
#include <codecvt>

#include "WinStructs.h"

DynamicLoader::DynamicLoader(const std::string& moduleName)
{
	// Convert the module name to a wide string
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wModuleName = converter.from_bytes(moduleName);

	// Get the PEB, which contains the linked module list
    PPEB peb = GetPEB();
    if (peb == nullptr)
        throw std::runtime_error("Failed to get PEB.");
    
	if (peb->LdrData == nullptr)
		throw std::runtime_error("Failed to get LdrData.");

    PLIST_ENTRY moduleList = &peb->LdrData->InMemoryOrderModuleList;

	// Iterate through the linked list of modules
    PLIST_ENTRY current = moduleList->Flink;
    while (current != moduleList) {
        PLDR_DATA_TABLE_ENTRY entry = CONTAINING_RECORD(current, LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);

        if (_wcsicmp(entry->BaseDllName.Buffer, wModuleName.c_str()) == 0)
            hModule = entry->DllBase;

        current = current->Flink;
    }

	if (!hModule)
		throw std::runtime_error("Module not found");
}

FARPROC DynamicLoader::GetProcAddress(const char* funcName) const
{
    FARPROC funcAddr = nullptr;

    // Get the base address of the module
    BYTE* baseAddr = (BYTE*)hModule;

    // Parse the Headers
    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)baseAddr;
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        throw std::runtime_error("Invalid DOS header signature");

    IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)(baseAddr + dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
        throw std::runtime_error("Invalid NT header signature");

    // Get Export Table
    IMAGE_DATA_DIRECTORY exportDir = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    IMAGE_EXPORT_DIRECTORY* exportTable = (IMAGE_EXPORT_DIRECTORY*)(baseAddr + exportDir.VirtualAddress);

    // Get function name and address tables
    DWORD* funcNames = (DWORD*)(baseAddr + exportTable->AddressOfNames);
    DWORD* funcAddrs = (DWORD*)(baseAddr + exportTable->AddressOfFunctions);
    WORD* funcOrdinals = (WORD*)(baseAddr + exportTable->AddressOfNameOrdinals);

    // Search for the function name
    for (DWORD i = 0; i < exportTable->NumberOfNames; i++) {
        const char* name = (const char*)(baseAddr + funcNames[i]);
        if (strcmp(name, funcName) == 0) {
            // Found the function, get its address
            DWORD funcRVA = funcAddrs[funcOrdinals[i]];
            funcAddr = (FARPROC)(baseAddr + funcRVA);
        }
    }

    if (funcAddr == nullptr)
        throw std::runtime_error("Function not found: " + std::string(funcName));

    return funcAddr;
}

PPEB DynamicLoader::GetPEB() const
{
#ifdef _M_X64 // x64
    return PPEB(__readgsqword(0x60));
#else // x86
    return PPEB(__readfsdword(0x30));
#endif
}
