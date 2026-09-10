bool EpollHandler::init()
{
    m_fd = epoll_create(600);
    if (m_fd < 0)
    {
        std::cerr << "[Epoll::init] Can't init epoll create" << std::endl;
        return 0;
    }

    m_events = (epoll_event*)new char[0x1c20];
    if (m_events == NULL)
    {
        std::cerr << "[Epoll::init] Can't alloc event memory" << std::endl;
        return 0;
    }
    return 1;
