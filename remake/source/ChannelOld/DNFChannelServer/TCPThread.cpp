#include "ChannelServiceApp.h"
#include "Socket.h"
#include "ScriptData.h"
#include "Script.h"
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

ChannelServiceApp::TCPThread::TCPThread()
{
    pHandler_ = NULL;
}

ChannelServiceApp::TCPThread::~TCPThread()
{
}

void ChannelServiceApp::TCPThread::lockPushConnectedUser(TCPUser* in_puser)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueConnectedUser_);
    queueConnectedUser_.push(in_puser);
}

ChannelServiceApp::TCPUser* ChannelServiceApp::TCPThread::lockPopConnectedUser()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueConnectedUser_);
    if (queueConnectedUser_.empty())
    {
        return NULL;
    }
    TCPUser* tmp = queueConnectedUser_.front();
    queueConnectedUser_.pop();
    return tmp;
}

void ChannelServiceApp::TCPThread::lockPushRequestConnect(int ident, char* file, int line)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueRequestConnect);
    queueRequestConnect.push(ident);
}

int ChannelServiceApp::TCPThread::lockPopRequestConnect()
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(lockQueueRequestConnect);
    if (queueRequestConnect.empty())
    {
        return -1;
    }
    int tmp = queueRequestConnect.front();
    queueRequestConnect.pop();
    return tmp;
}

bool ChannelServiceApp::TCPThread::DoPreWorkToStart()
{
    ChannelService* pApp = TManager<ChannelService>::getManager();
    TCPSocket* sTCP = pApp->UserPools::createTCPSocket("TCPThread.cpp", 0x42);
    if (sTCP == NULL)
    {
        gFileLogCri.Lock();
        gFileLogCri << "createTCPSocket failed." << endl;
        gFileLogCri.Unlock();
        return false;
    }
    if (!sTCP->open())
    {
        puts("failed to open UDP socket port");
        pApp->UserPools::destroyTCPSocket(sTCP);
        return false;
    }
    if (!sTCP->connect(G_ScriptData()->bridge_ip, (unsigned short)G_ScriptData()->bridge_port))
    {
        gFileLogInfo.Lock();
        gFileLogInfo << "failed to connect channel bridge server" << endl;
        gFileLogInfo.Unlock();
        sTCP->close();
        pApp->UserPools::destroyTCPSocket(sTCP);
        return false;
    }
    TCPUser* acUser = pApp->UserPools::createTCPUser("TCPThread.cpp", 0x5d);
    if (acUser == NULL)
    {
        sTCP->close();
        pApp->UserPools::destroyTCPSocket(sTCP);
        puts("cannot create TCP USER");
        return false;
    }
    acUser->startupAfterSetSocket();
    acUser->TManager<ChannelService>::setManager(pApp);
    acUser->setSocket(sTCP);
    acUser->onAccept();
    acUser->setACCID(G_ScriptData()->id);
    acUser->setLastAccessTime();
    TReactor<EpollReactor<TCPUser>, TCPUser>* r = pApp->getReactor();
    if (!r->registHandle(acUser, 7))
    {
        puts("register handle fail");
        return false;
    }
    lockPushConnectedUser(acUser);
    return true;
}
void ChannelServiceApp::TCPThread::loop(void* temp)
{
    puts("Start up TCPThread");
    ChannelService* pApp = TManager<ChannelService>::getManager();
    TReactor<EpollReactor<TCPUser>, TCPUser>* r = pApp->getReactor();
    r->init(5000);
    r->startup();
    int turn_of_idle_checkcount = 99999;
    bool retry_connection_success = true;
    mthreadId = pthread_self();
    bool bLoop = true;
    do
    {
        if (!bLoop)
        {
            r->shutdown();
            gFileLogInfo.Lock();
            gFileLogInfo << "shut down!!!" << endl;
            gFileLogInfo.Unlock();
            setTerminated();
            return;
        }
        gFileLogError.Lock();
        gFileLogError << "TCP Loop Start" << endl;
        gFileLogError.Unlock();
        pApp = TManager<ChannelService>::getManager();
        pApp->setTick();
        bool bInnerLoop = true;
        while (bInnerLoop)
        {
            pApp = TManager<ChannelService>::getManager();
            TCPAcceptThread* pAccept = pApp->getTCPAcceptThread();
            TCPUser* pUser = pAccept->lockPopAcceptedUser();
            if (pUser == NULL)
            {
                bInnerLoop = false;
                break;
            }
            pUser->onAccept();
            gFileLogError.Lock();
            gFileLogError << "On Accept()" << endl;
            gFileLogError.Unlock();
            if (!r->registHandle(pUser, 7))
            {
                pApp = TManager<ChannelService>::getManager();
                TScopedLock<TThreadLock<ThreadLock_linux> > slock(pApp->LockTCPUser);
                TCPSocket* s = pUser->getSocket();
                if (s != NULL)
                {
                    s->close();
                    pApp = TManager<ChannelService>::getManager();
                    pApp->UserPools::destroyTCPSocket(s);
                    gFileLogError.Lock();
                    gFileLogError << "Detroy Socket!!!!!!!!!" << endl;
                    gFileLogError.Unlock();
                }
                pUser->setSocket(NULL);
                pApp = TManager<ChannelService>::getManager();
                pApp->m_poolTCPUser.free(pUser);
                gFileLogError.Lock();
                gFileLogError << "In TCPThread : regist to Reactor was Failed" << endl;
                gFileLogError.Unlock();
            }
        }
        bool bVar1 = (turn_of_idle_checkcount < 0x65);
        turn_of_idle_checkcount = turn_of_idle_checkcount + 1;
        if (bVar1)
        {
            r->handleEvents(0, false);
        }
        else
        {
            r->handleEvents(0, true);
            turn_of_idle_checkcount = 0;
            int ident = lockPopRequestConnect();
            if ((0 < ident) || (retry_connection_success != true))
            {
                retry_connection_success = DoPreWorkToStart();
            }
        }
        usleep(400000);
        gFileLogError.Lock();
        gFileLogError << "TCP Loop End" << endl;
        gFileLogError.Unlock();
    } while (true);
}

