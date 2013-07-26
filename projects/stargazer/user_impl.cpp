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
 *    Date: 27.10.2002
 */

/*
 *    Author : Boris Mikhailenko <stg34@stargazer.dp.ua>
 */

/*
 $Revision: 1.101 $
 $Date: 2010/11/03 10:50:03 $
 $Author: faust $
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <pthread.h>
#include <unistd.h> // access

#include <cassert>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <functional>

#include "stg/users.h"
#include "stg/common.h"
#include "stg/scriptexecuter.h"
#include "stg/tariff.h"
#include "stg/tariffs.h"
#include "stg/admin.h"
#include "user_impl.h"
#include "settings_impl.h"
#include "stg_timer.h"

#ifdef USE_ABSTRACT_SETTINGS
USER_IMPL::USER_IMPL(const SETTINGS * s,
           const STORE * st,
           const TARIFFS * t,
           const ADMIN * a,
           const USERS * u)
    : USER(),
      users(u),
      property(s->GetScriptsDir()),
      WriteServLog(GetStgLogger()),
      lastScanMessages(0),
      login(),
      id(0),
      __connected(0),
      connected(__connected),
      enabledDirs(),
      userIDGenerator(),
      __currIP(0),
      currIP(__currIP),
      lastIPForDisconnect(0),
      pingTime(0),
      sysAdmin(a),
      store(st),
      tariffs(t),
      tariff(NULL),
      traffStat(),
      traffStatSaved(),
      settings(s),
      authorizedBy(),
      messages(),
      deleted(false),
      lastWriteStat(0),
      lastWriteDetailedStat(0),
      cash(property.cash),
      up(property.up),
      down(property.down),
      lastCashAdd(property.lastCashAdd),
      passiveTime(property.passiveTime),
      lastCashAddTime(property.lastCashAddTime),
      freeMb(property.freeMb),
      lastActivityTime(property.lastActivityTime),
      password(property.password),
      passive(property.passive),
      disabled(property.disabled),
      disabledDetailStat(property.disabledDetailStat),
      alwaysOnline(property.alwaysOnline),
      tariffName(property.tariffName),
      nextTariff(property.nextTariff),
      address(property.address),
      note(property.note),
      group(property.group),
      email(property.email),
      phone(property.phone),
      realName(property.realName),
      credit(property.credit),
      creditExpire(property.creditExpire),
      ips(property.ips),
      userdata0(property.userdata0),
      userdata1(property.userdata1),
      userdata2(property.userdata2),
      userdata3(property.userdata3),
      userdata4(property.userdata4),
      userdata5(property.userdata5),
      userdata6(property.userdata6),
      userdata7(property.userdata7),
      userdata8(property.userdata8),
      userdata9(property.userdata9),
      sessionUpload(),
      sessionDownload(),
      passiveNotifier(this),
      tariffNotifier(this),
      cashNotifier(this),
      ipNotifier(this),
      mutex(),
      errorStr()
{
password = "*_EMPTY_PASSWORD_*";
tariffName = NO_TARIFF_NAME;
ips = StrToIPS("*");
lastWriteStat = stgTime + random() % settings->GetStatWritePeriod();
lastWriteDetailedStat = stgTime;

property.tariffName.AddBeforeNotifier(&tariffNotifier);
property.passive.AddBeforeNotifier(&passiveNotifier);
property.disabled.AddAfterNotifier(&disabledNotifier);
property.cash.AddBeforeNotifier(&cashNotifier);
ips.AddAfterNotifier(&ipNotifier);

pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutex, &attr);
}
#else
USER_IMPL::USER_IMPL(const SETTINGS_IMPL * s,
                     const STORE * st,
                     const TARIFFS * t,
                     const ADMIN * a,
                     const USERS * u)
    : USER(),
      users(u),
      property(s->GetScriptsDir()),
      WriteServLog(GetStgLogger()),
      lastScanMessages(0),
      login(),
      id(0),
      __connected(0),
      connected(__connected),
      enabledDirs(),
      userIDGenerator(),
      __currIP(0),
      currIP(__currIP),
      lastIPForDisconnect(0),
      pingTime(0),
      sysAdmin(a),
      store(st),
      tariffs(t),
      tariff(NULL),
      traffStat(),
      traffStatSaved(),
      settings(s),
      authorizedBy(),
      messages(),
      deleted(false),
      lastWriteStat(0),
      lastWriteDetailedStat(0),
      cash(property.cash),
      up(property.up),
      down(property.down),
      lastCashAdd(property.lastCashAdd),
      passiveTime(property.passiveTime),
      lastCashAddTime(property.lastCashAddTime),
      freeMb(property.freeMb),
      lastActivityTime(property.lastActivityTime),
      password(property.password),
      passive(property.passive),
      disabled(property.disabled),
      disabledDetailStat(property.disabledDetailStat),
      alwaysOnline(property.alwaysOnline),
      tariffName(property.tariffName),
      nextTariff(property.nextTariff),
      address(property.address),
      note(property.note),
      group(property.group),
      email(property.email),
      phone(property.phone),
      realName(property.realName),
      credit(property.credit),
      creditExpire(property.creditExpire),
      ips(property.ips),
      userdata0(property.userdata0),
      userdata1(property.userdata1),
      userdata2(property.userdata2),
      userdata3(property.userdata3),
      userdata4(property.userdata4),
      userdata5(property.userdata5),
      userdata6(property.userdata6),
      userdata7(property.userdata7),
      userdata8(property.userdata8),
      userdata9(property.userdata9),
      sessionUpload(),
      sessionDownload(),
      passiveNotifier(this),
      disabledNotifier(this),
      tariffNotifier(this),
      cashNotifier(this),
      ipNotifier(this),
      mutex(),
      errorStr()
{
password = "*_EMPTY_PASSWORD_*";
tariffName = NO_TARIFF_NAME;
ips = StrToIPS("*");
lastWriteStat = stgTime + random() % settings->GetStatWritePeriod();
lastWriteDetailedStat = stgTime;

property.tariffName.AddBeforeNotifier(&tariffNotifier);
property.passive.AddBeforeNotifier(&passiveNotifier);
property.disabled.AddAfterNotifier(&disabledNotifier);
property.cash.AddBeforeNotifier(&cashNotifier);
ips.AddAfterNotifier(&ipNotifier);

pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutex, &attr);
}
#endif
//-----------------------------------------------------------------------------
USER_IMPL::USER_IMPL(const USER_IMPL & u)
    : USER(),
      users(u.users),
      property(u.settings->GetScriptsDir()),
      WriteServLog(GetStgLogger()),
      lastScanMessages(0),
      login(u.login),
      id(u.id),
      __connected(0),
      connected(__connected),
      enabledDirs(),
      userIDGenerator(u.userIDGenerator),
      __currIP(u.__currIP),
      currIP(__currIP),
      lastIPForDisconnect(0),
      pingTime(u.pingTime),
      sysAdmin(u.sysAdmin),
      store(u.store),
      tariffs(u.tariffs),
      tariff(u.tariff),
      traffStat(u.traffStat),
      traffStatSaved(u.traffStatSaved),
      settings(u.settings),
      authorizedBy(),
      messages(u.messages),
      deleted(u.deleted),
      lastWriteStat(u.lastWriteStat),
      lastWriteDetailedStat(u.lastWriteDetailedStat),
      cash(property.cash),
      up(property.up),
      down(property.down),
      lastCashAdd(property.lastCashAdd),
      passiveTime(property.passiveTime),
      lastCashAddTime(property.lastCashAddTime),
      freeMb(property.freeMb),
      lastActivityTime(property.lastActivityTime),
      password(property.password),
      passive(property.passive),
      disabled(property.disabled),
      disabledDetailStat(property.disabledDetailStat),
      alwaysOnline(property.alwaysOnline),
      tariffName(property.tariffName),
      nextTariff(property.nextTariff),
      address(property.address),
      note(property.note),
      group(property.group),
      email(property.email),
      phone(property.phone),
      realName(property.realName),
      credit(property.credit),
      creditExpire(property.creditExpire),
      ips(property.ips),
      userdata0(property.userdata0),
      userdata1(property.userdata1),
      userdata2(property.userdata2),
      userdata3(property.userdata3),
      userdata4(property.userdata4),
      userdata5(property.userdata5),
      userdata6(property.userdata6),
      userdata7(property.userdata7),
      userdata8(property.userdata8),
      userdata9(property.userdata9),
      sessionUpload(),
      sessionDownload(),
      passiveNotifier(this),
      disabledNotifier(this),
      tariffNotifier(this),
      cashNotifier(this),
      ipNotifier(this),
      mutex(),
      errorStr()
{
if (&u == this)
    return;

property.tariffName.AddBeforeNotifier(&tariffNotifier);
property.passive.AddBeforeNotifier(&passiveNotifier);
property.disabled.AddAfterNotifier(&disabledNotifier);
property.cash.AddBeforeNotifier(&cashNotifier);
ips.AddAfterNotifier(&ipNotifier);

property.SetProperties(u.property);

pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutex, &attr);
}
//-----------------------------------------------------------------------------
USER_IMPL::~USER_IMPL()
{
property.tariffName.DelBeforeNotifier(&tariffNotifier);
property.passive.DelBeforeNotifier(&passiveNotifier);
property.disabled.DelAfterNotifier(&disabledNotifier);
property.cash.DelBeforeNotifier(&cashNotifier);
pthread_mutex_destroy(&mutex);
}
//-----------------------------------------------------------------------------
void USER_IMPL::SetLogin(const std::string & l)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
assert(login.empty() && "Login is already set");
login = l;
id = userIDGenerator.GetNextID();
}
//-----------------------------------------------------------------------------
int USER_IMPL::ReadConf()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
USER_CONF conf;

if (store->RestoreUserConf(&conf, login))
    {
    WriteServLog("Cannot read conf for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    printfd(__FILE__, "Cannot read conf for user %s.\n", login.c_str());
    printfd(__FILE__, "%s\n", store->GetStrError().c_str());
    return -1;
    }

property.SetConf(conf);

tariff = tariffs->FindByName(tariffName);
if (tariff == NULL)
    {
    WriteServLog("Cannot read user %s. Tariff %s not exist.",
                 login.c_str(), property.tariffName.Get().c_str());
    return -1;
    }

std::vector<STG_MSG_HDR> hdrsList;

if (store->GetMessageHdrs(&hdrsList, login))
    {
    printfd(__FILE__, "Error GetMessageHdrs %s\n", store->GetStrError().c_str());
    WriteServLog("Cannot read user %s. Error reading message headers: %s.",
                 login.c_str(),
                 store->GetStrError().c_str());
    return -1;
    }

std::vector<STG_MSG_HDR>::const_iterator it;
for (it = hdrsList.begin(); it != hdrsList.end(); ++it)
    {
    STG_MSG msg;
    if (store->GetMessage(it->id, &msg, login) == 0)
        {
        messages.push_back(msg);
        }
    }

return 0;
}
//-----------------------------------------------------------------------------
int USER_IMPL::ReadStat()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
USER_STAT stat;

if (store->RestoreUserStat(&stat, login))
    {
    WriteServLog("Cannot read stat for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    printfd(__FILE__, "Cannot read stat for user %s.\n", login.c_str());
    printfd(__FILE__, "%s\n", store->GetStrError().c_str());
    return -1;
    }

property.SetStat(stat);

return 0;
}
//-----------------------------------------------------------------------------
int USER_IMPL::WriteConf()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
USER_CONF conf(property.GetConf());

printfd(__FILE__, "USER::WriteConf()\n");

if (store->SaveUserConf(conf, login))
    {
    WriteServLog("Cannot write conf for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    printfd(__FILE__, "Cannot write conf for user %s.\n", login.c_str());
    printfd(__FILE__, "%s\n", store->GetStrError().c_str());
    return -1;
    }

return 0;
}
//-----------------------------------------------------------------------------
int USER_IMPL::WriteStat()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
USER_STAT stat(property.GetStat());

if (store->SaveUserStat(stat, login))
    {
    WriteServLog("Cannot write stat for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    printfd(__FILE__, "Cannot write stat for user %s.\n", login.c_str());
    printfd(__FILE__, "%s\n", store->GetStrError().c_str());
    return -1;
    }

lastWriteStat = stgTime;

return 0;
}
//-----------------------------------------------------------------------------
int USER_IMPL::WriteMonthStat()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
time_t tt = stgTime - 3600;
struct tm t1;
localtime_r(&tt, &t1);

USER_STAT stat(property.GetStat());
if (store->SaveMonthStat(stat, t1.tm_mon, t1.tm_year, login))
    {
    WriteServLog("Cannot write month stat for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    printfd(__FILE__, "Cannot write month stat for user %s.\n", login.c_str());
    printfd(__FILE__, "%s\n", store->GetStrError().c_str());
    return -1;
    }

return 0;
}
//-----------------------------------------------------------------------------
int USER_IMPL::Authorize(uint32_t ip, uint32_t dirs, const AUTH * auth)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
/*
 *  Authorize user. It only means that user will be authorized. Nothing more.
 *  User can be connected or disconnected while authorized.
 *  Example: user is authorized but disconnected due to 0 money or blocking
 */

