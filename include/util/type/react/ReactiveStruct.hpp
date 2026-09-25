#ifndef REACTIVE_STRUCT_HPP
#define REACTIVE_STRUCT_HPP

#include "ReactiveValue.hpp"

namespace mtrs::react
{

template <typename T, size_t MaxObservers = 8, typename ...Deps>
class ReactiveStruct : public ReactiveValue<T, MaxObservers, Deps...>
{
public:
    ReactiveStruct(const ReactiveStruct&) = delete;
    ReactiveStruct &operator=(const ReactiveStruct&) = delete;
    ReactiveStruct(ReactiveStruct &&) = delete;
    ReactiveStruct &operator=(ReactiveStruct &&) = delete;
 
    ReactiveStruct()
    : ReactiveValue<T, MaxObservers, Deps...>()
    {
    }
 
    explicit ReactiveStruct(T initial)
    : ReactiveValue<T, MaxObservers, Deps...>(std::move(initial))
    {
    }
 
    ReactiveStruct(T(*calculator)(const Deps* ...), PushNode<Deps>... deps)
    : ReactiveValue<T, MaxObservers, Deps...>(calculator, std::move(deps) ...)
    {
    }

    template <typename Field, typename Arg>
    void set_field(Field T::*member, Arg value)
    {
        this->_value.*member = std::move(value);
        this->_dirty = true;
        this->notify_observers();
    }

    template <typename Field>
    Field &change_field(Field T::*member)
    {
        this->_dirty = true;
        this->notify_observers();
        return this->_value.*member;
    }

    template <typename Field>
    const Field& get_field(Field T::*member) const
    {
        return this->_value.*member;
    }

    REACTIVE_OPERATORS(ReactiveStruct)
};

}

#endif
