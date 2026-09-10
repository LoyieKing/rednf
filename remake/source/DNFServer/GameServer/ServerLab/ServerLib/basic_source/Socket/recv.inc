int TCPSocket::recv(char* buf, int size)
{
    if (buf == NULL || size < 1)
    {
        G_TraceLog()->sysLog(7, "In recv : recv buffer is null");
        return -1;
    }
    int nRead = (int)read(sock_, buf, size);
    if (nRead < 0)
    {
        // ORIG: separate __errno_location for each compare
        if ((errno == EAGAIN) || (errno == EINTR) || (errno == EAGAIN) || (errno == 0))
        {
            return 0;
        }
    }
    else if (nRead == 0)
    {
        G_TraceLog()->sysLog(8, "tcp recv : FIN recv, %s", strerror(errno));
        return -1;
    }
    G_TraceLog()->sysLog(8, "tcp recv ='%d'", nRead);
    return nRead;