template <class TSession>
bool EpollReactor<TSession>::handleEvents(unsigned int milisec, bool turn_of_idle)
{
    static int last_n_event = 0;
    int n_event = epoll_wait(epoll_fd_, events_, max_client_, (int)milisec);
    if ((((0 < n_event) && (last_n_event * 2 < n_event)) && (100 < n_event))
        || (((0 < n_event) && (n_event < last_n_event / 2)) && (100 < n_event)))
    {
        last_n_event = n_event;
        ChannelServiceApp::gFileLogInfo.Lock();
        ChannelServiceApp::gFileLogInfo << "last_n_event: " << last_n_event << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
    }
    if (n_event < 0)
    {
        ChannelServiceApp::gFileLogError.Lock();
        ChannelServiceApp::gFileLogError << "[EpollReactor< TSession >::HandleEvents] epoll_wait fail : errno=" << strerror(*__errno_location()) << endl;
        ChannelServiceApp::gFileLogError.Unlock();
        return false;
    }
    for (int i = 0; i < n_event; i++)
    {
        TSession* s = (TSession*)events_[i].data.ptr;
        if (s == NULL)
        {
            ChannelServiceApp::gFileLogInfo.Lock();
            ChannelServiceApp::gFileLogInfo << "************************TSession is NULL" << endl;
            ChannelServiceApp::gFileLogInfo.Unlock();
        }
        if ((events_[i].events & 8) == 0)
        {
            if ((events_[i].events & 0x10) == 0)
            {
                if ((events_[i].events & 1) != 0)
                {
                    s->onRead("Reactor.inl", 0xcc);
                    if (s->isDisconnected())
                    {
                        continue;
                    }
                }
                if (((events_[i].events & 4) == 0) || !s->isToWrite())
                {
                }
                else
                {
                    s->onWrite("Reactor.inl", 0xd7);
                    if (s->isDisconnected())
                    {
                        continue;
                    }
                }
                if (s->isAboutToDisconnect())
                {
                    ChannelServiceApp::gFileLogError.Lock();
                    ChannelServiceApp::gFileLogError << "isAboutToDisconnect \xb7\xce \xb2\xf7\xbe\xee\xc1\xf8\xb4\xd9" << endl;
                    ChannelServiceApp::gFileLogError.Unlock();
                    s->onClose("Reactor.inl", 0xe6);
                }
            }
            else
            {
                s->onClose("Reactor.inl", 0xc6);
            }
        }
        else
        {
            s->onClose("Reactor.inl", 0xc0);
        }
    }
    if (turn_of_idle)
    {
        typename std::map<TSession*, unsigned int>::iterator iter;
        iter = map_.begin();
        while (iter != map_.end())
        {
            if (iter->first == NULL)
            {
                goto done;
            }
            if (iter->first->isIdle())
            {
                break;
            }
            iter++;
        }
        ChannelServiceApp::gFileLogInfo.Lock();
        ChannelServiceApp::gFileLogInfo << "onClose!" << endl;
        ChannelServiceApp::gFileLogInfo.Unlock();
        iter->first->onClose("Reactor.inl", 0xf3);
    done:;
    }
    return true;
}

template <class TSession>
bool EpollReactor<TSession>::doSend(TSession* s)
{
    return s->isToWrite();
}

template class EpollReactor<ChannelServiceApp::TCPUser>;
template class TReactor<EpollReactor<ChannelServiceApp::TCPUser>, ChannelServiceApp::TCPUser>;
template class GlobalInstance<GLOBAL>;
template class GlobalInstance<Script>;
template class GlobalInstance<ScriptData>;

SOCKET ChannelServiceApp::TCPUser::getHandle()
{
    return pSock_->getHandle();
}
