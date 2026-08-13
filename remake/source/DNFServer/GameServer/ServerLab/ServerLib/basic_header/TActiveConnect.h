#ifndef NSL_TACTIVECONNECT_H_
#define NSL_TACTIVECONNECT_H_

#include <string.h>
#include <map>
#include <queue>

#include "ThreadLock.h"
#include "TCPUser.h"
#include "System.h"

namespace nsl {

class TCPUser;
class TCPSocket;

class ConInterface
{
public:
    void setInfo(int con_id, const char* remote_ip, int remote_port,
                 TCPUser::ENUM_DATA_TYPE send_data_type,
                 TCPUser::ENUM_DATA_TYPE recv_data_type, bool need_to_recon);
    int getId()
    {
        return con_id;
    }
    char* getIp()
    {
        return remote_ip;
    }
    int getPort()
    {
        return remote_port;
    }
    TCPUser::ENUM_DATA_TYPE getSendType()
    {
        return send_data_type;
    }
    TCPUser::ENUM_DATA_TYPE getRecvType()
    {
        return recv_data_type;
    }
    bool getNeedRecon()
    {
        return need_to_recon;
    }
    void SetConnected(bool bConnected)
    {
        mConnected = bConnected;
    }
    void setTCPUser(TCPUser* pUser)
    {
        this->pUser = pUser;
    }
    TCPUser* getTCPUser()
    {
        return pUser;
    }

    int con_id;
    char remote_ip[16];
    int remote_port;
    TCPUser::ENUM_DATA_TYPE send_data_type;
    TCPUser::ENUM_DATA_TYPE recv_data_type;
    bool need_to_recon;
    TCPUser* pUser;
    TCPSocket* pSocket;
    bool mIsRejected;
    bool mConnected;
};

typedef std::map<int, ConInterface*> TCPUserConnectMap;
typedef std::queue<ConInterface*, std::deque<ConInterface*> > RequestConnectQueue;

class ActiveConManager
{
public:
    ActiveConManager();
    void CheckTheConnection(ConInterface* conInfo);
    TCPUser* RequestConnect(ConInterface* conInfo);
    bool PopRequestConnect(TCPUser*& outConnectedUser);
    bool isRequestInQueue()
    {
        return bRequestInQueue;
    }

    bool bRequestInQueue;
    bool bConnectedInQueue;
    ThreadLock LockInCon;
    ThreadLock LockOutCon;
    TCPUserConnectMap mapConnectedUser_;
    RequestConnectQueue queueRequestConnect;
};

} // namespace nsl

#endif // NSL_TACTIVECONNECT_H_