/*
 * Prevent double authorization by identical authorizers
 */
if (authorizedBy.find(auth) != authorizedBy.end())
    return 0;

if (!ip)
    return -1;

for (int i = 0; i < DIR_NUM; i++)
    {
    enabledDirs[i] = dirs & (1 << i);
    }

if (!authorizedBy.empty())
    {
    if (currIP != ip)
        {
        //  We are already authorized, but with different IP address
        errorStr = "User " + login + " already authorized with IP address " + inet_ntostring(ip);
        return -1;
        }

    USER * u = NULL;
    if (!users->FindByIPIdx(ip, &u))
        {
        //  Address is already present in IP-index
        //  If it's not our IP - throw an error
        if (u != this)
            {
            errorStr = "IP address " + inet_ntostring(ip) + " already in use";
            return -1;
            }
        }
    }
else
    {
    if (users->IsIPInIndex(ip))
        {
        //  Address is already present in IP-index
        errorStr = "IP address " + inet_ntostring(ip) + " already in use";
        return -1;
        }

    if (ips.ConstData().IsIPInIPS(ip))
        {
        currIP = ip;
        lastIPForDisconnect = currIP;
        }
    else
        {
        printfd(__FILE__, " user %s: ips = %s\n", login.c_str(), ips.ConstData().GetIpStr().c_str());
        errorStr = "IP address " + inet_ntostring(ip) + " not belong user " + login;
        return -1;
        }
    }

