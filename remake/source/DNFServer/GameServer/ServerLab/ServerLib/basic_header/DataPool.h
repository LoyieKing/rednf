#ifndef NSL_DATAPOOL_H_
#define NSL_DATAPOOL_H_

#include <boost/pool/object_pool.hpp>

#include "ThreadLock.h"
#include "ObjectPoolByBoostPool.h"

namespace nsl {

class Message;
class SendBuffer;
class TCPUser;
class TCPSocket;

typedef boost::object_pool<Message, boost::default_user_allocator_new_delete> TLogSendMessagePool;
typedef boost::object_pool<SendBuffer, boost::default_user_allocator_new_delete> TBufferLogSend;
typedef object_pool_by_boost_pool<TCPSocket, int, int, int> TTCPSocketPool;
typedef object_pool_by_boost_pool<TCPUser, int, int, int> TTCPUserPool;

class DataPool
{
public:
    DataPool();
    Message* getLogMessage(TCPUser* u);
    void destroyLogMessage(Message* msg);
    TCPSocket* createTCPSocket();
    void destroyTCPSocket(TCPSocket* pTCPSocket);
    TCPUser* createTCPUser();
    void destroyTCPUser(TCPUser* pTCPUser);
    unsigned int GetTcpUserCount();

    TTCPSocketPool* TCPSocketPool;
    TTCPUserPool* TCPUserPool;
    TLogSendMessagePool* LogSendMessagePool;
    TBufferLogSend* BufferLogSend;
    ThreadLock LogSendMsgLock;
    unsigned int mTcpUserCount;
};

} // namespace nsl

#endif // NSL_DATAPOOL_H_
