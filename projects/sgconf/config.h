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
 *    Author : Maxim Mamontov <faust@stargazer.dp.ua>
 */

#ifndef __STG_SGCONF_CONFIG_H__
#define __STG_SGCONF_CONFIG_H__

#include "stg/common.h"
#include "stg/resetable.h"
#include "stg/os_int.h"

#include <string>

namespace SGCONF
{

struct CONFIG
{
    RESETABLE<std::string> configFile;
    RESETABLE<std::string> server;
    RESETABLE<uint16_t> port;
    RESETABLE<std::string> userName;
    RESETABLE<std::string> userPass;

    std::string Serialize() const
    {
        std::string res("{ ");
        if (!configFile.empty())
            res += "configFile: '" + configFile.data() + "'";
        if (!server.empty())
            res += ", server: '" + server.data() + "'";
        if (!port.empty())
            res += ", port: " + x2str(port.data());
        if (!userName.empty())
            res += ", userName: '" + userName.data() + "'";
        if (!userPass.empty())
            res += ", userPass: '" + userPass.data() + "'";
        return res + " }";
    }
};

}

#endif