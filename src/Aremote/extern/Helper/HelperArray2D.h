
#pragma once

namespace Helper
{
	template <typename T, uint32_t N, uint32_t M>
	class Array2D
	{
	    public:
        	using arr2d = T(*)[M];

	    private:
	        arr2d arr = nullptr;

	    public:
	        Array2D()  { arr = new T[N][M](); }
	        ~Array2D() { delete [] arr; }
	        arr2d get() const { return arr; }
	};
};
