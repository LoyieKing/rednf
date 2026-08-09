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
    if (queueAcceptedUser_.empty())
    {
        return NULL;
    }
    TCPUser* ret = queueAcceptedUser_.front();
    queueAcceptedUser_.pop();
    return ret;
}

void ChannelServiceApp::TCPAcceptThread::lockPushAcceptedUser(TCPUser* in_pUser)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueAcceptedUser_);
    queueAcceptedUser_.push(in_pUser);
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
        gFileLogError.Lock();
        gFileLogError << "Fail to open socket" << endl;
        gFileLogError.Unlock();
        goto cleanup;
    }
    if (!listenSocket.bind(accport, false))
    {
        gFileLogError.Lock();
        gFileLogError << "Fail to bind listen" << endl;
        gFileLogError.Unlock();
        goto cleanup;
    }
    if (!listenSocket.listen(0x400))
    {
        gFileLogError.Lock();
        gFileLogError << "Fail to Create AcceptThread" << endl;
        gFileLogError.Unlock();
        goto cleanup;
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
        TCPSocket* s = TManager<ChannelService>::getManager()->UserPools::createTCPSocket("TCPAcceptThread.cpp", 0x4f);
        if (s == NULL)
        {
            GLOG(gFileLogInfo, "Create Error :  Remain TCP Sockets =" << pApp->UserPools::poolTCPSockets_.getRemain());
            continue;
        }
        bool bRet = listenSocket.accept(*s);
        if (!bRet)
        {
            GLOG(gFileLogInfo, "Create Error :  accept error" << strerror(*__errno_location()));
            TManager<ChannelService>::getManager()->UserPools::destroyTCPSocket(s);
            continue;
        }
        TCPUser* acUser = TManager<ChannelService>::getManager()->UserPools::createTCPUser("TCPAcceptThread.cpp", 0x62);
        if (acUser == NULL)
        {
            GLOG(gFileLogInfo, "Create Error :  Remain TCP Sockets =" << pApp->UserPools::poolTCPSockets_.getRemain());
            GLOG(gFileLogInfo, "Create Error :  Remain TCP Users =" << pApp->UserPools::poolTCPUsers_.getRemain());
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
cleanup:
    if (!isTerminating())
    {
        gFileLogError.Lock();
        gFileLogError << "[EXIT] th_AcceptThread Return" << endl;
        gFileLogError.Unlock();
    }
    listenSocket.close();
    setTerminated();
}
