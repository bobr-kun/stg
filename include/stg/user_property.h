/*
$Revision: 1.44 $
$Date: 2010/09/13 05:54:43 $
$Author: faust $
*/

#ifndef USER_PROPERTY_H
#define USER_PROPERTY_H

#include <unistd.h> // access

#include <ctime>
#include <string>
#include <set>
#include <sstream>
#include <iostream>

#include "stg/logger.h"
#include "stg/locker.h"
#include "stg/scriptexecuter.h"

#include "store.h"
#include "admin.h"
#include "notifer.h"
#include "noncopyable.h"

extern volatile time_t stgTime;

//-----------------------------------------------------------------------------
template<typename varT>
class USER_PROPERTY {
public:
    USER_PROPERTY(varT & val);
    virtual ~USER_PROPERTY();

    void Set(const varT & rvalue);

    USER_PROPERTY<varT> & operator= (const varT & rvalue);

    const varT * operator&() const throw() { return &value; }
    const varT & ConstData() const throw() { return value; }

    operator const varT&() const throw() { return value; }

    void    AddBeforeNotifier(PROPERTY_NOTIFIER_BASE<varT> * n);
    void    DelBeforeNotifier(const PROPERTY_NOTIFIER_BASE<varT> * n);

    void    AddAfterNotifier(PROPERTY_NOTIFIER_BASE<varT> * n);
    void    DelAfterNotifier(const PROPERTY_NOTIFIER_BASE<varT> * n);

    time_t  ModificationTime() const throw() { return modificationTime; }
    void    ModifyTime() throw();

private:
    varT & value;
    time_t modificationTime;
    std::set<PROPERTY_NOTIFIER_BASE<varT> *> beforeNotifiers;
    std::set<PROPERTY_NOTIFIER_BASE<varT> *> afterNotifiers;
    pthread_mutex_t mutex;
};
//-----------------------------------------------------------------------------
template<typename varT>
class USER_PROPERTY_LOGGED: public USER_PROPERTY<varT> {
public:
    USER_PROPERTY_LOGGED(varT & val,
                         const std::string & n,
                         bool isPassword,
                         bool isStat,
                         STG_LOGGER & logger,
                         const std::string & sd);
    virtual ~USER_PROPERTY_LOGGED() {}

    USER_PROPERTY_LOGGED<varT> * GetPointer() throw() { return this; }
    const varT & Get() const { return USER_PROPERTY<varT>::ConstData(); }
    const std::string & GetName() const { return name; }
    bool Set(const varT & val,
             const ADMIN * admin,
             const std::string & login,
             const STORE * store,
             const std::string & msg = "");
private:
    void WriteAccessDenied(const std::string & login,
                           const ADMIN * admin,
                           const std::string & parameter);

    void WriteSuccessChange(const std::string & login,
                            const ADMIN * admin,
                            const std::string & parameter,
                            const std::string & oldValue,
                            const std::string & newValue,
                            const std::string & msg,
                            const STORE * store);

    void OnChange(const std::string & login,
                  const std::string & paramName,
                  const std::string & oldValue,
                  const std::string & newValue,
                  const ADMIN  * admin);

    STG_LOGGER &      stgLogger;
    bool              isPassword;
    bool              isStat;
    std::string       name;
    const std::string scriptsDir;
};
//-----------------------------------------------------------------------------
class USER_PROPERTIES : private NONCOPYABLE {
/*
 В этом месте важен порядок следования приватной и открытой частей.
 Это связано с тем, что часть которая находится в публичной секции
 по сути является завуалированной ссылкой на закрытую часть. Т.о. нам нужно
 чтобы конструкторы из закрытой части вызвались раньше открытой. Поэтомому в
 начале идет закрытая секция
 * */

private:
    USER_STAT stat;
    USER_CONF conf;

public:
    USER_PROPERTIES(const std::string & sd);

    USER_STAT & Stat() { return stat; }
    USER_CONF & Conf() { return conf; }
    const USER_STAT & GetStat() const { return stat; }
    const USER_CONF & GetConf() const { return conf; }
    void SetStat(const USER_STAT & s) { stat = s; }
    void SetConf(const USER_CONF & c) { conf = c; }

    void SetProperties(const USER_PROPERTIES & p) { stat = p.stat; conf = p.conf; }

