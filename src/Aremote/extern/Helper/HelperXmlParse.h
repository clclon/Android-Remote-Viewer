
#pragma once

#include "../xml/tixml2ex.h"

namespace Helper
{
    class XmlParse
    {
        public:
            //
            enum XmlInputType
            {
                XIT_STRING,
                XIT_FILE
            };
            //
            using xattr_t = std::vector<std::string>;
            using xfun_t  = std::function<void(std::vector<std::string> const&)>;

        private:
            //
            std::unique_ptr<tinyxml2::XMLDocument> m_doc;
            //
            void parse_xml_elemet_(
                    std::string const&,
                    xattr_t const&,
                    xfun_t);

        public:
            //
            XmlParse(std::string const&, XmlInputType = XmlInputType::XIT_STRING);
            //
            void parse(
                    std::string const&,
                    xattr_t const&,
                    xfun_t);
    };
};