authorizedBy.insert(auth);

ScanMessage();

return 0;
}
//-----------------------------------------------------------------------------
void USER_IMPL::Unauthorize(const AUTH * auth)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
/*
 *  Authorizer tries to unauthorize user, that was not authorized by it
 */
if (!authorizedBy.erase(auth))
    return;

if (authorizedBy.empty())
    {
    lastIPForDisconnect = currIP;
    currIP = 0; // DelUser in traffcounter
    return;
    }
}
//-----------------------------------------------------------------------------
bool USER_IMPL::IsAuthorizedBy(const AUTH * auth) const
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
//  Is this user authorized by specified authorizer?
return authorizedBy.find(auth) != authorizedBy.end();
}
//-----------------------------------------------------------------------------
std::vector<std::string> USER_IMPL::GetAuthorizers() const
{
    std::vector<std::string> list;
    std::transform(authorizedBy.begin(), authorizedBy.end(), std::back_inserter(list), std::mem_fun(&AUTH::GetVersion));
    return list;
}
//-----------------------------------------------------------------------------
void USER_IMPL::Connect(bool fakeConnect)
{
/*
 *  Connect user to Internet. This function is differ from Authorize() !!!
 */

STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (!fakeConnect)
    {
    std::string scriptOnConnect = settings->GetScriptsDir() + "/OnConnect";

    if (access(scriptOnConnect.c_str(), X_OK) == 0)
        {
        char dirsStr[DIR_NUM + 1];
        dirsStr[DIR_NUM] = 0;
        for (int i = 0; i < DIR_NUM; i++)
            {
            dirsStr[i] = enabledDirs[i] ? '1' : '0';
            }

        std::string scriptOnConnectParams;
        strprintf(&scriptOnConnectParams,
                "%s \"%s\" \"%s\" \"%f\" \"%d\" \"%s\"",
                scriptOnConnect.c_str(),
                login.c_str(),
                inet_ntostring(currIP).c_str(),
                cash.ConstData(),
                id,
                dirsStr);

        ScriptExec(scriptOnConnectParams.c_str());
        }
    else
        {
        WriteServLog("Script %s cannot be executed. File not found.", scriptOnConnect.c_str());
        }

    connected = true;
    }

if (store->WriteUserConnect(login, currIP))
    {
    WriteServLog("Cannot write connect for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    }

if (!fakeConnect)
    lastIPForDisconnect = currIP;
}
//-----------------------------------------------------------------------------
void USER_IMPL::Disconnect(bool fakeDisconnect, const std::string & reason)
{
/*
 *  Disconnect user from Internet. This function is differ from UnAuthorize() !!!
 */

STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (!lastIPForDisconnect)
    {
    printfd(__FILE__, "lastIPForDisconnect\n");
    return;
    }

if (!fakeDisconnect)
    {
    std::string scriptOnDisonnect = settings->GetScriptsDir() + "/OnDisconnect";

    if (access(scriptOnDisonnect.c_str(), X_OK) == 0)
        {
        char dirsStr[DIR_NUM + 1];
        dirsStr[DIR_NUM] = 0;
        for (int i = 0; i < DIR_NUM; i++)
            {
            dirsStr[i] = enabledDirs[i] ? '1' : '0';
            }

        std::string scriptOnDisonnectParams;
        strprintf(&scriptOnDisonnectParams,
                "%s \"%s\" \"%s\" \"%f\" \"%d\" \"%s\"",
                scriptOnDisonnect.c_str(),
                login.c_str(),
                inet_ntostring(lastIPForDisconnect).c_str(),
                cash.ConstData(),
                id,
                dirsStr);

        ScriptExec(scriptOnDisonnectParams.c_str());
        }
    else
        {
        WriteServLog("Script OnDisconnect cannot be executed. File not found.");
        }

    connected = false;
    }

if (store->WriteUserDisconnect(login, up, down, sessionUpload, sessionDownload, cash, freeMb, reason))
    {
    WriteServLog("Cannot write disconnect for user %s.", login.c_str());
    WriteServLog("%s", store->GetStrError().c_str());
    }

if (!fakeDisconnect)
    lastIPForDisconnect = 0;

DIR_TRAFF zeroSesssion;

sessionUpload = zeroSesssion;
sessionDownload = zeroSesssion;
}
//-----------------------------------------------------------------------------
void USER_IMPL::PrintUser() const
{
//return;
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
std::cout << "============================================================" << std::endl;
std::cout << "id=" << id << std::endl;
std::cout << "login=" << login << std::endl;
std::cout << "password=" << password << std::endl;
std::cout << "passive=" << passive << std::endl;
std::cout << "disabled=" << disabled << std::endl;
std::cout << "disabledDetailStat=" << disabledDetailStat << std::endl;
std::cout << "alwaysOnline=" << alwaysOnline << std::endl;
std::cout << "tariffName=" << tariffName << std::endl;
std::cout << "address=" << address << std::endl;
std::cout << "phone=" << phone << std::endl;
std::cout << "email=" << email << std::endl;
std::cout << "note=" << note << std::endl;
std::cout << "realName=" <<realName << std::endl;
std::cout << "group=" << group << std::endl;
std::cout << "credit=" << credit << std::endl;
std::cout << "nextTariff=" << nextTariff << std::endl;
std::cout << "userdata0" << userdata0 << std::endl;
std::cout << "userdata1" << userdata1 << std::endl;
std::cout << "creditExpire=" << creditExpire << std::endl;
std::cout << "ips=" << ips << std::endl;
std::cout << "------------------------" << std::endl;
std::cout << "up=" << up << std::endl;
std::cout << "down=" << down << std::endl;
std::cout << "cash=" << cash << std::endl;
std::cout << "freeMb=" << freeMb << std::endl;
std::cout << "lastCashAdd=" << lastCashAdd << std::endl;
std::cout << "lastCashAddTime=" << lastCashAddTime << std::endl;
std::cout << "passiveTime=" << passiveTime << std::endl;
std::cout << "lastActivityTime=" << lastActivityTime << std::endl;
std::cout << "============================================================" << std::endl;
}
//-----------------------------------------------------------------------------
void USER_IMPL::Run()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (stgTime > static_cast<time_t>(lastWriteStat + settings->GetStatWritePeriod()))
    {
    printfd(__FILE__, "USER::WriteStat user=%s\n", GetLogin().c_str());
    WriteStat();
    }
if (creditExpire.ConstData() && creditExpire.ConstData() < stgTime)
    {
    WriteServLog("User: %s. Credit expired.", login.c_str());
    credit = 0;
    creditExpire = 0;
    WriteConf();
    }

if (passive.ConstData()
    && (stgTime % 30 == 0)
    && (passiveTime.ModificationTime() != stgTime))
    {
    passiveTime = passiveTime + (stgTime - passiveTime.ModificationTime());
    printfd(__FILE__, "===== %s: passiveTime=%d =====\n", login.c_str(), passiveTime.ConstData());
    }

if (!authorizedBy.empty())
    {
    if (connected)
        {
        property.Stat().lastActivityTime = stgTime;
        }
    if (!connected && IsInetable())
        {
        Connect();
        }
    if (connected && !IsInetable())
        {
        if (disabled)
            Disconnect(false, "disabled");
        else if (passive)
            Disconnect(false, "passive");
        else
            Disconnect(false, "no cash");
        }

    if (stgTime - lastScanMessages > 10)
        {
        ScanMessage();
        lastScanMessages = stgTime;
        }
    }
else
    {
    if (connected)
        {
        Disconnect(false, "not authorized");
        }
    }

}
//-----------------------------------------------------------------------------
void USER_IMPL::UpdatePingTime(time_t t)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
//printfd(__FILE__, "UpdatePingTime(%d) %s\n", t, login.c_str());
if (t)
    pingTime = t;
else
    pingTime = stgTime;
}
//-----------------------------------------------------------------------------
bool USER_IMPL::IsInetable()
{
//STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (disabled || passive)
    return false;

if (settings->GetFreeMbAllowInet())
    {
    if (freeMb >= 0)
        return true;
    }

if (settings->GetShowFeeInCash() || tariff == NULL)
    {
    return (cash >= -credit);
    }

return (cash - tariff->GetFee() >= -credit);
}
//-----------------------------------------------------------------------------
std::string USER_IMPL::GetEnabledDirs()
{
//STG_LOCKER lock(&mutex, __FILE__, __LINE__);

std::string dirs = "";
for(int i = 0; i < DIR_NUM; i++)
    dirs += enabledDirs[i] ? "1" : "0";
return dirs;
}
//-----------------------------------------------------------------------------
#ifdef TRAFF_STAT_WITH_PORTS
void USER_IMPL::AddTraffStatU(int dir, uint32_t ip, uint16_t port, uint32_t len)
#else
void USER_IMPL::AddTraffStatU(int dir, uint32_t ip, uint32_t len)
#endif
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (!connected || tariff == NULL)
    return;

