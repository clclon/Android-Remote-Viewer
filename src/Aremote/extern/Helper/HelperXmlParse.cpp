
#if defined(_BUILD_SINGLE)
#  include <iostream>
#  include <string>
#  include <vector>
#  include <functional>
#  include "HelperXmlParse.h"
#else
#  include "../../ARemote.h"
#endif

namespace Helper
{
    XmlParse::XmlParse(std::string const & s, XmlParse::XmlInputType t)
    {
        switch (t)
        {
            case XmlParse::XmlInputType::XIT_STRING:
                {
                    m_doc = tinyxml2::load_document(s);
                    break;
                }
            case XmlParse::XmlInputType::XIT_FILE:
                {
                    m_doc = tinyxml2::load_xmlfile(s);
                    break;
                }
            default:
                throw std::runtime_error("bad type");
        }
    }

    void XmlParse::parse_xml_elemet_(
            std::string const & section,
            xattr_t const & attribute,
            xfun_t  f)
    {
        try
        {
            if (section.empty())
                throw std::runtime_error("section empty!");

            for (auto item : tinyxml2::selection(m_doc.get(), section))
            {
                std::vector<std::string> v;
                for (auto & attr : attribute)
                    v.push_back(
                        attribute_value(item, attr)
                    );
                f(v);
            }
        }
        catch (tinyxml2::XmlException const & _ex)
        {
            LOGEX(_ex.what());
        }
        catch (std::runtime_error const & _ex)
        {
            LOGEX(_ex.what());
        }
    }

    void XmlParse::parse(
            std::string const & section,
            xattr_t const & attribute,
            xfun_t  f)
    {
        XmlParse::parse_xml_elemet_(
                section,
                attribute,
                f);
    }
};
