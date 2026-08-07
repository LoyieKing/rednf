#include "TCPThread.h"
#include "ChannelServiceApp.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

ChannelServiceApp::TCPThread::TCPThread()
    : Thread(), TManager<ChannelService>()
{
    pHandler_ = NULL;
}

ChannelServiceApp::TCPThread::~TCPThread()
{
}

void ChannelServiceApp::TCPThread::loop(void* temp)
{
    puts("Start up TCPThread");
    ChannelService* pApp = getManager();
    TReactor<EpollReactor<TCPUser>, TCPUser>* r = pApp->getReactor();
    r->init(10000);
    r->startup();
    (void)pApp;
    while (true)
    {
        if (isTerminating())
        {
            break;
        }
        pApp = getManager();
        pApp->setTick();
        while (true)
        {
            pApp = getManager();
            TCPUser* pUser = pApp->getTCPAcceptThread()->lockPopAcceptedUser();
            if (pUser == NULL)
            {
                break;
            }
            pUser->onAccept();
            r->registHandle(pUser, 7);
        }
        r->handleEvents(1000);
        usleep(10000);
    }
    r->shutdown();
    setTerminated();
}

template <class TSession>
bool EpollReactor<TSession>::handleEvents(unsigned int milisec)
{
    int n_event = epoll_wait(epoll_fd_, events_, max_client_, (int)milisec);
    if (n_event != 0)
    {
        if (n_event < 0)
        {
            ChannelServiceApp::gFileLogError.Lock();
            ChannelServiceApp::gFileLogError << "[EpollReactor< TSession >::HandleEvents] epoll_wait fail : errno="
                                             << strerror(*__errno_location()) << endl;
            ChannelServiceApp::gFileLogError.Unlock();
            return false;
        }
        for (int i = 0; i < n_event; i++)
        {
            TSession* s = (TSession*)events_[i].data.ptr;
            if ((events_[i].events & 8) == 0)
            {
                if ((events_[i].events & 0x10) == 0)
                {
                    if ((events_[i].events & 1) != 0)
                    {
                        s->onRead("Reactor.inl", 0xa3);
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
                        s->onWrite("Reactor.inl", 0xab);
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
                        s->onClose("Reactor.inl", 0xb5);
                    }
                }
                else
                {
                    s->onClose("Reactor.inl", 0x9d);
                }
            }
            else
            {
                s->onClose("Reactor.inl", 0x97);
            }
        }
    }
    if (map_.size() != 0)
    {
        typename std::map<TSession*, unsigned int>::iterator iter = map_.begin();
        while (iter != map_.end())
        {
            TSession* s = iter->first;
            ++iter;
            if ((s != NULL) && s->isIdle())
            {
                s->onClose("Reactor.inl", 199);
                break;
            }
        }
    }
    return true;
}

template class EpollReactor<ChannelServiceApp::TCPUser>;
template class TReactor<EpollReactor<ChannelServiceApp::TCPUser>, ChannelServiceApp::TCPUser>;
