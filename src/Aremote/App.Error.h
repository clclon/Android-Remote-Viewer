
#pragma once

namespace App
{
    enum ErrorId
    {
        error_begin = 0,
#       define __ERRITEM(A,B) error_ ## A,
#       include "App.Error.Items.h"
        error_unknown,
        error_end
    };
    class ErrorCat : public std::error_category
    {
        public:
            //
            const char* name() const noexcept override;
            std::string message(int32_t ev) const override;
    };
    const ErrorCat errCat{};
    //
    std::error_code make_error_code(ErrorId);
    //
    std::string geterror(int32_t);
};

namespace std
{
    //
    template <>
    struct is_error_code_enum<App::ErrorId> : public true_type{};
};
