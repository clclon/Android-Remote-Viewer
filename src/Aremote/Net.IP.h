
#pragma once

namespace Net
{
    class IP
    {
        public:
            //
            IP();
            virtual ~IP();
            //
            std::string get();
            std::string get(std::string const &);

        private:
            //
            std::string m_ipa;
            //
            bool init();
    };
};
