int TCPSocket::send(char* buf, int size)
{
    if (buf == NULL || size < 1)
    {
        G_TraceLog()->sysLog(0, "buf error or size-%d error", size);
        return -1;
    }
    int n_bytes = (int)write(sock_, buf, size);
    int error_number = errno;
    // ORIG: jg to success at end; error paths fall through first
    if (n_bytes < 1)
    {
        if ((error_number == 0xb) || (error_number == 4) || (error_number == 0xb))
        {
            if (error_number == 0xb)
            {
                G_TraceLog()->sysLog(0, "EAGAIN");
            }
            else if (error_number == 4)
            {
                G_TraceLog()->sysLog(0, "EINTR");
            }
            else if (error_number == 0xb)
            {
                G_TraceLog()->sysLog(0, "EWOULDBLOCK");
            }
            // ORIG：自增并判断写在条件里（重载成员 + setg %al; test %al,%al）
            if ((mSendRetryCount = mSendRetryCount + 1) > 0x64)
            {
                G_TraceLog()->sysLog(7, "So many retry. so disconnect him, %d.%d.%d.%d:%d",
                                     c_adrs_[0], c_adrs_[1], c_adrs_[2], c_adrs_[3], port_);
                mSendRetryCount = 0;
                return -100;
            }
            G_TraceLog()->sysLog(0, "tcp send retry='%d', error ='%s(%d)'",
                                 n_bytes, strerror(error_number), error_number);
            return 0;
        }
        else if (error_number == 0)
        {
            G_TraceLog()->sysLog(0, "if errno == 0 then Critcal Problem!! YOU MUST CHECK THIS!!!");
            return 0;
        }
        else
        {
            G_TraceLog()->sysLog(0, "tcp send fail='%d', error ='%s'", n_bytes, strerror(errno));
            mSendRetryCount = 0;
            return -1;
        }
    }
    mSendRetryCount = 0;
    return n_bytes;
