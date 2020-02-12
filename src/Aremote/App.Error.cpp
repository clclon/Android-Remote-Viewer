
#include <string>
#include <system_error>
#include "App.Error.h"

namespace App
{
    static inline const char *l_app_error[] =
    {
        "",              // ErrorId::error_begin
#       define __ERRITEM(A,B) B,
#       include "App.Error.Items.h"
        "unknown error", // ErrorId::error_unknown
        ""               // ErrorId::error_end
    };
    //
    std::string geterror(int32_t e)
    {
        if ((e > ErrorId::error_begin) && (e < ErrorId::error_end))
            return l_app_error[e];
        return l_app_error[ErrorId::error_unknown];
    }

    const char* ErrorCat::name() const noexcept
    {
        return "ARemote Error";
    }

    std::string ErrorCat::message(int32_t e) const
    {
        return geterror(e);
    }

    /*
    bool ErrorCat::equivalent(const std::error_code & ec, int32_t cond) const noexcept
    {
        // switch (static_cast<ErrorId>(cond))
        return false;
    }
    */

    ///

    template <class T>
    T make_error_type(ErrorId e)
    {
        return { static_cast<int32_t>(e), errCat };
    }

    std::error_code make_error_code(ErrorId e)
    {
        return make_error_type<std::error_code>(e);
    }

    /*
    std::error_condition make_error_condition(ErrorId e)
    {
        return make_error_type<std::error_condition>(e);
    }
    */

};
