int DbmwClient::DoReceive()
{
    // b28d-L3: DIFF/N82+N89 — ORIG 0x... 帧 0x12c + 成员 store 段
    // ORIG @0x08120dbc：select → 读 18B 头 → msg_no 分支：
    //   1 = 服务端断开 ack（Close 后返 1）；payloadLen<0x1b 时读 payload，
    //   payload 前 4 字节写入 affectedRow。
    // N78 判词：ORIG readfds 清零/置位为 bmp[32] int 槽循环（无 rep stos），
    // 4.4-O0 的 FD_ZERO/FD_SET 恒内联成 rep stos（P116/P168 平台墙），语义等价豁免；
    // 非符号位域 shr$0x5 读取同以语义等价口径收。
    // S1(2026-09-11) 语义轴修复：ORIG select 超时为**成员** +0x14/+0x18
    // （0x08120dcb `movl $0x2,0x14(%eax)`；0x08120dd5 `movl $0x0,0x18(%eax)`；
    //  0x08120e3e `lea 0x14(%eax),%ecx` → select 第 5 实参）；原实现用局部
    // `struct timeval tv`（我方 `movl $0x2,-0x48(%ebp)`）⇒ 存储位置错，
    // 且 select 会回写剩余时间（成员化才对）。头 +0x14 m_timeout 已按 ORIG 定义。
    // codegen：ORIG 0x08120ddc..0x08120e08 清零为手写 bmp[0x20] 槽循环 + 指针槽
    // （`lea -0x10c(%ebp),%eax; mov %eax,-0x1c(%ebp)` … `cmpl $0x1f; setbe; jne`），
    // 非 glibc FD_ZERO 的 asm `cld; rep stosl`（我方 4.4 恒出 rep stos 且多两个
    // asm 输出约束残留槽）⇒ 按 ORIG 形手写。
    m_timeout.tv_sec = 2;
    m_timeout.tv_usec = 0;

    fd_set readfds;
    unsigned int* fdBits = reinterpret_cast<unsigned int*>(&readfds);
    for (unsigned int bi = 0; bi < 0x20; ++bi)
        fdBits[bi] = 0;
    // S1(2026-09-11)：ORIG 0x08120e0a..0x08120e34 的 FD_SET 是无符号形
    // （`shr $0x5` 取字槽 + `and $0x1f` 取位，15 条）。cast 到 unsigned 后与 ORIG
    // 同形同步数，本件 gap 由 +7 收敛到 **0**。
    // ⚠ 该旋钮曾在等待循环补齐前被旧口径误判为劣化：当时我方缺 ORIG 的 15 条
    // FD_ISSET 循环，缺口恰与该 cast 的 -8 条相抵，读数 +7→+15 看似变差。
    FD_SET((unsigned int)m_sock, &readfds);

    int ret = select(m_sock + 1, &readfds, 0, 0, &m_timeout);
    if (ret < 1)
    {
        // S1(2026-09-11) P0/P183 去具名局部：ORIG 本段**无 errno 缓存槽**，而是
        // 两次独立 `__errno_location` 展开（0x08120e79→strerror 实参、
        // 0x08120e8a→%d 实参），结果分别活在 callee-saved %ebx/%esi 跨 cMyTrace
        // 构造（`mov %eax,%ebx` / `mov (%eax),%esi`）。旧形 `int errNo = errno`
        // 引入 -0x1c 槽（我方 `mov %eax,-0x1c(%ebp)`）并只发一次调用。
        cMyTrace trace(__FUNCTION__, 0x25e, 0);
        trace("AccountDBMW select error : %d(%s)", errno, strerror(errno));
        Close();
        return 0;
    }

    // S1(2026-09-11) 语义缺口补齐：ORIG 在 select 成功后、读头之前有一段
    // **等 fd 就绪的 do-while 空体循环** 0x08120eea..0x08120f13
    // （`mov 0x8(%ebp),%eax; mov 0x8(%eax),%eax; shr $0x5,%eax;
    //   mov -0x10c(%ebp,%eax,4),%edx; ... sar %cl,%ebx; and $0x1,%eax;
    //   xor $0x1,%eax; test %al,%al; jne 0x08120eea`），
    // 且 `jg 0x08120eea` 直接跳进该循环 ⇒ 原实现整段缺失（我方 0x9ce 直接
    // 落到 headerLen）。语义等价于「select 返回后确认位图位已置」。
    while (!FD_ISSET((unsigned int)m_sock, &readfds))
        ;

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

    // S1(2026-09-11) P0/P183 去具名局部：ORIG 把 `m_recvBuf+2` 读了**两次**
    // （0x0812100a 比较用时 `mov 0x2(%eax),%eax; cmp $0x1a,%eax; jbe`；
    //  0x08121059 计算 payload 时再次 `mov 0x2(%eax),%eax; sub $0x12,%eax`），
    // 中间隔着 Close/logFormat 调用 ⇒ 无具名缓存槽（我方旧形 `mov %eax,-0x30
    // (%ebp)` 把 ORIG 缺的 2 条 load 省掉）。保持表达式直写。
    if (*reinterpret_cast<unsigned int*>(m_recvBuf + 2) >= 0x1b)
    {
        Close();
        LogManager::logFormat(1, "localchina/DbmwClient.cpp", __FUNCTION__,
                              0x284,
                              "AccountDBMW header packet size invalid!!");
        return 0;
    }

    int payload = (int)*reinterpret_cast<unsigned int*>(m_recvBuf + 2) - 0x12;
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
    // S1(2026-09-11) codegen/补 ORIG 段：ORIG 0x0812111d 起为 4.1 内联的 4 字节
    // 拷贝（base=m_recvBuf+0x12 → 指针槽 → 读 *p（死槽 -0x2c）→ `p+=4` → 读 *p
    // → 存 m_affectedRow(+0x30)，共 14 条）；我方 4.4 把 memcpy 发成 libcall（9 条）。
    // 改为显式指针形后本件 clean 233 = ORIG 233，**gap 归 0**。
    // （残余差仅为 4.1 内联骨架的 4 个指针槽 mov 与槽号，属跨版本 codegen。）
    unsigned int* pSrc = reinterpret_cast<unsigned int*>(m_recvBuf + 0x12);
    m_affectedRow = *++pSrc;
    return 2;
}