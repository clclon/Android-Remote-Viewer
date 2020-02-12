
#pragma once

#include <unordered_map>
#include <unordered_set>

namespace Helper
{
	class Xml final
	{
		private:
			//
			enum XmlStringId
			{
				SID_ROOT_BEGIN = 0,
				SID_ROOT_END,
				SID_SEC_BEGIN,
				SID_SEC_END,
				SID_OPT_NAME,
				SID_OPT_VALUE,
				SID_OPT_DESC,
				SID_OPT_END,
				SID_END
			};
			static inline const char head_[] =
				"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\n";
			static inline const char *opt_[] =
			{
				[SID_ROOT_BEGIN] = "<",
				[SID_ROOT_END]   = "</",
				[SID_SEC_BEGIN]  = "\t<",
				[SID_SEC_END]    = "\t</",
			    [SID_OPT_NAME]   = "\t\t<Option name=\"",
			    [SID_OPT_VALUE]  = "\" value=\"",
			    [SID_OPT_DESC]   = "\" desc=\"",
			    [SID_OPT_END]    = "\" />\n",
                [SID_END]        = ">\n"
			};
			//
			std::stringstream ss_;
			std::string       root_;
			std::string       section_;
			//
			template <typename T1, typename T2>
			void write_opt(T1 const & arg, T2 const & val)
			{
				ss_ << opt_[SID_OPT_NAME]
					<< arg
					<< opt_[SID_OPT_VALUE]
					<< val
					<< opt_[SID_OPT_END];
			}
			template <typename T1, typename T2, typename T3>
			void write_opt(T1 const & arg, T2 const & val, T3 const & desc)
			{
				ss_ << opt_[SID_OPT_NAME]
                    << arg
                    << opt_[SID_OPT_VALUE]
                    << val
                    << opt_[SID_OPT_DESC]
                    << desc
                    << opt_[SID_OPT_END];
			}
			template <typename T>
			void write_tag(std::string const & s, T const & arg)
			{
				ss_ << s.c_str()
                    << arg
                    << opt_[SID_END];
			}
			template <class T>
			void map_foreach(T const & m)
			{
				for (auto & [ id, val ] : m)
					write_opt(id, val);
			}
			void end_section()
			{
				if (section_.empty())
					return;

				write_tag(opt_[SID_SEC_END], section_);
				section_ = "";
			}

		public:
			//
			virtual ~Xml() = default;
			Xml(std::string const & arg)
			{
				root_ = arg;
				ss_ << head_;
				write_tag(opt_[SID_ROOT_BEGIN], arg);
			}
			operator std::string()
			{
				end_section();
				write_tag(opt_[SID_ROOT_END], root_);
				return ss_.str();
			}
			Xml & xmlsection(std::string const & arg)
			{
				end_section();
				write_tag(opt_[SID_SEC_BEGIN], arg);
				section_ = arg;
				return *this;
			}
			template <typename T1, typename T2, typename T3>
			Xml & xmlpair(T1 const & arg, T2 const & val, T3 const & desc)
			{
				write_opt(arg, val, desc);
				return *this;
			}
			template <typename T1, typename T2>
			Xml & xmlpair(T1 const & arg, T2 const & val)
			{
				write_opt(arg, val);
				return *this;
			}
			template <typename T1, typename T2>
			Xml & xmlarray(T1 const & arg, std::vector<T2> const & m)
			{
				for (auto & val : m)
					write_opt(arg, val);
				return *this;
			}
			template <typename T1, typename T2>
			Xml & xmlarrayn(T1 const & arg, std::vector<T2> const & m)
			{
				uint32_t cnt = 0U;
				for (auto & val : m)
				{
					std::stringstream ss;
					ss << arg << ++cnt;
					write_opt(ss.str(), val);
				}
				return *this;
			}
			template <typename T1, typename T2>
			Xml & xmlmap(std::map<T1, T2> const & m)
			{
				map_foreach<std::map<T1, T2>>(m);
				return *this;
			}
			template <typename T1, typename T2>
			Xml & xmlmap(std::unordered_map<T1, T2> const & m)
			{
				map_foreach<std::unordered_map<T1, T2>>(m);
				return *this;
			}
			template <typename T1, typename T2, typename T3, typename T4>
			Xml & xmlmap(std::unordered_multimap<T1, T2, T3, T4> const & m)
			{
				map_foreach<std::unordered_multimap<T1, T2, T3, T4>>(m);
				return *this;
			}
	};
};
