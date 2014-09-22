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
 *    Author : Maxim Mamontov <faust@stargazer.dp.ua>
 */

#ifndef __STG_SGCONFIG_PARSER_USERS_H__
#define __STG_SGCONFIG_PARSER_USERS_H__

#include "parser.h"

#include "stg/user_conf.h"
#include "stg/user_stat.h"
#include "stg/resetable.h"

#include <string>

class USERS;
class USER;
class TARIFFS;
class ADMIN;
class STORE;

namespace STG
{
namespace PARSER
{

class GET_USERS: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const ADMIN & admin, USERS & users)
                    : m_admin(admin), m_users(users)
                {}
                virtual BASE_PARSER * create() { return new GET_USERS(m_admin, m_users); }
            private:
                const ADMIN & m_admin;
                USERS & m_users;
        };

        GET_USERS(const ADMIN & admin, USERS & users)
            : BASE_PARSER(admin, "GetUsers"), m_users(users),
              m_lastUserUpdateTime(0) {}
        int Start(void * data, const char * el, const char ** attr);

    private:
        USERS & m_users;
        time_t m_lastUserUpdateTime;

        void CreateAnswer();
};

class GET_USER: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const ADMIN & admin, const USERS & users)
                    : m_admin(admin), m_users(users)
                {}
                virtual BASE_PARSER * create() { return new GET_USER(m_admin, m_users); }
            private:
                const ADMIN & m_admin;
                const USERS & m_users;
        };

        GET_USER(const ADMIN & admin, const USERS & users)
            : BASE_PARSER(admin, "GetUser"), m_users(users) {}
        int Start(void * data, const char * el, const char ** attr);

    private:
        const USERS & m_users;
        std::string m_login;

        void CreateAnswer();
};

class ADD_USER: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const ADMIN & admin, USERS & users)
                    : m_admin(admin), m_users(users)
                {}
                virtual BASE_PARSER * create() { return new ADD_USER(m_admin, m_users); }
            private:
                const ADMIN & m_admin;
                USERS & m_users;
        };

        ADD_USER(const ADMIN & admin, USERS & users)
            : BASE_PARSER(admin, "AddUser"), m_users(users) {}
        int Start(void * data, const char * el, const char ** attr);

    private:
        USERS & m_users;
        std::string m_login;

        void CreateAnswer();
};

class CHG_USER: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const ADMIN & admin, USERS & users, STORE & store, const TARIFFS & tariffs)
                    : m_admin(admin), m_users(users), m_store(store), m_tariffs(tariffs)
                {}
                virtual BASE_PARSER * create() { return new CHG_USER(m_admin, m_users, m_store, m_tariffs); }
            private:
                const ADMIN & m_admin;
                USERS & m_users;
                STORE & m_store;
                const TARIFFS & m_tariffs;
        };

        CHG_USER(const ADMIN & admin, USERS & users,
                 STORE & store, const TARIFFS & tariffs)
            : BASE_PARSER(admin, "SetUser"),
              m_users(users),
              m_store(store),
              m_tariffs(tariffs),
              m_cashMustBeAdded(false),
              m_res(0) {}

        int Start(void * data, const char * el, const char ** attr);

    private:
        USERS & m_users;
        STORE & m_store;
        const TARIFFS & m_tariffs;
        USER_STAT_RES m_usr;
        USER_CONF_RES m_ucr;
        RESETABLE<uint64_t> m_upr[DIR_NUM];
        RESETABLE<uint64_t> m_downr[DIR_NUM];
        std::string m_cashMsg;
        std::string m_login;
        bool m_cashMustBeAdded;
        int m_res;

        int ApplyChanges();
        void CreateAnswer();
};

class DEL_USER: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const ADMIN & admin, USERS & users)
                    : m_admin(admin), m_users(users)
                {}
                virtual BASE_PARSER * create() { return new DEL_USER(m_admin, m_users); }
            private:
                const ADMIN & m_admin;
                USERS & m_users;
        };

        DEL_USER(const ADMIN & admin, USERS & users)
            : BASE_PARSER(admin, "DelUser"), m_users(users), res(0), u(NULL) {}
        int Start(void * data, const char * el, const char ** attr);
        int End(void * data, const char * el);

    private:
        USERS & m_users;
        int res;
        USER * u;

        void CreateAnswer();
};

class CHECK_USER: public BASE_PARSER
{
    public:
        class FACTORY : public BASE_PARSER::FACTORY
        {
            public:
                FACTORY(const ADMIN & admin, const USERS & users)
                    : m_admin(admin), m_users(users)
                {}
                virtual BASE_PARSER * create() { return new CHECK_USER(m_admin, m_users); }
            private:
                const ADMIN & m_admin;
                const USERS & m_users;
        };

        CHECK_USER(const ADMIN & admin, const USERS & users)
            : BASE_PARSER(admin, "CheckUser"), m_users(users) {}
        int Start(void * data, const char * el, const char ** attr);
        int End(void * data, const char * el);

    private:
        const USERS & m_users;

        void CreateAnswer(const char * error);
        void CreateAnswer() {} // dummy
};

} // namespace PARSER
} // namespace STG

#endif
