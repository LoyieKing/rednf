int DbmwClient::DoReceive()
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 帧 0x12c + 成员 store 段
    // ORIG @0x08120dbc：select → 读 18B 头 → msg_no 分支：
    //   1 = 服务端断开 ack（Close 后返 1）；payloadLen<0x1b 时读 payload，
    //   payload 前 4 字节写入 affectedRow。
    // N78 判词：ORIG readfds 清零/置位为 bmp[32] int 槽循环（无 rep stos），
    // 4.4-O0 的 FD_ZERO/FD_SET 恒内联成 rep stos（P116/P168 平台墙），语义等价豁免；
    // 非符号位域 shr$0x5 读取同以语义等价口径收。
    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_sock, &readfds);

    int ret = select(m_sock + 1, &readfds, 0, 0, &tv);
    if (ret < 1)
    {
        int errNo = errno;
        cMyTrace trace(__FUNCTION__, 0x25e, 0);
        trace("AccountDBMW select error : %d(%s)", errNo, strerror(errNo));
        Close();
        return 0;
    }

    // 同上（ORIG 0x08120f3b/0x0812108a 两处 `mov -0x3c; cmp -0x38; je`）。
    int headerLen = 0x12;
    int got = ReadData(m_recvBuf, headerLen);
    if (got != headerLen)
    {
        if (got != -1)
        {
            cMyTrace trace(__FUNCTION__, 0x271, 0);
            trace("AccountDBMW header read size error : %d, expected : %d",
                  got, headerLen);
        }
        Close();
        return 0;
    }

    int msgNo = (int)(char)m_recvBuf[1];
    {
        cMyTrace trace(__FUNCTION__, 0x278, 0);
        trace("AccountDBMW read msg_no : %d", msgNo);
    }
    if (msgNo == 1)
    {
        Close();
        return 1;
    }

    unsigned int payloadLen =
        *reinterpret_cast<unsigned int*>(m_recvBuf + 2);
    if (payloadLen >= 0x1b)
    {
        Close();
        LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                              0x284,
                              "AccountDBMW header packet size invalid!!");
        return 0;
    }

    int payload = (int)payloadLen - 0x12;
    int payloadGot = ReadData(m_recvBuf + 0x12, payload);
    if (payload != payloadGot)
    {
        if (payloadGot != -1)
        {
            cMyTrace trace(__FUNCTION__, 0x290, 0);
            trace("AccountDBMW payload read size error:%d", payloadGot);
        }
        Close();
        return 0;
    }

    {
        cMyTrace trace(__FUNCTION__, 0x296, 0);
        trace("AccountDBMW payload read ok");
    }
    memcpy(&m_affectedRow, m_recvBuf + 0x16, sizeof(unsigned int));
    return 2;
