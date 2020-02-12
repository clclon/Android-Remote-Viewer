
#pragma once

namespace Helper
{
    template <typename T>
    static inline void auto_free(T *v) { delete v; }
    //
    template <typename T>
    std::shared_ptr<T> auto_shared(T *t)
    {
        return std::shared_ptr<T>(t, [](T *t){ auto_free(t); });
    }
    template <typename T>
    std::shared_ptr<T> auto_shared(T *t, std::function<void(T*)> f)
    {
        return std::shared_ptr<T>(t, f);
    }
};
