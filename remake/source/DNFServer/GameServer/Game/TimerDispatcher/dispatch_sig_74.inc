int TimerNotResponseUserTimeout::dispatch_sig(int p1, int p2, unsigned int p3)
{
    CUser* user = G_CGameManager()->getUser(p1, 0xc);
    if (user == NULL)
        return 0;
    if ((unsigned short)user->get_unique_id() != p2)
        return 0;
    // N96/P0：uchar callee 无槽折叠形（probe /tmp/l7_m.cpp：4.4 恒折
    // cmpb$1 直判，xor 寄存器物化属 setcc-fold 负例）。
    if ((unsigned char)(user->IsRecvEvent() ^ 1))
    {
        // ORIG 0x086369e6：DisConnSig(0x1d, false, 0) 断线
        user->DisConnSig((DISCONN_SIG)0x1d, false, 0);
        // [b28d-solo-TimerDispatcher] ORIG 0x08636a1d..0x08636a34：0x14 字节缓冲
        // 以 5×`movl $0x0,-0x30..-0x20(%ebp)` 清零（4.1 对 20B 块按 dword 展开）
        // ⇒ 源形用 int[5] 具名缓冲（逐字节 movb 反劣），语义同为全零 IP 缓冲。
        int ipBuf[5] = {0};
        // [b28d-solo-TimerDispatcher2] ORIG `mov %eax,%ebx`（结果驻 %ebx 跨 cMyTrace
        // ctor，无栈槽）⇒ P231/P307 纯 register 存储类提示（非 asm 绑定，P0 合规）。
        // [G9WorldTimerLeague 2026-09-10 复核] N107 封顶确认：ORIG 0x08636a11
        // `lea 0xe0(%eax),%edx` 取 m_network 成员地址；我方 4.4 恒 `add $0xe0,%edx`
        // （成员常量偏移取址 add-vs-lea，N107 六形探针全产 add）⇒ 语义等价，勿再试。
        register const char* ipStr =
            user->m_network.GetPeerIP2((char*)ipBuf, 0x14);  // +0xe0
        cMyTrace(__PRETTY_FUNCTION__,
                 0xb73, 0)("Not response user timeout: idx(%d), IP(%s)", p1, ipStr);
    }
    return 1;
