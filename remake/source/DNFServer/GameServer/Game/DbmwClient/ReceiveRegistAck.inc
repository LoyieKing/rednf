bool DbmwClient::ReceiveRegistAck()
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 帧 0x10c + 成员常量 store 段 vs 我方 0x...
    // ORIG @0x08120702：select(tv_sec=2) → 读 0x12 头 → msg_no==0 成功。
    // N78 判词：ORIG fd_set 清零为 bmp[32] int 槽循环（无 rep stos），4.4-O0 的
    // FD_ZERO/FD_SET 恒内联成 rep stos 展开（P116/P168 平台墙），语义等价豁免。
    // S1(2026-09-11) 语义轴修复：ORIG 的 select 超时是**成员** +0x14/+0x18
    // （0x08120dcb `movl $0x2,0x14(%eax)`；0x08120dd5 `movl $0x0,0x18(%eax)`；
    //  0x08120e3e `lea 0x14(%eax),%ecx` → select 第 5 实参），
    // 原实现用局部 `struct timeval tv`（我方 `movl $0x2,-0x48(%ebp)`）⇒ 存储位
    // 置错（且 select 会回写剩余时间，成员化才对）。头 DbmwClient.h +0x14
    // m_timeout 已按 ORIG 布局定义，此处直接用。
    m_timeout.tv_sec = 2;
    m_timeout.tv_usec = 0;

    fd_set readfds;
    // S1(2026-09-11) codegen：ORIG 0x08120ddc..0x08120e08 的清零为**手写
    // bmp[0x20] 槽循环 + 指针槽**（`lea -0x10c(%ebp),%eax; mov %eax,-0x1c(%ebp)`
    // / `movl $0x0,-0x20(%ebp)` / 循环体 `mov -0x20(%ebp),%edx; mov -0x1c(%ebp),
    // %eax; movl $0x0,(%eax,%edx,4)` / `cmpl $0x1f; setbe; test; jne`），
    // 而非 glibc FD_ZERO 的 asm `cld; rep stosl`（我方 4.4 恒出 rep stos +
    // 两个 asm 输出约束残留槽）。按 ORIG 形手写。
    // S1(2026-09-11)：去具名指针、直接对 &readfds 做类型转换 —— 比具名指针形少 1 条
    // （174 对 175），朝向 ORIG 的 `mov -0x20(%ebp),%edx; mov -0x1c(%ebp),%eax;
    // movl $0,(%eax,%edx,4)` scaled-index 形收敛。
    for (unsigned int bi = 0; bi < 0x20; ++bi)
        reinterpret_cast<unsigned int*>(&readfds)[bi] = 0;
    // S1(2026-09-11)：ORIG 0x08120750..0x0812077a 的 FD_SET 是无符号形
    // （`shr $0x5` + `and $0x1f`，15 条）；cast 后我方与 ORIG 同形同步数
    // （该结论在 DoReceive 上先被旧口径误判为劣化，补齐等待循环后复测成立）。
    FD_SET((unsigned int)m_sock, &readfds);

    int ret = select(m_sock + 1, &readfds, 0, 0, &m_timeout);
    if (ret < 1)
    {
        // S1(2026-09-11) P0/P183 去具名局部：ORIG 0x081207bf 段无 errno 缓存槽，
        // 两次独立 `__errno_location` 展开（strerror 实参 + %d 实参），结果分别
        // 活于 callee-saved %ebx/%esi 跨 cMyTrace 构造。
        cMyTrace trace(__FUNCTION__, 0xfd, 0);
        trace("AccountDBMW select error : %d(%s)", errno, strerror(errno));
        Close();
        return false;
    }

    // S1(2026-09-11) 语义缺口补齐：ORIG 0x0812082a..0x08120853 是 select 成功后的
    // **FD_ISSET do-while 空体等待循环**（`jg 0x0812082a` 直跳入循环体，
    // `sar %cl,%ebx; and $0x1,%eax; xor $0x1,%eax; test %al,%al; jne 0x0812082a`），
    // 原实现整段缺失（补齐后本件 our 160→173）。
    while (!FD_ISSET((unsigned int)m_sock, &readfds))
        ;

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
}