double cost = 0;
DIR_TRAFF dt(up);

int64_t traff = tariff->GetTraffByType(up.ConstData()[dir], down.ConstData()[dir]);
int64_t threshold = tariff->GetThreshold(dir) * 1024 * 1024;

dt[dir] += len;

int tt = tariff->GetTraffType();
if (tt == TRAFF_UP ||
    tt == TRAFF_UP_DOWN ||
    // Check NEW traff data
    (tt == TRAFF_MAX && dt[dir] > down.ConstData()[dir]))
    {
    double dc = 0;
    if (traff < threshold &&
        traff + len >= threshold)
        {
        // cash = partBeforeThreshold * priceBeforeThreshold +
        //        partAfterThreshold * priceAfterThreshold
        int64_t before = threshold - traff; // Chunk part before threshold
        int64_t after = len - before; // Chunk part after threshold
        dc = tariff->GetPriceWithTraffType(up.ConstData()[dir], // Traff before chunk
                                           down.ConstData()[dir],
                                           dir,
                                           stgTime) * before +
             tariff->GetPriceWithTraffType(dt[dir], // Traff after chunk
                                           down.ConstData()[dir],
                                           dir,
                                           stgTime) * after;
        }
    else
        {
        dc = tariff->GetPriceWithTraffType(up.ConstData()[dir],
                                           down.ConstData()[dir],
                                           dir,
                                           stgTime) * len;
        }

    if (freeMb.ConstData() <= 0) // FreeMb is exhausted
        cost = dc;
    else if (freeMb.ConstData() < dc) // FreeMb is partially exhausted
        cost = dc - freeMb.ConstData();

    // Direct access to internal data structures via friend-specifier
    property.Stat().freeMb -= dc;
    property.Stat().cash -= cost;
    cash.ModifyTime();
    freeMb.ModifyTime();
    }

