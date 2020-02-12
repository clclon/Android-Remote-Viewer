
#pragma once

#include <sstream>

namespace Lua
{
    enum ErrorId
    {
        error_lua_begin = 0,
#       define __ERRITEM(A,B) error_lua_ ## A,
#       include "Lua.Error.Items.h"
        error_lua_unknown,
        error_lua_end
    };
    class ErrorCat : public std::error_category
    {
        public:
            //
            const char* name() const noexcept override;
            std::string message(int32_t) const override;
            std::string_view message(Lua::ErrorId) const;
    };
    const ErrorCat errCat{};
    //
    std::error_code make_error_code(Lua::ErrorId);
    //
    template <typename ... Args>
    std::system_error make_system_error(Lua::ErrorId eid, Args && ... args)
    {
        std::stringstream ss;
        ((ss << ' ' << std::forward<Args>(args)), ...);
        return std::system_error(Lua::make_error_code(eid), ss.str());
    //
    }
    template <typename ... Args>
    std::system_error make_system_error(std::error_code const & ec, Args && ... args)
    {
        std::stringstream ss;
        ((ss << ' ' << std::forward<Args>(args)), ...);
        return std::system_error(ec, ss.str());
    //
    }
    //
    std::string_view geterror(int32_t);
}

namespace std
{
    //
    template <>
    struct is_error_code_enum<Lua::ErrorId> : public true_type{};
}
