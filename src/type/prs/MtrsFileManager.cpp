#include "util/type/prs/MtrsFileManager.hpp"

#include "util/type/prs/comp/comp_types.hpp"
#include "util/type/prs/res/res_types.hpp"

#include "util/type/prs/res/ScriptFile.hpp"
#include "util/type/prs/res/ShaderProgram.hpp"
#include "util/type/prs/res/Texture.hpp"
#include "util/type/prs/res/TextureAtlas.hpp"
#include "util/type/prs/res/Sound.hpp"
#include "util/type/prs/res/Font.hpp"

#include "util/type/prs/comp/ScriptUpdate.hpp"
#include "util/type/prs/comp/ScriptCallback.hpp"
#include "util/type/prs/comp/StoredData.hpp"
#include "util/type/prs/comp/Transform.hpp"
#include "util/type/prs/comp/Sprite.hpp"
#include "util/type/prs/comp/Animator.hpp"
#include "util/type/prs/comp/StateAnimator.hpp"
#include "util/type/prs/comp/SpriteMap.hpp"
#include "util/type/prs/comp/MapAnimator.hpp"
#include "util/type/prs/comp/SoundPlayer.hpp"
#include "util/type/prs/comp/Label.hpp"

#include "util/fun/math/hash.hpp"
#include "util/fun/prs/read.hpp"

#include <fstream>
#include <algorithm>

namespace mtrs::prs
{

MtrsFileManager::MtrsFileManager(std::unordered_set<std::string> paths, const uint64_t limit_size_cache)
: _limit_size_cache(limit_size_cache * 1024), _current_size_cache(0)
{
    for(auto &path : paths)
    {
        _files.emplace(path, create_file_to_path(path));
        _file_iterators.push_back(_files.find(path));
    }
}

MtrsFileManager::MtrsFileManager(MtrsFileManager &&other)
{
    _limit_size_cache = other._limit_size_cache;
    other._limit_size_cache = 0;
    _current_size_cache = other._current_size_cache;
    other._current_size_cache = 0;
    _files = std::move(other._files);
    _file_iterators = std::move(other._file_iterators);
}

MtrsFileManager &MtrsFileManager::operator=(MtrsFileManager &&other)
{
    if(this != &other)
    {
        _limit_size_cache = other._limit_size_cache;
        other._limit_size_cache = 0;
        _current_size_cache = other._current_size_cache;
        other._current_size_cache = 0;
        _files = std::move(other._files);
        _file_iterators = std::move(other._file_iterators);
    }
    return *this;
}

MtrsFileManager::~MtrsFileManager()
{
    _file_iterators.clear();
    _files.clear();
}

DeferredData field_to_ddata(uint64_t *field_ptr, char *data)
{
    uint32_t offset = static_cast<uint32_t>(*field_ptr);
    uint32_t size = static_cast<uint32_t>(*field_ptr >> 32);
    std::string_view view(data + offset, size);
    return DeferredData(view, *field_ptr);
}

MtrsFileManager::MtrsFile MtrsFileManager::create_file_to_path(const std::string &path)
{
    uint64_t data_size, chach_lifetime;
    std::ifstream stream(path, std::ios::binary | std::ios::ate);
    if(!stream.is_open())
    {
#ifndef FLAG_RELEASE
        msg::mtrs_error("Could not open file at path \"", path, '\"');
#endif
        return MtrsFile{0, 0};
    };

    data_size = stream.tellg();
    stream.seekg(0);

#ifndef FLAG_RELEASE
    float perc = static_cast<float>(data_size) / _limit_size_cache;
    if(perc > 0.15f)
    {
        msg::mtrs_warning("One file on the \"", path,"\" path takes up about ", 
            static_cast<int>(perc * 100), "\% of the memory available to the file manager,",
            " the limit of which is ",_limit_size_cache / 1024," KB");
    }
#endif

    char data[16];
    stream.read(data, 16);
    stream.close();

    if(stream.gcount() < 16)
    {
#ifndef FLAG_RELEASE
        msg::mtrs_error("The file at path \"", path, "\" is too short to be an MTRS file.");
#endif
        return MtrsFile{0, 0};
    }

#ifndef FLAG_RELEASE
    std::string_view magic(data, 7);
    if(magic != "mtrsscn" && magic != "mtrspck")
    {
        msg::mtrs_error("The file at path \"", path, "\" is not an MTRS file.");
    }
#endif

    std::memcpy(&chach_lifetime, data + 8, sizeof(uint64_t));

    return MtrsFile{data_size, chach_lifetime};
}

void MtrsFileManager::read_file(const std::string &path, MtrsFileManager::MtrsFile &file)
{
    std::ifstream stream(path, std::ios::binary);
    if(!stream.is_open()) return;
    
    file.data.resize(file.data_size);
    char *data = file.data.data();
    stream.read(data, file.data_size);
    stream.close();

    std::string_view magic(data, 7);

    if(magic == "mtrsscn")
    {
        size_t cur = 16;

        uint32_t entity_count, data_offset, ddata_offset;
        read(entity_count, data, cur);
        read(data_offset, data, cur);
        read(ddata_offset, data, cur);

        cur = data_offset;
        uint64_t entity_id, entity_end;
        while(cur < ddata_offset && cur < file.data.size())
        {
            read(entity_id, data, cur);
            read(entity_end, data, cur);

            while(cur < entity_end && cur < ddata_offset)
            {
                uint64_t component_id;
                read(component_id, data, cur);
                switch (component_id)
                {
#define X(Comp) case math::hash64_(#Comp): {\
    size_t comp_pos = cur;\
    Comp comp; std::memcpy(&comp, data + comp_pos, sizeof(comp)); cur = comp_pos + sizeof(comp);\
    for(auto field : comp.get_deferred_fields())\
    { uint64_t *field_ptr = reinterpret_cast<uint64_t*>(data + comp_pos \
        + (reinterpret_cast<char*>(field) - reinterpret_cast<char*>(&comp))); \
    file.deferred_data.emplace(*field_ptr, field_to_ddata(field_ptr, data)); } break;}
                COMPONENT_TYPES
#undef X
                }
            }
            cur = entity_end;
        }
    }
    else if(magic == "mtrspck")
    {
        size_t cur = 8;
        uint64_t group_id, group_end, res_id;
        while(cur < file.data.size())
        {
            read(group_id, data, cur);
            read(group_end, data, cur);

            while(cur < group_end && cur < file.data.size())
            {
                read(res_id, data, cur);
                switch (group_id)
                {
#define X(Res) case math::hash64_(#Res): {\
    size_t res_pos = cur;\
    Res res; std::memcpy(&res, data + res_pos, sizeof(res)); cur = res_pos + sizeof(res);\
    for(auto field : res.get_deferred_fields())\
    { uint64_t *field_ptr = reinterpret_cast<uint64_t*>(data + res_pos \
        + (reinterpret_cast<char*>(field) - reinterpret_cast<char*>(&res))); \
    file.deferred_data.emplace(*field_ptr, field_to_ddata(field_ptr, data)); } break;}
                RESOURCE_TYPES
#undef X
                }
            }
            cur = group_end;
        }
    }

    file.current_lifetime = file.chach_lifetime;
}