up = dt;
sessionUpload[dir] += len;

//Add detailed stat

if (!settings->GetWriteFreeMbTraffCost() &&
     freeMb.ConstData() >= 0)
    cost = 0;

#ifdef TRAFF_STAT_WITH_PORTS
IP_DIR_PAIR idp(ip, dir, port);
#else
IP_DIR_PAIR idp(ip, dir);
#endif

std::map<IP_DIR_PAIR, STAT_NODE>::iterator lb;
lb = traffStat.lower_bound(idp);
if (lb == traffStat.end() || lb->first != idp)
    {
    traffStat.insert(lb,
                     std::make_pair(idp,
                                    STAT_NODE(len, 0, cost)));
    }
else
    {
    lb->second.cash += cost;
    lb->second.up += len;
    }
}
//-----------------------------------------------------------------------------
#ifdef TRAFF_STAT_WITH_PORTS
void USER_IMPL::AddTraffStatD(int dir, uint32_t ip, uint16_t port, uint32_t len)
#else
void USER_IMPL::AddTraffStatD(int dir, uint32_t ip, uint32_t len)
#endif
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (!connected || tariff == NULL)
    return;

double cost = 0;
DIR_TRAFF dt(down);

int64_t traff = tariff->GetTraffByType(up.ConstData()[dir], down.ConstData()[dir]);
int64_t threshold = tariff->GetThreshold(dir) * 1024 * 1024;

