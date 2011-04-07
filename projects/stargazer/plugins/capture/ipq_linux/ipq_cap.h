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
* Author : Boris Mikhailenko <stg34@stargazer.dp.ua>
*/

#ifndef IPQ_CAP_H
#define IPQ_CAP_H

#include <string>

#include "stg/plugin.h"
#include "stg/module_settings.h"
#include "stg/os_int.h"

#define BUFSIZE     (256)
#define PAYLOAD_LEN (96)

class USERS;
class TARIFFS;
class ADMINS;
class TRAFFCOUNTER;
class SETTINGS;

extern "C" PLUGIN * GetPlugin();

//-----------------------------------------------------------------------------
class IPQ_CAP :public PLUGIN {
public:
    IPQ_CAP();
    virtual ~IPQ_CAP() {}

    void SetUsers(USERS *) {}
    void SetTariffs(TARIFFS *) {}
    void SetAdmins(ADMINS *) {}
    void SetTraffcounter(TRAFFCOUNTER * tc) { traffCnt = tc; }
    void SetStore(STORE *) {}
    void SetStgSettings(const SETTINGS *) {}

    int Start();
    int Stop();
    int Reload() { return 0; }
    bool IsRunning() { return isRunning; }

    void  SetSettings(const MODULE_SETTINGS &) {}
    int  ParseSettings() { return 0; }
    const std::string & GetStrError() const { return errorStr; }
    const std::string GetVersion() const;
    uint16_t GetStartPosition() const { return 10; }
    uint16_t GetStopPosition() const { return 10; }

private:
    static void * Run(void *);
    int IPQCapOpen();
    int IPQCapClose();
    int IPQCapRead(void * buffer, int blen);

    struct ipq_handle * ipq_h;
    mutable std::string errorStr;

    pthread_t thread;
    bool nonstop;
    bool isRunning;
    int capSock;

    TRAFFCOUNTER * traffCnt;
    unsigned char buf[BUFSIZE];
};

#endif
