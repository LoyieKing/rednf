CSession* CSession::OnAccept()
{
    sockaddr_in addr;                         // -0x50(%ebp)，len@-0x54
    socklen_t addrlen = 0x10;

    int fd = accept(GetSocket(), (sockaddr*)&addr, &addrlen);
    if (fd < 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x1f7, 0)("Accept Socket[%d] Error(%s)",
            fd, strerror(errno));
        return 0;                             // mov $0,%eax
    }

    unsigned short port = ntohs(addr.sin_port);
    char* ip = inet_ntoa(addr.sin_addr);
    cMyTrace(__PRETTY_FUNCTION__, 0x1fa, 0)(
        "Accepted other game server ip(%s), port(%d), sock(%d)",
        ip, (int)port, fd);

    // vtable+8 AllocSession()
    CSession* session = (CSession*)ES()->AllocSession();
    if (session != 0)
    {
        session->set_socket(fd, false);
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x202, 0)("session_pool_.Acquire() error");
    }
    return session;                           // ORIG 尾：mov -0xc,%eax
