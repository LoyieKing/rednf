int EpollHandler::SetEpoll(void* data, int fd, bool bET)
{
    if (bET)
    {
        m_event.events = 0x8000001d;
    }
    else
    {
        m_event.events = 0x1d;
    }
    m_event.data.ptr = data;

    Guard<Mutex> guard(&m_mutex);
    if (epoll_ctl(m_fd, EPOLL_CTL_ADD, fd, &m_event) < 0)
    {
        return errno;
    }
    return 0;
}