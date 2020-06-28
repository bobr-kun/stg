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

#ifndef __STG_SGCONFIG_PARSER_SERVER_INFO_H__
#define __STG_SGCONFIG_PARSER_SERVER_INFO_H__

#include "parser.h"

#include "stg/common.h"

class ADMIN;
class SETTINGS;
class USERS;
class TARIFFS;

namespace STG
{
namespace PARSER
{

class GET_SERVER_INFO: public BASE_PARSER {
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const SETTINGS & settings, const USERS & users, const TARIFFS & tariffs)
                    : m_settings(settings), m_users(users), m_tariffs(tariffs) {}
                virtual BASE_PARSER * create(const ADMIN & admin) { return new GET_SERVER_INFO(admin, m_settings, m_users, m_tariffs); }
                static void Register(REGISTRY & registry, const SETTINGS & settings, const USERS & users, const TARIFFS & tariffs)
                { registry[ToLower(tag)] = new FACTORY(settings, users, tariffs); }
            private:
                const SETTINGS & m_settings;
                const USERS & m_users;
                const TARIFFS & m_tariffs;
        };

        static const char * tag;

        GET_SERVER_INFO(const ADMIN & admin,
                        const SETTINGS & settings,
                        const USERS & users,
                        const TARIFFS & tariffs)
            : BASE_PARSER(admin, tag),
              m_settings(settings),
              m_users(users),
              m_tariffs(tariffs)
        {}

    private:
        const SETTINGS & m_settings;
        const USERS & m_users;
        const TARIFFS & m_tariffs;

        void CreateAnswer();
};

}
}

#endif
