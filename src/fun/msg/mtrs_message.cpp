#include "util/fun/msg/mtrs_message.hpp"
#include <iostream>

namespace mtrs::msg
{

void flag_message(FlagMessage flag)
{
    switch (flag)
    {
    case FlagMessage::SKIP_ERROR:
        detail::_config.skip_error = true;
        break;
    case FlagMessage::SKIP_WARNING:
        detail::_config.skip_warning = true;
        break;
    case FlagMessage::PRINT_INFO:
        detail::_config.print_info = true;
        break;
    case FlagMessage::DETAIL_INFO:
        detail::_config.detail_info = true;
        break;
    }
}

void detail::show_message(TypeMessage tmsg, std::string&& message)
{
    switch (tmsg)
    {
    case TypeMessage::ERROR:
        if(detail::_config.skip_error) return;
        std::cerr << "ERROR:   ";
        if(detail::_config.detail_info) std::cerr << "[ !! - !! ] ";
        std::cerr << message;
        break;
    case TypeMessage::WARNING:
        if(detail::_config.skip_warning) return;
        std::cout << "WARNING: ";
        if(detail::_config.detail_info) std::cout << "[ ?? - ?? ] ";
        std::cout << message;
        break;
    case TypeMessage::INFO:
        if(!detail::_config.print_info) return;
        std::cout << "INFO:    " << message;
        break;
    default:
        if(detail::_config.skip_error) return;
        std::cerr << "UNKNOWN: ";
        if(detail::_config.detail_info) std::cerr << "[ ?! - ?! ] ";
        std::cerr << message;
        break;
    }
}

}