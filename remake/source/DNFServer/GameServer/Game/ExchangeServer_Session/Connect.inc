CSession* CSession::Connect(const char* ip, int port)
{
    int sock;                                 // -0x24(%ebp)
    // [b28c-6][N7] AllocSession 虚调用槽寄存器 %eax↔%edx 互换（4.4 恒形），源码不可翻（ndiff 8）。
    // CreateConnectionSocket 为静态成员（ORIG 同）；返回非 0=失败码。
    const int ok = CNetwork<95000, 450000>::CreateConnectionSocket(sock, ip, port);

    // [N96 清绑] 原 `register bool failed asm("eax")` 已删（P0/N96 全域禁绑）。
    // ORIG 0x849084c `cmpl $0x0,-0x10(%ebp); setne %al; test %al,%al; je` ——
    // failed 值全程在 %al、无栈槽 ⇒ 按 P0 双推律原码无该具名局部，还原为
    // `if (ok != 0)` 表达式直判（残差见函数头判词）。
    if (ok != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x24b, 0)(
            "CreateConnectionSocket failed - %s", strerror(errno));
        return 0;
    }

    // vtable+8 AllocSession
    CSession* session = static_cast<CSession*>(ES()->AllocSession());
    if (session != 0)
    {
        session->set_socket(sock, true);
        return session;
    }
    return 0;                                  // ORIG 0x84908e7: mov $0,%eax
