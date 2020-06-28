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

#include "parser_admins.h"

#include "stg/admins.h"

#include <strings.h> // strcasecmp

using STG::PARSER::GET_ADMINS;
using STG::PARSER::ADD_ADMIN;
using STG::PARSER::DEL_ADMIN;
using STG::PARSER::CHG_ADMIN;

const char * GET_ADMINS::tag = "GetAdmins";
const char * ADD_ADMIN::tag  = "AddAdmin";
const char * DEL_ADMIN::tag  = "DelAdmin";
const char * CHG_ADMIN::tag  = "ChgAdmin";

void GET_ADMINS::CreateAnswer()
{
    const PRIV * priv = m_currAdmin.GetPriv();
    if (!priv->adminChg)
    {
        m_answer = "<Error Result=\"Error. Access denied.\"/>";
        return;
    }

    m_answer = "<Admins>";
    ADMIN_CONF ac;
    int h = m_admins.OpenSearch();

    while (m_admins.SearchNext(h, &ac) == 0)
    {
        unsigned int p = (ac.priv.userStat << 0) +
                         (ac.priv.userConf << 2) +
                         (ac.priv.userCash << 4) +
                         (ac.priv.userPasswd << 6) +
                         (ac.priv.userAddDel << 8) +
                         (ac.priv.adminChg << 10) +
                         (ac.priv.tariffChg << 12);
        m_answer += "<admin login=\"" + ac.login + "\" priv=\"" + x2str(p) + "\"/>";
    }
    m_admins.CloseSearch(h);
    m_answer += "</Admins>";
}

int DEL_ADMIN::Start(void *, const char * el, const char ** attr)
{
    if (strcasecmp(el, m_tag.c_str()) == 0)
    {
        m_admin = attr[1];
        return 0;
    }
    return -1;
}

void DEL_ADMIN::CreateAnswer()
{
    if (m_admins.Del(m_admin, &m_currAdmin) == 0)
        m_answer = "<" + m_tag + " Result=\"Ok\"/>";
    else
        m_answer = "<" + m_tag + " Result=\"Error. " + m_admins.GetStrError() + "\"/>";
}

int ADD_ADMIN::Start(void *, const char *el, const char **attr)
{
    if (strcasecmp(el, m_tag.c_str()) == 0)
    {
        m_admin = attr[1];
        return 0;
    }
    return -1;
}

void ADD_ADMIN::CreateAnswer()
{
    if (m_admins.Add(m_admin, &m_currAdmin) == 0)
        m_answer = "<" + m_tag + " Result=\"Ok\"/>";
    else
        m_answer = "<" + m_tag + " Result=\"Error. " + m_admins.GetStrError() + "\"/>";
}

int CHG_ADMIN::Start(void *, const char * el, const char ** attr)
{
    if (strcasecmp(el, m_tag.c_str()) == 0)
    {
        for (size_t i = 0; i < 6; i += 2)
        {
            printfd(__FILE__, "PARSER_CHG_ADMIN::attr[%d] = %s\n", i, attr[i]);
            if (attr[i] == NULL)
                break;

            if (strcasecmp(attr[i], "Login") == 0)
            {
                login = attr[i + 1];
                continue;
            }

            if (strcasecmp(attr[i], "Priv") == 0)
            {
                privAsString = attr[i + 1];
                continue;
            }

            if (strcasecmp(attr[i], "Password") == 0)
            {
                password = attr[i + 1];
                continue;
            }
        }

        return 0;
    }
    return -1;
}

void CHG_ADMIN::CreateAnswer()
{
    if (!login.empty())
    {
        ADMIN * origAdmin = NULL;

        if (m_admins.Find(login, &origAdmin))
        {
            m_answer = "<" + m_tag + " Result = \"Admin '" + login + "' is not found.\"/>";
            return;
        }

        ADMIN_CONF conf(origAdmin->GetConf());

        if (!password.empty())
            conf.password = password.data();

        if (!privAsString.empty())
        {
            int p = 0;
            if (str2x(privAsString.data().c_str(), p) < 0)
            {
                m_answer = "<" + m_tag + " Result = \"Incorrect parameter Priv.\"/>";
                return;
            }

            conf.priv.FromInt(p);
        }

        if (m_admins.Change(conf, &m_currAdmin) != 0)
            m_answer = "<" + m_tag + " Result = \"" + m_admins.GetStrError() + "\"/>";
        else
            m_answer = "<" + m_tag + " Result = \"Ok\"/>";
    }
    else
        m_answer = "<" + m_tag + " Result = \"Incorrect parameter login.\"/>";
}
