#include "ChannelServiceApp.h"
#include "Socket.h"
#include <unistd.h>
#include <errno.h>

ChannelServiceApp::TCPAcceptThread::TCPAcceptThread()
{
}

ChannelServiceApp::TCPAcceptThread::~TCPAcceptThread()
{
}

ChannelServiceApp::TCPUser* ChannelServiceApp::TCPAcceptThread::lockPopAcceptedUser()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueAcceptedUser_);
    TCPUser* ret;
    if (queueAcceptedUser_.empty())
    {
        return NULL;
    }
    ret = queueAcceptedUser_.front();
    queueAcceptedUser_.pop();
    GLOG(gFileLogInfo, "lockPopAcceptedUser ++");
    return ret;
}

void ChannelServiceApp::TCPAcceptThread::lockPushAcceptedUser(TCPUser* in_pUser)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueAcceptedUser_);
    queueAcceptedUser_.push(in_pUser);
    GLOG(gFileLogInfo, "lockPushAcceptedUser --");
}

void ChannelServiceApp::TCPAcceptThread::notifyCannotCreateUser(TCPSocket& s)
{
}

void ChannelServiceApp::TCPAcceptThread::notifyCannotLoginByMaxUserCount(TCPSocket& s)
{
}

unsigned short ChannelServiceApp::TCPAcceptThread::getPort() const
{
    return nPort_;
}

void ChannelServiceApp::TCPAcceptThread::loop(void* temp)
{
    puts("Start up TCPAcceptThread");
    unsigned short accport = (unsigned short)getPort();
    ChannelService* pApp = TManager<ChannelService>::getManager();
    TCPSocket listenSocket;
    if (!listenSocket.open())
    {
        throw Exception("Fail to open socket");
    }
    if (!listenSocket.bind(accport, false))
    {
        throw Exception("Fail to bind listen");
    }
    if (!listenSocket.listen(0x400))
    {
        throw Exception("Fail to Create AcceptThread.");
    }
    while (!isTerminating())
    {
        usleep(100);
        if (isStop())
        {
            break;
        }
        sockaddr_in useradr;
        int size = 0x10;
        TCPSocket* s = TManager<ChannelService>::getManager()->UserPools::createTCPSocket("TCPAcceptThread.cpp", 0x4d);
        if (s == NULL)
        {
            GLOG(gFileLogInfo, "Create Error :  Remain TCP Sockets =" << pApp->UserPools::m_poolTCPSocket.getRemain());
            continue;
        }
        bool bRet = listenSocket.accept(*s);
        if (!bRet)
        {
            GLOG(gFileLogInfo, "Create Error :  accept error" << strerror(*__errno_location()));
            TManager<ChannelService>::getManager()->UserPools::destroyTCPSocket(s);
            continue;
        }
        TCPUser* acUser = TManager<ChannelService>::getManager()->UserPools::createTCPUser("TCPAcceptThread.cpp", 0x61);
        if (acUser == NULL)
        {
            GLOG(gFileLogInfo, "Create Error :  Remain TCP Sockets =" << pApp->UserPools::m_poolTCPSocket.getRemain());
            GLOG(gFileLogInfo, "Create Error :  Remain TCP Users =" << pApp->UserPools::m_poolTCPUser.getRemain());
            notifyCannotCreateUser(*s);
            s->close();
            TManager<ChannelService>::getManager()->UserPools::destroyTCPSocket(s);
            continue;
        }
        acUser->TManager<ChannelService>::setManager(TManager<ChannelService>::getManager());
        acUser->setSocket(s);
        acUser->startupAfterSetSocket();
        acUser->setLastAccessTime();
        lockPushAcceptedUser(acUser);
    }
    if (isTerminating())
    {
    }
    listenSocket.close();
    setTerminated();
}
