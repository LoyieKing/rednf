int DbmwClient::exec()
{
    // ORIG @0x081211c4：非查询类直接退出；未初始化/未连接先 Reconnect；
    // DoQuery 通过后 DoReceive，0 返回时再按连接状态重试一次。
    if (m_isSelect)
        return 0;

    if (!(IsInitialized() && IsConnected()))
    {
        if (!Reconnect())
            return 0;
    }

    if (!DoQuery())
    {
        if (!(IsInitialized() && IsConnected()))
        {
            LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                                  0x2f4,
                                  "AccountDBMW DoQuery() Error!! Query(%s) line(%d)",
                                  m_query, 0x2f4);
            if (!Reconnect())
                return -1;
            if (!DoQuery())
                return -1;
        }
        return 0;
    }

    int received = DoReceive();
    if (received == 0)
    {
        if (!(IsInitialized() && IsConnected()))
        {
            LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                                  0x306,
                                  "AccountDBMW DoReceive() Error!! Query(%s) line(%d)",
                                  m_query, 0x306);
            if (!Reconnect())
                return 0;
        }
        return 0;
    }
    if (received == 1)
        return -2;
    if (received == 2)
        return 1;
#if !remake_verify
    // [DNF-FIX 2026-09-19 · ud2 SIGILL] 仅 OFF 路线：ORIG 0x081213bc 在此是 `jmp 0x81213be`
    //   （跳 0，直落 epilogue），该路径 %eax 仍是 DoReceive 的返回值（中间分支不改 eax）
    //   ⇒ ORIG 事实行为 = `return received;`。GCC 13 对同一落底路径发 ud2 ⇒
    //   DoReceive 返回 >=3 时立即 SIGILL（DB 中间层异常返回会崩频道）。
    //   ON 分支文本保持原样（GCC 4.x 行为等价 ORIG）。
    return received;
#endif
}