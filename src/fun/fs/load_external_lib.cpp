#include "util/fun/fs/load_external_lib.hpp"

#include <cstdio>

namespace mtrs::fs
{

LibHandle load_library(const char *path)
{
#if defined(_WIN32)
    return LoadLibraryA(path);
#else
    return dlopen(path, RTLD_NOW | RTLD_LOCAL);
#endif
}

void free_library(LibHandle handle)
{
    if (!handle) return;
#if defined(_WIN32)
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}

void* get_symbol(LibHandle handle, const char *name)
{
#if defined(_WIN32)
    return reinterpret_cast<void*>(GetProcAddress(handle, name));
#else
    return dlsym(handle, name);
#endif
}

const char* get_last_error()
{
#if defined(_WIN32)
    static char buf[256];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(),
                   0, buf, sizeof(buf), nullptr);
    return buf;
#else
    return dlerror();
#endif
}

}