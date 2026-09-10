int TCPSocket::send(char* buffer, unsigned int length)
{
    if (buffer == 0 || length == 0)
        return -1;

    int n = static_cast<int>(::write(m_socket, buffer, length));  // 0x081af6e0 write@plt
    int savedErrno = errno;                                       // 0x081af6e8-0x081af6f1

    // ORIG 0x081af6f6 `jg +0xb1`：n>0 块位于函数尾 leave 前岛（技巧23/24 goto 钉块序）
    if (n > 0)
        goto positive;
    if (savedErrno == EAGAIN || savedErrno == EINTR || savedErrno == EAGAIN)
    {
        ++m_sendRetryCount;                                       // 0x081af70a-0x081af716
        // [b28d-D43] P0/N96 清绑：原 register bool over asm("al") 删除（全域禁
        // 绑定）。ORIG 0x081af722 为 setg %al; test %al,%al; je（无槽往返）⇒ 按
        // P0「无槽 = 无具名局部」写裸条件。
        // [b28d-solo-nexon_cash] P225：ORIG `cmp $0x64; setg %al; test %al,%al;
        // je` 物化形 ⇒ 裸 if 在 4.4 折 `jle` 直跳；改 while+break 族
        // （body 末 return）即复现 setg 物化。
        while (m_sendRetryCount > static_cast<int>(TCPSocket::MAX_SEND_RETRY))
        {
            m_sendRetryCount = 0;                                 // 0x081af72c
            return -100;                                          // 0x081af733
        }
        return 0;                                                 // 0x081af73a
    }
    if (savedErrno == 0)                                          // 0x081af741
        return 0;                                                 // 0x081af747
    m_sendRetryCount = 0;                                         // 0x081af74e
    return -1;                                                    // 0x081af758
positive:
    m_sendRetryCount = 0;                                         // 0x081af75f-0x081af762
    return n;                                                     // 0x081af769
