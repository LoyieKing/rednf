// nsl::CommonDataPool.cpp
#include <assert.h>
#include <stdio.h>

#include "CommonDataPool.h"
#include "ServiceFactory.h"
#include "DataPool.h"
#include "Message.h"
#include "SendBuffer.h"
#include "TCPUser.h"
#include "Thread.h"

namespace nsl {

CommonDataPool::CommonDataPool()
{
    mSendMessagePool = new object_pool_by_boost_pool<Message, int, int, int>(0x20);
    mBufferSend = new object_pool_by_boost_pool<SendBuffer, int, int, int>(0x20);
    mMessagePool = new object_pool_by_boost_pool<Message, int, int, int>(0x20);
    mBufPool = new object_pool_by_boost_pool<DbBuffer, int, int, int>(0x20);
    pool_send_msg = 0;
    pool_send_buf = 0;
}

CommonDataPool::~CommonDataPool()
{
}

Message* CommonDataPool::getSendMessage(TCPUser* u)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mSendPoolLock);
    Message* msg = mSendMessagePool->construct();
    assert(msg && "mSendMessagePool");
    msg->setWorkId(tlsThreadId);
    msg->setUserToMessage(u);
    msg->setOnDataTypeMask(u->getSendDataType());
    SendBuffer* pSendBuffer = mBufferSend->malloc();
    if (pSendBuffer == NULL)
    {
        puts("wtf");
    }
    assert(pSendBuffer && "getSendMessage");
    msg->setStringToMessage(pSendBuffer);
    pool_send_msg = pool_send_msg + 1;
    pool_send_buf = pool_send_buf + 1;
    return msg;
}

void CommonDataPool::destroySendMessage(Message* msg)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mSendPoolLock);
    mBufferSend->free(msg->getSendBufferFromMessage());
    mSendMessagePool->free(msg);
    pool_send_msg = pool_send_msg - 1;
    pool_send_buf = pool_send_buf - 1;
}

Message* CommonDataPool::createMessage(int msgType)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mMsgPoolLock);
    Message* r = mMessagePool->construct();
    r->mMsgType = (char)msgType;
    DbBuffer* pBuf = mBufPool->malloc();
    assert(pBuf && "mBufPool");
    r->setStringToMessage(pBuf);
    return r;
}

void CommonDataPool::destroyMessage(Message* pMessage)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(mMsgPoolLock);
    mBufPool->free(pMessage->getDbBufferFromMessage());
    mMessagePool->free(pMessage);
}

} // namespace nsl
