#include "util/fun/math/hash.hpp"

#ifndef FLAG_RELEASE
#include <unordered_map>
static std::unordered_map<uint64_t, std::string> map64;
static std::unordered_map<uint32_t, std::string> map32;
#include "util/fun/msg/mtrs_message.hpp"
#endif

namespace mtrs::math
{

uint32_t hash32(const std::string& str, uint32_t seed)
{
#ifndef FLAG_RELEASE
    uint32_t hash = fnv1a<uint32_t>(str.data(), str.size(), seed, FNV_PRIME_32);
    detail::save_hash32_to_map(str, hash);
    return hash;
#elif
    return fnv1a<uint32_t>(str.data(), str.size(), seed, FNV_PRIME_32);
#endif
}

uint64_t hash64(const std::string& str, uint64_t seed)
{
#ifndef FLAG_RELEASE
    uint64_t hash = fnv1a<uint64_t>(str.data(), str.size(), seed, FNV_PRIME_64);
    detail::save_hash64_to_map(str, hash);
    return hash;
#elif
    return fnv1a<uint64_t>(str.data(), str.size(), seed, FNV_PRIME_64);
#endif
}

#ifndef FLAG_RELEASE

namespace detail
{

uint32_t save_hash32_to_map(std::string str, uint32_t hash)
{
    auto [it, is_emplace] = map32.try_emplace(hash, str);
    if(!is_emplace && it->second != str)
    {
        msg::mtrs_warning("Matching hash32 <",hash,"> found for strings \"", str,"\" and \"",it->second,'"');
    }
    return hash;
}

uint64_t save_hash64_to_map(std::string str, uint64_t hash)
{
    auto [it, is_emplace] = map64.try_emplace(hash, str);
    if(!is_emplace && it->second != str)
    {
        msg::mtrs_warning("Matching hash64 <",hash,"> found for strings \"", str,"\" and \"",it->second,'"');
    }
    return hash;
}

}

std::string rehash32(uint32_t hash)
{
    auto it = map32.find(hash);
    if(it == map32.end())
    {
        msg::mtrs_warning("map32 does not contain hash <", hash, '"');
        return "null";
    }
    return it->second;
}

std::string rehash64(uint64_t hash)
{
    auto it = map64.find(hash);
    if(it == map64.end())
    {
        msg::mtrs_warning("map64 does not contain hash <", hash, '"');
        return "null";
    }
    return it->second;
}

#endif

}