bool TCPSocket::listen(int back_log)
{
    // ORIG: listen → shr $0x1f on eax; fail → close + return 0; else return 1
    if (::listen(sock_, back_log) < 0)
    {
        close();
        return false;
    }
    return true;
