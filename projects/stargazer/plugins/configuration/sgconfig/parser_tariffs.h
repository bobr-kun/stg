/*
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 *    Author : Boris Mikhailenko <stg34@stargazer.dp.ua>
 *    Author : Maksym Mamontov <stg@madf.info>
 */

#ifndef __STG_SGCONFIG_PARSER_TARIFFS_H__
#define __STG_SGCONFIG_PARSER_TARIFFS_H__

#include "parser.h"

#include "stg/tariff_conf.h"
#include "stg/common.h"

#include <string>

class TARIFFS;
class USERS;
class ADMIN;

namespace STG
{
namespace PARSER
{

class GET_TARIFFS: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                explicit FACTORY(const TARIFFS & tariffs) : m_tariffs(tariffs) {}
                virtual BASE_PARSER * create(const ADMIN & admin) { return new GET_TARIFFS(admin, m_tariffs); }
                static void Register(REGISTRY & registry, const TARIFFS & tariffs)
                { registry[ToLower(tag)] = new FACTORY(tariffs); }
            private:
                const TARIFFS & m_tariffs;
        };

        static const char * tag;

        GET_TARIFFS(const ADMIN & admin, const TARIFFS & tariffs)
            : BASE_PARSER(admin, tag), m_tariffs(tariffs) {}

    private:
        const TARIFFS & m_tariffs;

        void CreateAnswer();
};

class ADD_TARIFF: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                explicit FACTORY(TARIFFS & tariffs) : m_tariffs(tariffs) {}
                virtual BASE_PARSER * create(const ADMIN & admin) { return new ADD_TARIFF(admin, m_tariffs); }
                static void Register(REGISTRY & registry, TARIFFS & tariffs)
                { registry[ToLower(tag)] = new FACTORY(tariffs); }
            private:
                TARIFFS & m_tariffs;
        };

        static const char * tag;

        ADD_TARIFF(const ADMIN & admin, TARIFFS & tariffs)
            : BASE_PARSER(admin, tag), m_tariffs(tariffs) {}
        int Start(void * data, const char * el, const char ** attr);

    private:
        std::string tariff;
        TARIFFS & m_tariffs;

        void CreateAnswer();
};

class DEL_TARIFF: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(TARIFFS & tariffs, const USERS & users) : m_tariffs(tariffs), m_users(users) {}
                virtual BASE_PARSER * create(const ADMIN & admin) { return new DEL_TARIFF(admin, m_users, m_tariffs); }
                static void Register(REGISTRY & registry, TARIFFS & tariffs, const USERS & users)
                { registry[ToLower(tag)] = new FACTORY(tariffs, users); }
            private:
                TARIFFS & m_tariffs;
                const USERS & m_users;
        };

        static const char * tag;

        DEL_TARIFF(const ADMIN & admin, const USERS & users, TARIFFS & tariffs)
            : BASE_PARSER(admin, tag), m_users(users), m_tariffs(tariffs) {}
        int Start(void * data, const char * el, const char ** attr);

    private:
        std::string tariff;
        const USERS & m_users;
        TARIFFS & m_tariffs;

        void CreateAnswer();
};

class CHG_TARIFF: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                explicit FACTORY(TARIFFS & tariffs) : m_tariffs(tariffs) {}
                virtual BASE_PARSER * create(const ADMIN & admin) { return new CHG_TARIFF(admin, m_tariffs); }
                static void Register(REGISTRY & registry, TARIFFS & tariffs)
                { registry[ToLower(tag)] = new FACTORY(tariffs); }
            private:
                TARIFFS & m_tariffs;
        };

        static const char * tag;

        CHG_TARIFF(const ADMIN & admin, TARIFFS & tariffs)
            : BASE_PARSER(admin, tag), m_tariffs(tariffs) {}
        int Start(void * data, const char * el, const char ** attr);

    private:
        TARIFF_DATA_RES td;
        TARIFFS & m_tariffs;

        int CheckTariffData();
        void CreateAnswer();
};

} // namespace PARSER
} // namespace STG

#endif
