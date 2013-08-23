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

#include "stg/parser_server_info.h"

#include "stg/common.h"

#include <cstdio> // sprintf
#include <cstddef>

#include <strings.h>

namespace
{

const size_t UNAME_LEN    = 256;
const size_t SERV_VER_LEN = 64;
const size_t DIRNAME_LEN  = 16;

}

PARSER_SERVER_INFO::PARSER_SERVER_INFO()
    : callback(NULL),
      data(NULL),
      depth(0),
      parsingAnswer(false)
{
    AddParser(propertyParsers, "uname", info.uname);
    AddParser(propertyParsers, "version", info.version);
    AddParser(propertyParsers, "tariff", info.tariffType);
    AddParser(propertyParsers, "dir_num", info.dirNum);
    AddParser(propertyParsers, "users_num", info.usersNum);
    AddParser(propertyParsers, "tariff_num", info.tariffNum);

    for (size_t i = 0; i < DIR_NUM; i++)
        AddParser(propertyParsers, "dir_name_" + x2str(i), info.dirName[i], GetEncodedValue);
}
//-----------------------------------------------------------------------------
int PARSER_SERVER_INFO::ParseStart(const char *el, const char **attr)
{
depth++;
if (depth == 1)
    if (strcasecmp(el, "ServerInfo") == 0)
        parsingAnswer = true;
else
    if (depth == 2 && parsingAnswer)
        if (!TryParse(propertyParsers, ToLower(el), attr))
            error = "Invalid parameter.";
return 0;
}
//-----------------------------------------------------------------------------
void PARSER_SERVER_INFO::ParseEnd(const char * /*el*/)
{
depth--;
if (depth == 0 && parsingAnswer)
    {
    if (callback)
        callback(error.empty(), error, info, data);
    error.clear();
    parsingAnswer = false;
    }
}
//-----------------------------------------------------------------------------
void PARSER_SERVER_INFO::SetCallback(CALLBACK f, void * d)
{
callback = f;
data = d;
}
