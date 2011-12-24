#ifndef __USERS_METHODS_H__
#define __USERS_METHODS_H__

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/registry.hpp>

class RPC_CONFIG;
class ADMINS;
class TARIFFS;
class USERS;
class STORE;
struct IP_MASK;

class METHOD_USER_GET : public xmlrpc_c::method {
public:
    METHOD_USER_GET(RPC_CONFIG * c,
                    USERS * u)
        : config(c),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_GET(const METHOD_USER_GET & rvalue);
    METHOD_USER_GET & operator=(const METHOD_USER_GET & rvalue);

    RPC_CONFIG * config;
    USERS * users;
};

class METHOD_USER_ADD : public xmlrpc_c::method {
public:
    METHOD_USER_ADD(RPC_CONFIG * c,
                    ADMINS * a,
                    USERS * u)
        : config(c),
          admins(a),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_ADD(const METHOD_USER_ADD & rvalue);
    METHOD_USER_ADD & operator=(const METHOD_USER_ADD & rvalue);

    RPC_CONFIG * config;
    ADMINS * admins;
    USERS * users;
};

class METHOD_USER_DEL : public xmlrpc_c::method {
public:
    METHOD_USER_DEL(RPC_CONFIG * c,
                    ADMINS * a,
                    USERS * u)
        : config(c),
          admins(a),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_DEL(const METHOD_USER_DEL & rvalue);
    METHOD_USER_DEL & operator=(const METHOD_USER_DEL & rvalue);

    RPC_CONFIG * config;
    ADMINS * admins;
    USERS * users;
};

class METHOD_USERS_GET : public xmlrpc_c::method {
public:
    METHOD_USERS_GET(RPC_CONFIG * c,
                     USERS * u)
        : config(c),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USERS_GET(const METHOD_USERS_GET & rvalue);
    METHOD_USERS_GET & operator=(const METHOD_USERS_GET & rvalue);

    RPC_CONFIG * config;
    USERS * users;
};

class METHOD_USER_CHG : public xmlrpc_c::method {
public:
    METHOD_USER_CHG(RPC_CONFIG * c,
                    ADMINS * a,
                    TARIFFS * t,
                    STORE * s,
                    USERS * u)
        : config(c),
          admins(a),
          tariffs(t),
          store(s),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_CHG(const METHOD_USER_CHG & rvalue);
    METHOD_USER_CHG & operator=(const METHOD_USER_CHG & rvalue);

    RPC_CONFIG * config;
    ADMINS * admins;
    TARIFFS * tariffs;
    STORE * store;
    USERS * users;
};

class METHOD_USER_CASH_ADD : public xmlrpc_c::method {
public:
    METHOD_USER_CASH_ADD(RPC_CONFIG * c,
                         ADMINS * a,
                         STORE * s,
                         USERS * u)
        : config(c),
          admins(a),
          store(s),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_CASH_ADD(const METHOD_USER_CASH_ADD & rvalue);
    METHOD_USER_CASH_ADD & operator=(const METHOD_USER_CASH_ADD & rvalue);

    RPC_CONFIG * config;
    ADMINS * admins;
    STORE * store;
    USERS * users;
};

class METHOD_USER_CASH_SET : public xmlrpc_c::method {
public:
    METHOD_USER_CASH_SET(RPC_CONFIG * c,
                         ADMINS * a,
                         STORE * s,
                         USERS * u)
        : config(c),
          admins(a),
          store(s),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_CASH_SET(const METHOD_USER_CASH_SET & rvalue);
    METHOD_USER_CASH_SET & operator=(const METHOD_USER_CASH_SET & rvalue);

    RPC_CONFIG * config;
    ADMINS * admins;
    STORE * store;
    USERS * users;
};

class METHOD_USER_TARIFF_CHANGE : public xmlrpc_c::method {
public:
    METHOD_USER_TARIFF_CHANGE(RPC_CONFIG * c,
                              ADMINS * a,
                              TARIFFS * t,
                              STORE * s,
                              USERS * u)
        : config(c),
          admins(a),
          tariffs(t),
          store(s),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_USER_TARIFF_CHANGE(const METHOD_USER_TARIFF_CHANGE & rvalue);
    METHOD_USER_TARIFF_CHANGE & operator=(const METHOD_USER_TARIFF_CHANGE & rvalue);

    RPC_CONFIG * config;
    ADMINS * admins;
    TARIFFS * tariffs;
    STORE * store;
    USERS * users;
};

class METHOD_GET_ONLINE_IPS : public xmlrpc_c::method {
public:
    METHOD_GET_ONLINE_IPS(RPC_CONFIG * c,
                          USERS * u)
        : config(c),
          users(u)
    {
    }

    void execute(xmlrpc_c::paramList const & paramList,
                 xmlrpc_c::value *   const   retvalP);

private:
    METHOD_GET_ONLINE_IPS(const METHOD_GET_ONLINE_IPS & rvalue);
    METHOD_GET_ONLINE_IPS & operator=(const METHOD_GET_ONLINE_IPS & rvalue);

    RPC_CONFIG * config;
    USERS * users;

    bool ParseNet(const std::string & net, IP_MASK & ipm) const;
};

#endif