dt[dir] += len;

int tt = tariff->GetTraffType();
if (tt == TRAFF_DOWN ||
    tt == TRAFF_UP_DOWN ||
    // Check NEW traff data
    (tt == TRAFF_MAX && up.ConstData()[dir] <= dt[dir]))
    {
    double dc = 0;
    if (traff < threshold &&
        traff + len >= threshold)
        {
        // cash = partBeforeThreshold * priceBeforeThreshold +
        //        partAfterThreshold * priceAfterThreshold
        int64_t before = threshold - traff; // Chunk part before threshold
        int64_t after = len - before; // Chunk part after threshold
        dc = tariff->GetPriceWithTraffType(up.ConstData()[dir],
                                           down.ConstData()[dir], // Traff before chunk
                                           dir,
                                           stgTime) * before +
             tariff->GetPriceWithTraffType(up.ConstData()[dir],
                                           dt[dir], // Traff after chunk
                                           dir,
                                           stgTime) * after;
        }
    else
        {
        dc = tariff->GetPriceWithTraffType(up.ConstData()[dir],
                                           down.ConstData()[dir],
                                           dir,
                                           stgTime) * len;
        }

    if (freeMb.ConstData() <= 0) // FreeMb is exhausted
        cost = dc;
    else if (freeMb.ConstData() < dc) // FreeMb is partially exhausted
        cost = dc - freeMb.ConstData();

    property.Stat().freeMb -= dc;
    property.Stat().cash -= cost;
    cash.ModifyTime();
    freeMb.ModifyTime();
    }

down = dt;
sessionDownload[dir] += len;

//Add detailed stat

if (!settings->GetWriteFreeMbTraffCost() &&
     freeMb.ConstData() >= 0)
    cost = 0;

#ifdef TRAFF_STAT_WITH_PORTS
IP_DIR_PAIR idp(ip, dir, port);
#else
IP_DIR_PAIR idp(ip, dir);
#endif

std::map<IP_DIR_PAIR, STAT_NODE>::iterator lb;
lb = traffStat.lower_bound(idp);
if (lb == traffStat.end() || lb->first != idp)
    {
    traffStat.insert(lb,
                     std::make_pair(idp,
                                    STAT_NODE(0, len, cost)));
    }