void MtrsFileManager::clear_file(const std::string &path, MtrsFileManager::MtrsFile &file)
{
    if(file.dirty && !file.data.empty())
    {
        std::ofstream stream(path, std::ios::binary | std::ios::trunc);
        if(!stream.is_open())
        {
#ifndef FLAG_RELEASE
            msg::mtrs_error("Could not open file at path \"", path, "\" while overwriting");
#endif
        }
        else
        {
            char *data = file.data.data();

            uint32_t base = 0xFFFFFFFFu;
            for(auto &ddata : file.deferred_data)
            {
                if(ddata.second.field && ddata.second.field[0] < base)
                {
                    base = ddata.second.field[0];
                }
            }

            std::string_view magic(data, 7);
            if(magic == "mtrsscn")
            {
                std::memcpy(&base, data + 24, sizeof(uint32_t));
            }

            size_t ddata_size = 0;
            for(auto &ddata : file.deferred_data)
            {
                ddata_size += ddata.second.field[1];
            }

            std::vector<char> out(static_cast<size_t>(base) + ddata_size);
            std::memcpy(out.data(), data, base);

            uint32_t offset = base;
            for(auto &ddata : file.deferred_data)
            {
                if(!ddata.second.field) continue;

                size_t field_pos = reinterpret_cast<char*>(ddata.second.field) - data;

                uint64_t packed =
                    (static_cast<uint64_t>(ddata.second.field[1]) << 32) | offset;
                std::memcpy(out.data() + field_pos, &packed, sizeof(uint64_t));

                std::memcpy(out.data() + offset, ddata.second.data, ddata.second.field[1]);
                offset += ddata.second.field[1];
            }

            file.data = std::move(out);
            file.data_size = file.data.size();

            stream.write(file.data.data(), file.data.size());
        }
        stream.close();
        file.dirty = false;
    }
    std::vector<char>().swap(file.data);
    file.deferred_data.clear();
    file.current_lifetime = 0;
}

MtrsFileManager::MtrsFile *MtrsFileManager::get_file(const std::string &path)
{
    auto iter = _files.find(path);
    if(iter == _files.end())
    {
#ifndef FLAG_RELEASE
        msg::mtrs_error("Could not find file with path \"", path,"\" in File Manager");
#endif
        return nullptr;
    }

    if(iter->second.current_lifetime <= 0)
    {
        uint64_t size_cache = _current_size_cache + iter->second.data_size;
        if(size_cache > _limit_size_cache)
        {
            std::sort(_file_iterators.begin(), _file_iterators.end(),
                [](const decltype(_files)::iterator &a, const decltype(_files)::iterator &b)
                { return a->second.current_lifetime < b->second.current_lifetime; });

            for(auto iter = _file_iterators.begin(); iter != _file_iterators.end(); iter++)
            {
                if((*iter)->second.current_lifetime <= 0) continue;

                clear_file((*iter)->first, (*iter)->second);

                size_cache -= (*iter)->second.data_size;

                if(size_cache <= _limit_size_cache)
                {
                    break;
                }
            }
        }
        _current_size_cache = size_cache;
        read_file(path, iter->second);
    }

    return &iter->second;
}

void MtrsFileManager::update(const double &delta)
{
    for(auto &iter : _file_iterators)
    {
        if(iter->second.current_lifetime <= 0)
        {
            continue;
        }
        else if(iter->second.current_lifetime <= delta)
        {
            clear_file(iter->first, iter->second);

            _current_size_cache -= iter->second.data_size;
        }
        else
        {
            iter->second.current_lifetime -= delta;
        }
    }
}

void MtrsFileManager::clear()
{
    for(auto &iter : _file_iterators)
    {
        clear_file(iter->first, iter->second);
    }
}

}