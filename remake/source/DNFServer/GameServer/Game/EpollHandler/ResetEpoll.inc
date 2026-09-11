int EpollHandler::ResetEpoll(int fd)
{
    memset(&m_event, 0, sizeof(m_event));
    m_event.events = EPOLLIN;

    Guard<Mutex> guard(&m_mutex);
    if (epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, &m_event) < 0)
    {
        return errno;
    }
    return 0;
}