else
    {
    lb->second.cash += cost;
    lb->second.down += len;
    }
}
//-----------------------------------------------------------------------------
void USER_IMPL::AddCurrIPBeforeNotifier(CURR_IP_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
currIP.AddBeforeNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::DelCurrIPBeforeNotifier(const CURR_IP_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
currIP.DelBeforeNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::AddCurrIPAfterNotifier(CURR_IP_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
currIP.AddAfterNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::DelCurrIPAfterNotifier(const CURR_IP_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
currIP.DelAfterNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::AddConnectedBeforeNotifier(CONNECTED_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
connected.AddBeforeNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::DelConnectedBeforeNotifier(const CONNECTED_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
connected.DelBeforeNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::AddConnectedAfterNotifier(CONNECTED_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
connected.AddAfterNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::DelConnectedAfterNotifier(const CONNECTED_NOTIFIER * notifier)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
connected.DelAfterNotifier(notifier);
}
//-----------------------------------------------------------------------------
void USER_IMPL::OnAdd()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

std::string scriptOnAdd = settings->GetScriptsDir() + "/OnUserAdd";

if (access(scriptOnAdd.c_str(), X_OK) == 0)
    {
    std::string scriptOnAddParams;
    strprintf(&scriptOnAddParams,
            "%s \"%s\"",
            scriptOnAdd.c_str(),
            login.c_str());

    ScriptExec(scriptOnAddParams.c_str());
    }
else
    {
    WriteServLog("Script %s cannot be executed. File not found.", scriptOnAdd.c_str());
    }
}
//-----------------------------------------------------------------------------
void USER_IMPL::OnDelete()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

std::string scriptOnDel = settings->GetScriptsDir() + "/OnUserDel";

if (access(scriptOnDel.c_str(), X_OK) == 0)
    {
    std::string scriptOnDelParams;
    strprintf(&scriptOnDelParams,
            "%s \"%s\"",
            scriptOnDel.c_str(),
            login.c_str());

    ScriptExec(scriptOnDelParams.c_str());
    }
else
    {
    WriteServLog("Script %s cannot be executed. File not found.", scriptOnDel.c_str());
    }

Run();
}
//-----------------------------------------------------------------------------
int USER_IMPL::WriteDetailStat(bool hard)
{
printfd(__FILE__, "USER::WriteDetailedStat() - saved size = %d\n", traffStatSaved.second.size());

if (!traffStatSaved.second.empty())
    {
    if (store->WriteDetailedStat(traffStatSaved.second, traffStatSaved.first, login))
        {
        printfd(__FILE__, "USER::WriteDetailStat() - failed to write detail stat from queue\n");
        WriteServLog("Cannot write detail stat from queue (of size %d recs) for user %s.", traffStatSaved.second.size(), login.c_str());
        WriteServLog("%s", store->GetStrError().c_str());
        return -1;
        }
    traffStatSaved.second.erase(traffStatSaved.second.begin(), traffStatSaved.second.end());
    }

TRAFF_STAT ts;

    {
    STG_LOCKER lock(&mutex, __FILE__, __LINE__);
    ts.swap(traffStat);
    }

printfd(__FILE__, "USER::WriteDetailedStat() - size = %d\n", ts.size());

if (ts.size() && !disabledDetailStat)
    {
    if (store->WriteDetailedStat(ts, lastWriteDetailedStat, login))
        {
        printfd(__FILE__, "USER::WriteDetailStat() - failed to write current detail stat\n");
        WriteServLog("Cannot write detail stat for user %s.", login.c_str());
        WriteServLog("%s", store->GetStrError().c_str());
        if (!hard)
            {
            printfd(__FILE__, "USER::WriteDetailStat() - pushing detail stat to queue\n");
            STG_LOCKER lock(&mutex, __FILE__, __LINE__);
            traffStatSaved.second.swap(ts);
            traffStatSaved.first = lastWriteDetailedStat;
            }
        return -1;
        }
    }
lastWriteDetailedStat = stgTime;
return 0;
}
//-----------------------------------------------------------------------------
double USER_IMPL::GetPassiveTimePart() const
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

static int daysInMonth[12] =
{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

struct tm tms;
time_t t = stgTime;
localtime_r(&t, &tms);

time_t secMonth = daysInMonth[(tms.tm_mon + 11) % 12] * 24 * 3600; // Previous month

if (tms.tm_year % 4 == 0 && tms.tm_mon == 1)
    {
    // Leap year
    secMonth += 24 * 3600;
    }

time_t dt = secMonth - passiveTime;

if (dt < 0)
    dt = 0;

return static_cast<double>(dt) / secMonth;
}
//-----------------------------------------------------------------------------
void USER_IMPL::SetPassiveTimeAsNewUser()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

time_t t = stgTime;
struct tm tm;
localtime_r(&t, &tm);
int daysCurrMon = DaysInCurrentMonth();
double pt = tm.tm_mday - 1;
pt /= daysCurrMon;

passiveTime = static_cast<time_t>(pt * 24 * 3600 * daysCurrMon);
}
//-----------------------------------------------------------------------------
void USER_IMPL::MidnightResetSessionStat()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (connected)
    {
    Disconnect(true, "fake");
    Connect(true);
    }
}
//-----------------------------------------------------------------------------
void USER_IMPL::ProcessNewMonth()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);
//  Reset traff
if (connected)
    {
    Disconnect(true, "fake");
    }
DIR_TRAFF zeroTarff;

WriteMonthStat();

up = zeroTarff;
down = zeroTarff;

if (connected)
    {
    Connect(true);
    }

//  Set new tariff
if (nextTariff.ConstData() != "")
    {
    const TARIFF * nt;
    nt = tariffs->FindByName(nextTariff);
    if (nt == NULL)
        {
        WriteServLog("Cannot change tariff for user %s. Tariff %s not exist.",
                     login.c_str(), property.tariffName.Get().c_str());
        }
    else
        {
        property.tariffName.Set(nextTariff, sysAdmin, login, store);
        //tariff = nt;
        }
    ResetNextTariff();
    WriteConf();
    }
}
//-----------------------------------------------------------------------------
void USER_IMPL::ProcessDayFeeSpread()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (passive.ConstData() || tariff == NULL)
    return;

double fee = tariff->GetFee() / DaysInCurrentMonth();

if (std::fabs(fee) < 1.0e-3)
    return;

double c = cash;
switch (settings->GetFeeChargeType())
    {
    case 0:
        property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
        break;
    case 1:
        if (c + credit >= 0)
            property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
        break;
    case 2:
        if (c + credit >= fee)
            property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
        break;
    case 3:
        if (c >= 0)
            property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
        break;
    }
ResetPassiveTime();
}
//-----------------------------------------------------------------------------
void USER_IMPL::ProcessDayFee()
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (tariff == NULL)
    return;

double passiveTimePart = 1.0;
if (!settings->GetFullFee())
    {
    passiveTimePart = GetPassiveTimePart();
    }
else
    {
    if (passive.ConstData())
        {
        printfd(__FILE__, "Don't charge fee `cause we are passive\n");
        return;
        }
    }
double fee = tariff->GetFee() * passiveTimePart;

ResetPassiveTime();

if (std::fabs(fee) < 1.0e-3)
    {
    SetPrepaidTraff();
    return;
    }

double c = cash;
printfd(__FILE__, "login: %8s Cash=%f Credit=%f  Fee=%f PassiveTimePart=%f fee=%f\n",
        login.c_str(),
        cash.ConstData(),
        credit.ConstData(),
        tariff->GetFee(),
        passiveTimePart,
        fee);
