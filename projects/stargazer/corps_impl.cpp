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
 *    Author : Maksym Mamontov <stg@madf.info>
 */

#include "corps_impl.h"

#include "stg/admin.h"
#include "stg/common.h"

#include <cerrno>
#include <cassert>
#include <algorithm>

//-----------------------------------------------------------------------------
CORPORATIONS_IMPL::CORPORATIONS_IMPL(STORE * st)
    : CORPORATIONS(),
      data(),
      store(st),
      WriteServLog(GetStgLogger()),
      searchDescriptors(),
      handle(0),
      mutex(),
      strError()
{
pthread_mutex_init(&mutex, NULL);
Read();
}
//-----------------------------------------------------------------------------
int CORPORATIONS_IMPL::Add(const CORP_CONF & corp, const ADMIN * admin)
{
STG_LOCKER lock(&mutex);
const PRIV * priv = admin->GetPriv();

if (!priv->corpChg)
    {
    std::string s = admin->GetLogStr() + " Add corporation \'" + corp.name + "\'. Access denied.";
    strError = "Access denied.";
    WriteServLog(s.c_str());
    return -1;
    }

crp_iter si(find(data.begin(), data.end(), corp));

if (si != data.end())
    {
    strError = "Corporation \'" + corp.name + "\' cannot not be added. Corporation already exist.";
    WriteServLog("%s %s", admin->GetLogStr().c_str(), strError.c_str());

    return -1;
    }

data.push_back(corp);

if (store->AddCorp(corp.name) == 0)
    {
    WriteServLog("%s Corporation \'%s\' added.",
                 admin->GetLogStr().c_str(), corp.name.c_str());
    return 0;
    }

strError = "Corporation \'" + corp.name + "\' was not added. Error: " + store->GetStrError();
WriteServLog("%s %s", admin->GetLogStr().c_str(), strError.c_str());

return -1;
}
//-----------------------------------------------------------------------------
int CORPORATIONS_IMPL::Del(const std::string & name, const ADMIN * admin)
{
STG_LOCKER lock(&mutex);
const PRIV * priv = admin->GetPriv();

if (!priv->corpChg)
    {
    std::string s = admin->GetLogStr() + " Delete corporation \'" + name + "\'. Access denied.";
    strError = "Access denied.";
    WriteServLog(s.c_str());
    return -1;
    }

crp_iter si(find(data.begin(), data.end(), CORP_CONF(name)));

if (si == data.end())
    {
    strError = "Corporation \'" + name + "\' cannot be deleted. Corporation does not exist.";
    WriteServLog("%s %s", admin->GetLogStr().c_str(), strError.c_str());
    return -1;
    }

std::map<int, const_crp_iter>::iterator csi;
csi = searchDescriptors.begin();
while (csi != searchDescriptors.end())
    {
    if (csi->second == si)
        (csi->second)++;
    ++csi;
    }

data.remove(*si);
if (store->DelCorp(name) < 0)
    {
    strError = "Corporation \'" + name + "\' was not deleted. Error: " + store->GetStrError();
    WriteServLog("%s %s", admin->GetLogStr().c_str(), strError.c_str());

    return -1;
    }

WriteServLog("%s Corporation \'%s\' deleted.", admin->GetLogStr().c_str(), name.c_str());
return 0;
}
//-----------------------------------------------------------------------------
int CORPORATIONS_IMPL::Change(const CORP_CONF & corp, const ADMIN * admin)
{
STG_LOCKER lock(&mutex);
const PRIV * priv = admin->GetPriv();

if (!priv->corpChg)
    {
    std::string s = admin->GetLogStr() + " Change corporation \'" + corp.name + "\'. Access denied.";
    strError = "Access denied.";
    WriteServLog(s.c_str());
    return -1;
    }

crp_iter si(find(data.begin(), data.end(), corp));

if (si == data.end())
    {
    strError = "Corporation \'" + corp.name + "\' cannot be changed " + ". Corporation does not exist.";
    WriteServLog("%s %s", admin->GetLogStr().c_str(), strError.c_str());
    return -1;
    }

*si = corp;
if (store->SaveCorp(corp))
    {
    WriteServLog("Cannot write corporation %s.", corp.name.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    return -1;
    }

WriteServLog("%s Corporation \'%s\' changed.",
             admin->GetLogStr().c_str(), corp.name.c_str());

return 0;
}
//-----------------------------------------------------------------------------
bool CORPORATIONS_IMPL::Read()
{
STG_LOCKER lock(&mutex);
std::vector<std::string> corpsList;
if (store->GetCorpsList(&corpsList) < 0)
    {
    WriteServLog(store->GetStrError().c_str());
    return true;
    }

for (size_t i = 0; i < corpsList.size(); i++)
    {
    CORP_CONF corp;

    if (store->RestoreCorp(&corp, corpsList[i]))
        {
        WriteServLog(store->GetStrError().c_str());
        return true;
        }

    data.push_back(corp);
    }
return false;
}
//-----------------------------------------------------------------------------
bool CORPORATIONS_IMPL::Find(const std::string & name, CORP_CONF * corp)
{
assert(corp != NULL && "Pointer to corporation is not null");

STG_LOCKER lock(&mutex);
if (data.empty())
    return false;

crp_iter si(find(data.begin(), data.end(), CORP_CONF(name)));

if (si != data.end())
    {
    *corp = *si;
    return false;
    }

return true;
}
//-----------------------------------------------------------------------------
bool CORPORATIONS_IMPL::Exists(const std::string & name) const
{
STG_LOCKER lock(&mutex);
if (data.empty())
    {
    printfd(__FILE__, "no admin in system!\n");
    return true;
    }

const_crp_iter si(find(data.begin(), data.end(), CORP_CONF(name)));

if (si != data.end())
    return true;

return false;
}
//-----------------------------------------------------------------------------
int CORPORATIONS_IMPL::OpenSearch() const
{
STG_LOCKER lock(&mutex);
handle++;
searchDescriptors[handle] = data.begin();
return handle;
}
//-----------------------------------------------------------------------------
int CORPORATIONS_IMPL::SearchNext(int h, CORP_CONF * corp) const
{
STG_LOCKER lock(&mutex);
if (searchDescriptors.find(h) == searchDescriptors.end())
    {
    WriteServLog("CORPORATIONS. Incorrect search handle.");
    return -1;
    }

if (searchDescriptors[h] == data.end())
    return -1;

*corp = *searchDescriptors[h]++;

return 0;
}
//-----------------------------------------------------------------------------
int CORPORATIONS_IMPL::CloseSearch(int h) const
{
STG_LOCKER lock(&mutex);
if (searchDescriptors.find(h) != searchDescriptors.end())
    {
    searchDescriptors.erase(searchDescriptors.find(h));
    return 0;
    }

WriteServLog("CORPORATIONS. Incorrect search handle.");
return -1;
}
//-----------------------------------------------------------------------------
