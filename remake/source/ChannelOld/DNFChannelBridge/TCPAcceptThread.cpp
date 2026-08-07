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
        ret = NULL;
    }
    else
    {
        ret = queueAcceptedUser_.front();
        queueAcceptedUser_.pop();
        gFileLogInfo.Lock();
        gFileLogInfo << "lockPopAcceptedUser ++" << endl;
        gFileLogInfo.Unlock();
    }
    return ret;
}

void ChannelServiceApp::TCPAcceptThread::lockPushAcceptedUser(TCPUser* in_pUser)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueAcceptedUser_);
    queueAcceptedUser_.push(in_pUser);
    gFileLogInfo.Lock();
    gFileLogInfo << "lockPushAcceptedUser --" << endl;
    gFileLogInfo.Unlock();
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
        TCPSocket* s = pApp->UserPools::createTCPSocket("TCPAcceptThread.cpp", 0x4d);
        if (s == NULL)
        {
            gFileLogInfo.Lock();
            gFileLogInfo << "Create Error :  Remain TCP Sockets =" << pApp->UserPools::m_poolTCPSocket.getRemain() << endl;
            gFileLogInfo.Unlock();
        }
        else
        {
            bool bRet = listenSocket.accept(*s);
            if (bRet)
            {
                TCPUser* acUser = TManager<ChannelService>::getManager()->UserPools::createTCPUser("TCPAcceptThread.cpp", 0x61);
                if (acUser == NULL)
                {
                    gFileLogInfo.Lock();
                    gFileLogInfo << "Create Error :  Remain TCP Sockets =" << pApp->UserPools::m_poolTCPSocket.getRemain() << endl;
                    gFileLogInfo.Unlock();
                    gFileLogInfo.Lock();
                    gFileLogInfo << "Create Error :  Remain TCP Users =" << pApp->UserPools::m_poolTCPUser.getRemain() << endl;
                    gFileLogInfo.Unlock();
                    notifyCannotCreateUser(*s);
                    s->close();
                    TManager<ChannelService>::getManager()->UserPools::destroyTCPSocket(s);
                }
                else
                {
                    acUser->TManager<ChannelService>::setManager(TManager<ChannelService>::getManager());
                    acUser->setSocket(s);
                    acUser->startupAfterSetSocket();
                    acUser->setLastAccessTime();
                    lockPushAcceptedUser(acUser);
                }
            }
            else
            {
                gFileLogInfo.Lock();
                gFileLogInfo << "Create Error :  accept error" << strerror(*__errno_location()) << endl;
                gFileLogInfo.Unlock();
                TManager<ChannelService>::getManager()->UserPools::destroyTCPSocket(s);
            }
        }
    }
    listenSocket.close();
    setTerminated();
}
