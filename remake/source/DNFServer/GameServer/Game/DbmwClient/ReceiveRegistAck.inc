bool DbmwClient::ReceiveRegistAck()
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 帧 0x10c + 成员常量 store 段 vs 我方 0x...
    // ORIG @0x08120702：select(tv_sec=2) → 读 0x12 头 → msg_no==0 成功。
    // N78 判词：ORIG fd_set 清零为 bmp[32] int 槽循环（无 rep stos），4.4-O0 的
    // FD_ZERO/FD_SET 恒内联成 rep stos 展开（P116/P168 平台墙），语义等价豁免。
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
        cMyTrace trace(__FUNCTION__, 0xfd, 0);
        trace("AccountDBMW select error : %d(%s)", errNo, strerror(errNo));
        Close();
        return false;
    }

    // 2026-09-10（C05② 反向旋钮）：ORIG 0x0812087b 为 `mov -0x30(%ebp),%eax;
    //   cmp -0x2c(%ebp),%eax; je`（具名槽读 + mem 直比），我方 `const int` 被
    //   4.4 常量传播成 `cmpl $0x12,-0x2c(%ebp)` ⇒ 去 const 恢复槽读。
    int expected = 0x12;
    int got = ReadData(m_recvBuf, expected);
    if (got == expected)
    {
        int msgNo = (int)(char)m_recvBuf[1];
        {
            cMyTrace trace(__FUNCTION__, 0x117, 0);
            trace("AccountDBMW read msg_no : %d", msgNo);
        }
        if (msgNo == 0)
        {
            cMyTrace trace(__FUNCTION__, 0x11d, 0);
            trace("AccountDBMW regist Ok");
            return true;
        }
        Close();
        LogManager::logFormat(1, "localchina/DbmwClient.cpp",
                              __FUNCTION__, 0x123,
                              "AccountDBMW regist fail");
        return false;
    }
    else
    {
        if (got != -1)
        {
            cMyTrace trace(__FUNCTION__, 0x110, 0);
            trace("ReceiveRegistAck read size error : %d, expected : %d",
                  got, expected);
        }
        Close();
        return false;
    }