    USER_PROPERTY_LOGGED<double>            cash;
    USER_PROPERTY_LOGGED<DIR_TRAFF>         up;
    USER_PROPERTY_LOGGED<DIR_TRAFF>         down;
    USER_PROPERTY_LOGGED<double>            lastCashAdd;
    USER_PROPERTY_LOGGED<time_t>            passiveTime;
    USER_PROPERTY_LOGGED<time_t>            lastCashAddTime;
    USER_PROPERTY_LOGGED<double>            freeMb;
    USER_PROPERTY_LOGGED<time_t>            lastActivityTime;

    USER_PROPERTY_LOGGED<std::string>       password;
    USER_PROPERTY_LOGGED<int>               passive;
    USER_PROPERTY_LOGGED<int>               disabled;
    USER_PROPERTY_LOGGED<int>               disabledDetailStat;
    USER_PROPERTY_LOGGED<int>               alwaysOnline;
    USER_PROPERTY_LOGGED<std::string>       tariffName;
    USER_PROPERTY_LOGGED<std::string>       nextTariff;
    USER_PROPERTY_LOGGED<std::string>       address;
    USER_PROPERTY_LOGGED<std::string>       note;
    USER_PROPERTY_LOGGED<std::string>       group;
    USER_PROPERTY_LOGGED<std::string>       email;
    USER_PROPERTY_LOGGED<std::string>       phone;
    USER_PROPERTY_LOGGED<std::string>       realName;
    USER_PROPERTY_LOGGED<double>            credit;
    USER_PROPERTY_LOGGED<time_t>            creditExpire;
    USER_PROPERTY_LOGGED<USER_IPS>          ips;
    USER_PROPERTY_LOGGED<std::string>       userdata0;
    USER_PROPERTY_LOGGED<std::string>       userdata1;
    USER_PROPERTY_LOGGED<std::string>       userdata2;
    USER_PROPERTY_LOGGED<std::string>       userdata3;
    USER_PROPERTY_LOGGED<std::string>       userdata4;
    USER_PROPERTY_LOGGED<std::string>       userdata5;
    USER_PROPERTY_LOGGED<std::string>       userdata6;
    USER_PROPERTY_LOGGED<std::string>       userdata7;
    USER_PROPERTY_LOGGED<std::string>       userdata8;
    USER_PROPERTY_LOGGED<std::string>       userdata9;
};
//=============================================================================

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename varT>
inline
USER_PROPERTY<varT>::USER_PROPERTY(varT & val)
    : value(val),
      modificationTime(stgTime),
      beforeNotifiers(),
      afterNotifiers(),
      mutex()
{
pthread_mutex_init(&mutex, NULL);
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
USER_PROPERTY<varT>::~USER_PROPERTY()
{
pthread_mutex_destroy(&mutex);
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY<varT>::ModifyTime() throw()
{
modificationTime = stgTime;
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY<varT>::Set(const varT & rvalue)
{
STG_LOCKER locker(&mutex, __FILE__, __LINE__);

typename std::set<PROPERTY_NOTIFIER_BASE<varT> *>::iterator ni;

varT oldVal = value;

ni = beforeNotifiers.begin();
while (ni != beforeNotifiers.end())
    (*ni++)->Notify(oldVal, rvalue);

value = rvalue;
modificationTime = stgTime;

ni = afterNotifiers.begin();
while (ni != afterNotifiers.end())
    (*ni++)->Notify(oldVal, rvalue);
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
USER_PROPERTY<varT> & USER_PROPERTY<varT>::operator= (const varT & newValue)
{
Set(newValue);
return *this;
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY<varT>::AddBeforeNotifier(PROPERTY_NOTIFIER_BASE<varT> * n)
{
STG_LOCKER locker(&mutex, __FILE__, __LINE__);
beforeNotifiers.insert(n);
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY<varT>::DelBeforeNotifier(const PROPERTY_NOTIFIER_BASE<varT> * n)
{
STG_LOCKER locker(&mutex, __FILE__, __LINE__);
beforeNotifiers.erase(const_cast<PROPERTY_NOTIFIER_BASE<varT> *>(n));
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY<varT>::AddAfterNotifier(PROPERTY_NOTIFIER_BASE<varT> * n)
{
STG_LOCKER locker(&mutex, __FILE__, __LINE__);
afterNotifiers.insert(n);
}
//-----------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY<varT>::DelAfterNotifier(const PROPERTY_NOTIFIER_BASE<varT> * n)
{
STG_LOCKER locker(&mutex, __FILE__, __LINE__);
afterNotifiers.erase(const_cast<PROPERTY_NOTIFIER_BASE<varT> *>(n));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename varT>
inline
USER_PROPERTY_LOGGED<varT>::USER_PROPERTY_LOGGED(varT & val,
                                                 const std::string & n,
                                                 bool isPass,
                                                 bool isSt,
                                                 STG_LOGGER & logger,
                                                 const std::string & sd)

    : USER_PROPERTY<varT>(val),
      stgLogger(logger),
      isPassword(isPass),
      isStat(isSt),
      name(n),
      scriptsDir(sd)
{
}
//-------------------------------------------------------------------------
template <typename varT>
bool USER_PROPERTY_LOGGED<varT>::Set(const varT & val,
                                     const ADMIN * admin,
                                     const std::string & login,
                                     const STORE * store,
                                     const std::string & msg)
{
const PRIV * priv = admin->GetPriv();

if ((priv->userConf && !isStat) ||
    (priv->userStat && isStat) ||
    (priv->userPasswd && isPassword) ||
    (priv->userCash && name == "cash"))
    {
    std::stringstream oldVal;
    std::stringstream newVal;

    oldVal.flags(oldVal.flags() | std::ios::fixed);
    newVal.flags(newVal.flags() | std::ios::fixed);

    oldVal << USER_PROPERTY<varT>::ConstData();
    newVal << val;

    OnChange(login, name, oldVal.str(), newVal.str(), admin);

    if (isPassword)
        {
        WriteSuccessChange(login, admin, name, "******", "******", msg, store);
        }
    else
        {
        WriteSuccessChange(login, admin, name, oldVal.str(), newVal.str(), msg, store);
        }
    USER_PROPERTY<varT>::Set(val);
    return true;
    }
else
    {
    WriteAccessDenied(login, admin, name);
    return false;
    }
return true;
}
//-------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY_LOGGED<varT>::WriteAccessDenied(const std::string & login,
                                                   const ADMIN * admin,
                                                   const std::string & parameter)
{
stgLogger("%s Change user \'%s.\' Parameter \'%s\'. Access denied.",
          admin->GetLogStr().c_str(), login.c_str(), parameter.c_str());
}
//-------------------------------------------------------------------------
template <typename varT>
inline
void USER_PROPERTY_LOGGED<varT>::WriteSuccessChange(const std::string & login,
                                                    const ADMIN * admin,
                                                    const std::string & parameter,
                                                    const std::string & oldValue,
                                                    const std::string & newValue,
                                                    const std::string & msg,
                                                    const STORE * store)
{
stgLogger("%s User \'%s\': \'%s\' parameter changed from \'%s\' to \'%s\'. %s",
          admin->GetLogStr().c_str(),
          login.c_str(),
          parameter.c_str(),
          oldValue.c_str(),
          newValue.c_str(),
          msg.c_str());

store->WriteUserChgLog(login, admin->GetLogin(), admin->GetIP(), parameter, oldValue, newValue, msg);
}
//-------------------------------------------------------------------------
template <typename varT>
void USER_PROPERTY_LOGGED<varT>::OnChange(const std::string & login,
                                          const std::string & paramName,
                                          const std::string & oldValue,
                                          const std::string & newValue,
                                          const ADMIN * admin)
{
std::string filePath = scriptsDir + "/OnChange";

if (access(filePath.c_str(), X_OK) == 0)
    {
    std::string execString("\"" + filePath + "\" \"" + login + "\" \"" + paramName + "\" \"" + oldValue + "\" \"" + newValue + "\" \"" + admin->GetLogin() + "\" \"" + admin->GetIPStr() + "\"");
    ScriptExec(execString.c_str());
    }
else
    {
    stgLogger("Script OnChange cannot be executed. File %s not found.", filePath.c_str());
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
template<typename varT>
inline
std::ostream & operator<< (std::ostream & stream, const USER_PROPERTY<varT> & value)
{
return stream << value.ConstData();
}
//-----------------------------------------------------------------------------

#endif // USER_PROPERTY_H
