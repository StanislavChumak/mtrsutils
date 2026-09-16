#ifndef HASH_HPP
#define HASH_HPP

#include <cstdint>
#include <cstddef>

#include <string>

constexpr uint32_t FNV_OFFSET_BASIS_32 = 2166136261u;
constexpr uint64_t FNV_OFFSET_BASIS_64 = 14695981039346656037ull;
constexpr uint32_t FNV_PRIME_32        = 16777619u;
constexpr uint64_t FNV_PRIME_64        = 1099511628211ull;

namespace mtrs::math
{

#ifndef FLAG_RELEASE

namespace detail
{
    uint64_t save_hash64_to_map(std::string str, uint64_t hash);
    uint32_t save_hash32_to_map(std::string str, uint32_t hash);
}

std::string rehash64(uint64_t hash);
std::string rehash32(uint32_t hash);

#define HASH_SPECIFER inline
#elif
#define HASH_SPECIFER constexpr
#endif

template<typename T>
constexpr T fnv1a(const char *data, size_t size, T seed, T prime)
{
    for (size_t i = 0; i < size; i++)
    {
        seed ^= data[i];
        seed *= prime;
    }
    return seed;
}

constexpr uint32_t hash32_(const char *str, uint32_t seed = FNV_OFFSET_BASIS_32)
{
    int i = 0;
    while (str[i]) i++;
    return fnv1a<uint32_t>(str, i, seed, FNV_PRIME_32);
}

constexpr uint64_t hash64_(const char *str, uint64_t seed = FNV_OFFSET_BASIS_64)
{
    int i = 0;
    while (str[i]) i++;
    return fnv1a<uint64_t>(str, i, seed, FNV_PRIME_64);
}

HASH_SPECIFER uint32_t hash32(const void *data, size_t size, uint32_t seed = FNV_OFFSET_BASIS_32)
{
#ifndef FLAG_RELEASE
    uint32_t hash = fnv1a<uint32_t>(static_cast<const char*>(data), size, seed, FNV_PRIME_32);
    return detail::save_hash32_to_map("data with size:" + std::to_string(size), hash);
#elif
    return fnv1a<uint32_t>(static_cast<const char*>(data), size, seed, FNV_PRIME_32);
#endif
}

HASH_SPECIFER uint64_t hash64(const void *data, size_t size, uint64_t seed = FNV_OFFSET_BASIS_64)
{
#ifndef FLAG_RELEASE
    uint32_t hash = fnv1a<uint32_t>(static_cast<const char*>(data), size, seed, FNV_PRIME_32);
    return detail::save_hash32_to_map("data with size:" + std::to_string(size), hash);
#elif
    return fnv1a<uint32_t>(static_cast<const char*>(data), size, seed, FNV_PRIME_32);
#endif
}

HASH_SPECIFER uint32_t hash32(const char *str, uint32_t seed = FNV_OFFSET_BASIS_32)
{
    int i = 0;
    while (str[i]) i++;
#ifndef FLAG_RELEASE
    uint32_t hash = fnv1a<uint32_t>(str, i, seed, FNV_PRIME_32);
    return detail::save_hash32_to_map(str, hash);
#elif
    return fnv1a<uint32_t>(str, i, seed, FNV_PRIME_32);
#endif
}

HASH_SPECIFER uint64_t hash64(const char *str, uint64_t seed = FNV_OFFSET_BASIS_64)
{
    int i = 0;
    while (str[i]) i++;
#ifndef FLAG_RELEASE
    uint64_t hash = fnv1a<uint64_t>(str, i, seed, FNV_PRIME_64);
    return detail::save_hash64_to_map(str, hash);
#elif
    return fnv1a<uint64_t>(str, i, seed, FNV_PRIME_64);
#endif
}

uint32_t hash32(const std::string& str, uint32_t seed = FNV_OFFSET_BASIS_32);
uint64_t hash64(const std::string& str, uint64_t seed = FNV_OFFSET_BASIS_64);

}

#endif
