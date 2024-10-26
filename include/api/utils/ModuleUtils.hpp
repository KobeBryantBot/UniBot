#pragma once
#include <Windows.h>
#include <iostream>
#include <optional>

namespace utils {

[[nodiscard]] inline HMODULE getModuleHandle() {
    HMODULE hModule = NULL;
    if (GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)getModuleHandle,
            &hModule
        )
        == FALSE) {
        return NULL;
    }
    return hModule;
}

[[nodiscard]] inline std::optional<std::string> readResource(HMODULE hModule, int id, bool isBinary = false) {
    if (HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(id), isBinary ? L"BINFILE" : L"TEXTFILE")) {
        if (HGLOBAL resource = LoadResource(hModule, hRes)) {
            void*       data = LockResource(resource);
            DWORD       size = SizeofResource(hModule, hRes);
            std::string outData(static_cast<const char*>(data), size);
            return std::move(outData);
        }
    }
    return {};
}

} // namespace utils