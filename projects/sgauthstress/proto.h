#ifndef __PROTO_H__
#define __PROTO_H__

#ifndef LINUX
#include <netinet/in_systm.h>
#endif
#include <netinet/ip.h>
#include <pthread.h>
#include <poll.h>

#include <string>
#include <list>
#include <vector>
#include <map>

#include "stg/os_int.h"
#include "stg/blowfish.h"

#include "user.h"

class PROTO;

typedef bool (PROTO::*PacketProcessor)(const void *, USER *);

class PROTO {
    public:
        PROTO(const std::string & server,
              uint16_t port,
              uint16_t localPort,
              int timeout = 1);
        ~PROTO();

        bool Start();
        bool Stop();

        const std::string GetStrError() const { return errorStr; }

        void AddUser(const USER & user, bool connect = false);

        bool Connect(uint32_t ip);
        bool Disconnect(uint32_t ip);

        size_t UserCount() const { return users.size(); }
    private:
        BLOWFISH_CTX ctx;
        struct sockaddr_in localAddr;
        struct sockaddr_in serverAddr;
        int timeout;

        std::list<std::pair<uint32_t, USER> > users;
        std::vector<struct pollfd> pollFds;

        bool running;
        bool stopped;

        pthread_t tid;
        pthread_mutex_t mutex;

        std::string errorStr;

        std::map<std::string, PacketProcessor> processors;

        static void * Runner(void * data);

        void Run();
        void CheckTimeouts();
        bool RecvPacket();
        bool SendPacket(const void * buffer, size_t length, USER * user);
        bool HandlePacket(const char * buffer, size_t length, USER * user);

        bool CONN_SYN_ACK_Proc(const void * buffer, USER * user);
        bool ALIVE_SYN_Proc(const void * buffer, USER * user);
        bool DISCONN_SYN_ACK_Proc(const void * buffer, USER * user);
        bool FIN_Proc(const void * buffer, USER * user);
        bool INFO_Proc(const void * buffer, USER * user);
        bool ERR_Proc(const void * buffer, USER * user);

        bool Send_CONN_SYN(USER * user);
        bool Send_CONN_ACK(USER * user);
        bool Send_DISCONN_SYN(USER * user);
        bool Send_DISCONN_ACK(USER * user);
        bool Send_ALIVE_ACK(USER * user);

        bool RealConnect(USER * user);
        bool RealDisconnect(USER * user);
};

#endif
