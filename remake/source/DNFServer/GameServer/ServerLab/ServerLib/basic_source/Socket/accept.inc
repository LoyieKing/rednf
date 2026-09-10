bool TCPSocket::accept(TCPSocket& accepted)
{
    socklen_t len = 0x10;
    accepted.sock_ = ::accept(sock_, (sockaddr*)&accepted.adrs_, &len);
    if (accepted.sock_ == 0)
    {
        G_TraceLog()->sysLog(4, "accept() returned 0");
    }
    if (accepted.sock_ < 0 || accepted.sock_ == -1)
    {
        G_TraceLog()->sysLog(4, "Fail: accept(), errno: %d", errno);
        return false;
    }
    memcpy(accepted.c_adrs_, &accepted.adrs_.sin_addr, 4);
    accepted.port_ = accepted.adrs_.sin_port;
    accepted.setOptNonBlock();
    return true;
