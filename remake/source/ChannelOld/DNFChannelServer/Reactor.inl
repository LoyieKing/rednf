#ifndef REACTOR_INL_
#define REACTOR_INL_

template <class TSession>
bool EpollReactor<TSession>::registHandle(TSession* s, unsigned int event_filter)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockReactor);
    epoll_event ev;
    memset(&ev, 0, 0xc);
    ev.events = getNativeEventFilter(event_filter);
    ev.data.ptr = s;
    if (s->getHandle() == -1 || epoll_fd_ == -1)
    {
        ChannelServiceApp::gFileLogError.Lock();
        ChannelServiceApp::gFileLogError << "[EpollReactor< TSession >::RegistHandle] Invalid variable : sock" << s->getHandle() << " epoll_fd(" << epoll_fd_ << ")" << endl;
        ChannelServiceApp::gFileLogError.Unlock();
        return false;
    }
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, s->getHandle(), &ev) < 0)
    {
        ChannelServiceApp::gFileLogError.Lock();
        ChannelServiceApp::gFileLogError << "[EpollReactor< TSession >::RegistHandle] Invalid variable : sock" << s->getHandle() << " epoll_fd(" << epoll_fd_ << ")" << endl;
        ChannelServiceApp::gFileLogError.Unlock();
        return false;
    }
    map_[s] = event_filter;
    return true;
}

template <class TSession>
bool EpollReactor<TSession>::unregistHandle(TSession* s)
{
    TScopedLock<TThreadLock<ThreadLock_linux> > slock(LockReactor);
    if (s->getHandle() == -1 || epoll_fd_ == -1)
    {
        return false;
    }
    epoll_event ev;
    memset(&ev, 0, 0xc);
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, s->getHandle(), &ev) < 0)
    {
        ChannelServiceApp::gFileLogError.Lock();
        ChannelServiceApp::gFileLogError << "[EpollReactor< TSession >::RegistHandle] Invalid variable : sock" << s->getHandle() << " epoll_fd(" << epoll_fd_ << ")" << endl;
        ChannelServiceApp::gFileLogError.Unlock();
        return false;
    }
    typename std::map<TSession*, unsigned int>::iterator iter = map_.find(s);
    map_.erase(iter);
    return true;
}

#endif // REACTOR_INL_
