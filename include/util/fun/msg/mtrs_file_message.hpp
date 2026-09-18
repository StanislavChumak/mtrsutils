#ifndef MTRS_FILE_MESSAGE_HPP
#define MTRS_FILE_MESSAGE_HPP

#include "util/fun/msg/mtrs_message.hpp"

namespace mtrs::msg
{

template<typename T>
bool verification_message(std::string name, const T &due, const T &check)
{
    bool check_result = check == due;
    if(detail::_config.skip_error || check_result) return check_result;

    mtrs_message(TypeMessage::ERROR, "The value of \"", name, "\" actually corresponds to \"",
        check, "\", which does not match the proper \"", due, "\"");
    
    return false;
}

template<typename T>
void parameter_message(size_t offset, std::string name, T var, size_t size, size_t current_pos)
{
    if(detail::_config.detail_info)
    {
        mtrs_message(TypeMessage::INFO, '[', std::right, std::setw(4), (current_pos - size), '-',
            std::left, std::setw(4), current_pos, "] ", std::string(offset, ' '), name, '[', var, ']');
    }
    else
    {
        mtrs_message(TypeMessage::INFO, std::string(offset, ' '), name, '[', var, ']');
    }
}

template<typename T>
void variable_message(size_t offset, std::string name, T var)
{
    if(detail::_config.detail_info)
    {
        mtrs_message(TypeMessage::INFO, "[    -    ] ", std::string(offset, ' '), name, '[', var, ']');
    }
}

}

#endif
