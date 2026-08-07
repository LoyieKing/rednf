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
    }
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
    if (listenSocket.open())
    {
        if (listenSocket.bind(accport, false))
        {
            if (listenSocket.listen(0x400))
            {
                while (!isTerminating())
                {
                    usleep(100);
                    if (isStop())
                    {
                        break;
                    }
                    sockaddr_in useradr;
                    int size = 0x10;
                    ChannelService* pApp2 = TManager<ChannelService>::getManager();
                    TCPSocket* s = pApp2->UserPools::createTCPSocket("TCPAcceptThread.cpp", 0x4f);
                    if (s == NULL)
                    {
                        gFileLogInfo << "Create Error :  Remain TCP Sockets =" << pApp->UserPools::m_poolTCPSocket.getRemain() << endl;
                    }
                    else
                    {
                        bool bRet = listenSocket.accept(*s);
                        if (bRet)
                        {
                            ChannelService* pApp3 = TManager<ChannelService>::getManager();
                            TCPUser* acUser = pApp3->UserPools::createTCPUser("TCPAcceptThread.cpp", 0x62);
                            if (acUser == NULL)
                            {
                                gFileLogInfo << "Create Error :  Remain TCP Sockets =" << pApp->UserPools::m_poolTCPSocket.getRemain() << endl;
                                gFileLogInfo << "Create Error :  Remain TCP Users =" << pApp->UserPools::m_poolTCPUser.getRemain() << endl;
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
                            gFileLogInfo << "Create Error :  accept error" << strerror(*__errno_location()) << endl;
                            TManager<ChannelService>::getManager()->UserPools::destroyTCPSocket(s);
                        }
                    }
                }
            }
            else
            {
                gFileLogError << "Fail to Create AcceptThread" << endl;
            }
        }
        else
        {
            gFileLogError << "Fail to bind listen" << endl;
        }
    }
    else
    {
        gFileLogError << "Fail to open socket" << endl;
    }
    if (!isTerminating())
    {
        gFileLogError << "[EXIT] th_AcceptThread Return" << endl;
    }
    listenSocket.close();
    setTerminated();
}
