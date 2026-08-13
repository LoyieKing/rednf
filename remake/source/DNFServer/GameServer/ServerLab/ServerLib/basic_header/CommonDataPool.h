#ifndef NSL_COMMONDATAPOOL_H_
#define NSL_COMMONDATAPOOL_H_

#include "ObjectPoolByBoostPool.h"
#include "ThreadLock.h"

namespace nsl {

class Message;
class TCPUser;
class SendBuffer;
class DbBuffer;

class CommonDataPool
{
public:
    CommonDataPool();
    virtual ~CommonDataPool();
    virtual void destroyTimeEntity(void* pEntity) = 0;
    Message* getSendMessage(TCPUser* pUser);
    void destroySendMessage(Message* pMsg);
    Message* createMessage(int msgType);
    void destroyMessage(Message* pMsg);

    object_pool_by_boost_pool<Message, int, int, int>* mSendMessagePool;
    object_pool_by_boost_pool<SendBuffer, int, int, int>* mBufferSend;
    ThreadLock mSendPoolLock;
    object_pool_by_boost_pool<Message, int, int, int>* mMessagePool;
    object_pool_by_boost_pool<DbBuffer, int, int, int>* mBufPool;
    ThreadLock mMsgPoolLock;
    int pool_send_msg;
    int pool_send_buf;
};

} // namespace nsl

#endif // NSL_COMMONDATAPOOL_H_
