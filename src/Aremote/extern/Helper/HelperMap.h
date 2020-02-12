
#pragma once

namespace Helper
{
	template <typename T, typename U>
	class Map
	{
		private:
			std::map<T, U> map_;

		public:
			Map(const T & key, const U & val)
			{
				map_[key] = val;
			}
			Map<T, U> & operator()(const T & key, const U & val)
			{
				map_[key] = val;
				return *this;
			}
			operator std::map<T, U>()
			{
				return map_;
			}
	};
	typedef std::map<std::string, std::string> StringMap;
	typedef Map<std::string, std::string> StringMapInit;
	//
#   if defined(__WIN32__)
	typedef std::map<uint32_t, std::tuple<uint32_t, HBITMAP>> ImageMap;
	typedef Map<uint32_t, std::tuple<uint32_t, HBITMAP>> ImageMapInit;
#   endif
	//
	typedef std::map<uint16_t, std::string> CatalogMap;
	typedef Map<uint16_t, std::string> CatalogMapInit;
}
