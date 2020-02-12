
#include "../../ARemote.h"

namespace Lua
{
    static inline const char *l_lua_error[] =
    {
        "",              // ErrorId::error_begin
#       define __ERRITEM(A,B) B,
#       include "Lua.Error.Items.h"
        "unknown error", // ErrorId::error_unknown
        ""               // ErrorId::error_end
    };
    //
    std::string_view geterror(int32_t e)
    {
        if ((e > Lua::ErrorId::error_lua_begin) && (e < Lua::ErrorId::error_lua_end))
            return l_lua_error[e];
        return l_lua_error[Lua::ErrorId::error_lua_unknown];
    }

    const char* ErrorCat::name() const noexcept
    {
        return "Lua engine";
    }

    std::string_view ErrorCat::message(Lua::ErrorId e) const
    {
        return geterror(static_cast<int32_t>(e));
    }

    std::string ErrorCat::message(int32_t e) const
    {
        return std::string(geterror(e));
    }

    std::error_code make_error_code(Lua::ErrorId e)
    {
        return { static_cast<int32_t>(e), errCat };
    }
}
