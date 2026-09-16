#ifndef LOAD_EXTERNAL_LIB_HPP
#define LOAD_EXTERNAL_LIB_HPP

#if defined(_WIN32)
    #include <windows.h>
    #define EXPORT __declspec(dllexport)
#else
    #include <dlfcn.h>
    #define EXPORT __attribute__((visibility("default")))
#endif

namespace mtrs::fs
{

#if defined(_WIN32)
    using LibHandle = HMODULE;
#else
    using LibHandle = void*;
#endif

LibHandle load_library(const char *path);
void free_library(LibHandle handle);
void* get_symbol(LibHandle handle, const char *name);
const char* get_last_error();

constexpr const char* lib_extension()
{
#if defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

}

#endif
