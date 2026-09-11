// ---------------------------------------------------------------------------
// Listen — ORIG 0x8490767 size 0xc2
// ---------------------------------------------------------------------------
CSession* CSession::Listen(int port, const char* ip)
{
    int sock;                                 // -0x20(%ebp)
    // [b28c-6][N7] AllocSession 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    // ORIG：静态调用 CreateListenSocket(&sock, port, ip)（无对象/无 this）；
    // 直接 !返回值 判断（ORIG xor $0x1,%eax; test %al,%al 形态）。
    if (!CNetwork<95000, 450000>::CreateListenSocket(sock, port, ip))
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x236, 0)(
            "CreateListenSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;                       // ORIG mov -0xc,%eax + jmp 岛
    }
    return 0;                                  // ORIG 0x849081e: mov $0,%eax（je 目标后置块）
}