#include "util/fun/fs/get_folder.hpp"

#include "util/fun/msg/mtrs_message.hpp"

#include <filesystem>

namespace stdfs = std::filesystem;

namespace mtrs::fs
{

std::unordered_set<std::string> get_files_from_folder(const std::string &dirPath, const std::string &extension)
{
    std::unordered_set<std::string> files;
    try
    {
        for (const auto& entry : stdfs::directory_iterator(dirPath))
        {
            stdfs::path p = entry.path();
            if (entry.is_directory())
            {
                files.merge(get_files_from_folder(p.generic_string(), extension));
            }
            else if (entry.is_regular_file() && p.extension() == extension)
            {
                files.emplace(p.generic_string());
            }
        }
    }
    catch (const stdfs::filesystem_error& e)
    {
#ifndef FLAG_RELEASE
        msg::mtrs_error(e.what());
#endif
    }
    return files;
}

}