switch (settings->GetFeeChargeType())
    {
    case 0:
        property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
        SetPrepaidTraff();
        break;
    case 1:
        if (c + credit >= 0)
            {
            property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
            SetPrepaidTraff();
            }
        break;
    case 2:
        if (c + credit >= fee)
            {
            property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
            SetPrepaidTraff();
            }
        break;
    case 3:
        if (c >= 0)
            {
            property.cash.Set(c - fee, sysAdmin, login, store, "Subscriber fee charge");
            SetPrepaidTraff();
            }
        break;
    }
}
//-----------------------------------------------------------------------------
void USER_IMPL::SetPrepaidTraff()
{
if (tariff != NULL)
    property.freeMb.Set(tariff->GetFree(), sysAdmin, login, store, "Prepaid traffic");
}
//-----------------------------------------------------------------------------
int USER_IMPL::AddMessage(STG_MSG * msg)
{
STG_LOCKER lock(&mutex, __FILE__, __LINE__);

if (SendMessage(*msg))
    {
    if (store->AddMessage(msg, login))
        {
        errorStr = store->GetStrError();
        WriteServLog("Error adding message: '%s'", errorStr.c_str());
        printfd(__FILE__, "Error adding message: '%s'\n", errorStr.c_str());
        return -1;
        }
    messages.push_back(*msg);
    }
else
    {
    if (msg->header.repeat > 0)
        {
        msg->header.repeat--;
        #ifndef DEBUG
        //TODO: gcc v. 4.x generate ICE on x86_64
        msg->header.lastSendTime = static_cast<int>(time(NULL));
        #else
        msg->header.lastSendTime = static_cast<int>(stgTime);
        #endif
        if (store->AddMessage(msg, login))
            {
            errorStr = store->GetStrError();
            WriteServLog("Error adding repeatable message: '%s'", errorStr.c_str());
            printfd(__FILE__, "Error adding repeatable message: '%s'\n", errorStr.c_str());
            return -1;
            }
        messages.push_back(*msg);
        }
    }
return 0;
}
//-----------------------------------------------------------------------------
int USER_IMPL::SendMessage(STG_MSG & msg) const
{
// No lock `cause we are already locked from caller
int ret = -1;
std::set<const AUTH*>::iterator it(authorizedBy.begin());
while (it != authorizedBy.end())
    {
    if (!(*it++)->SendMessage(msg, currIP))
        ret = 0;
    }
if (!ret)
    {
#ifndef DEBUG
    //TODO: gcc v. 4.x generate ICE on x86_64
    msg.header.lastSendTime = static_cast<int>(time(NULL));
#else
    msg.header.lastSendTime = static_cast<int>(stgTime);
#endif
    msg.header.repeat--;
    }
return ret;
}
//-----------------------------------------------------------------------------
void USER_IMPL::ScanMessage()
{
// No lock `cause we are already locked from caller
// We need not check for the authorizedBy `cause it has already checked by caller

std::list<STG_MSG>::iterator it(messages.begin());
while (it != messages.end())
    {
    if (settings->GetMessageTimeout() > 0 &&
        difftime(stgTime, it->header.creationTime) > settings->GetMessageTimeout())
        {
        // Timeout exceeded
        if (store->DelMessage(it->header.id, login))
            {
            WriteServLog("Error deleting message: '%s'", store->GetStrError().c_str());
            printfd(__FILE__, "Error deleting message: '%s'\n", store->GetStrError().c_str());
            }
        messages.erase(it++);
        continue;
        }
    if (it->GetNextSendTime() <= stgTime)
        {
        if (SendMessage(*it))
            {
            // We need to check all messages in queue for timeout
            ++it;
            continue;
            }
        if (it->header.repeat < 0)
            {
            if (store->DelMessage(it->header.id, login))
                {
                WriteServLog("Error deleting message: '%s'", store->GetStrError().c_str());
                printfd(__FILE__, "Error deleting message: '%s'\n", store->GetStrError().c_str());
                }
            messages.erase(it++);
            }
        else
            {
            if (store->EditMessage(*it, login))
                {
                WriteServLog("Error modifying message: '%s'", store->GetStrError().c_str());
                printfd(__FILE__, "Error modifying message: '%s'\n", store->GetStrError().c_str());
                }
            ++it;
            }
        }
    else
        {
        ++it;
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHG_PASSIVE_NOTIFIER::Notify(const int & oldPassive, const int & newPassive)
{
if (newPassive && !oldPassive && user->tariff != NULL)
    user->property.cash.Set(user->cash - user->tariff->GetPassiveCost(),
                            user->sysAdmin,
                            user->login,
                            user->store,
                            "Freeze");
}
//-----------------------------------------------------------------------------
void CHG_DISABLED_NOTIFIER::Notify(const int & oldValue, const int & newValue)
{
if (oldValue && !newValue && user->GetConnected())
    {
    user->Disconnect(false, "disabled");
    }
else if (!oldValue && newValue && user->IsInetable())
    {
    user->Connect(false);
    }

}
//-----------------------------------------------------------------------------
void CHG_TARIFF_NOTIFIER::Notify(const std::string &, const std::string & newTariff)
{
if (user->settings->GetReconnectOnTariffChange() && user->connected)
    user->Disconnect(false, "Change tariff");
user->tariff = user->tariffs->FindByName(newTariff);
if (user->settings->GetReconnectOnTariffChange() &&
    !user->authorizedBy.empty() &&
    user->IsInetable())
    user->Connect(false);
}
//-----------------------------------------------------------------------------
void CHG_CASH_NOTIFIER::Notify(const double & oldCash, const double & newCash)
{
user->lastCashAddTime = *const_cast<time_t *>(&stgTime);
user->lastCashAdd = newCash - oldCash;
}
//-----------------------------------------------------------------------------
void CHG_IPS_NOTIFIER::Notify(const USER_IPS & from, const USER_IPS & to)
{
    printfd(__FILE__, "Change IP from '%s' to '%s'\n", from.GetIpStr().c_str(), to.GetIpStr().c_str());
    if (user->connected)
        user->Disconnect(false, "Change IP");
    if (!user->authorizedBy.empty() && user->IsInetable())
        user->Connect(false);
}
//-----------------------------------------------------------------------------
