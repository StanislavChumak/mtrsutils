#ifndef MTRS_MESSAGE_HPP
#define MTRS_MESSAGE_HPP

#include <sstream>

#define TYPE_MESSAGE \
X(ERROR) \
X(WARNING) \
X(INFO)

namespace mtrs::msg
{

enum class TypeMessage
{
#define X(TMSG) TMSG,
    TYPE_MESSAGE
    UNKNOWN
#undef X
};

enum class FlagMessage
{
    SKIP_ERROR,
    SKIP_WARNING,
    PRINT_INFO,
    DETAIL_INFO
};

namespace detail
{
    struct MessegeConfig
    {
        bool skip_error = false;
        bool skip_warning = false;
        bool print_info = false;
        bool detail_info = false;
    };
    inline MessegeConfig _config;
    void show_message(TypeMessage tmsg, std::string&& message);
}

void flag_message(FlagMessage flag);

template<typename... Args>
void mtrs_message(TypeMessage tmsg, Args&&... args)
{
    std::stringstream ss;
    ((ss << std::forward<Args>(args)), ...) << std::endl;

    detail::show_message(tmsg, ss.str());
}

template<typename... Args>
void mtrs_info(Args&&... args)
{
    mtrs_message(TypeMessage::INFO, args...);
}

template<typename... Args>
void mtrs_warning(Args&&... args)
{
    mtrs_message(TypeMessage::WARNING, args...);
}

template<typename... Args>
void mtrs_error(Args&&... args)
{
    mtrs_message(TypeMessage::ERROR, args...);
}

}

#endif
