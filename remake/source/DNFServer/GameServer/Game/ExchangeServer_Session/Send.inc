// ---------------------------------------------------------------------------
// Send — ORIG 0x84906d0 size 0x97
// ---------------------------------------------------------------------------
bool CSession::Send(PacketGuard& guard)
{
    // ORIG 0x84906d0：test %al,%al + je（正向 if，logFormat 块为 je 目标后置，
    // P32：!f() 会生 xor $1 物化而 ORIG 无）。send_packet 失败路径独立 mov$0;jmp
    // 出口不经 logFormat。
    if (guard.is_finallized())
    {
        // ORIG 0x84906f1：get_len 结果升格 %ebx 跨 get_packet 调用存活（P52），
        // get_packet 结果进 eax 直接入参槽不落栈槽——单语句右到左实参求值形。
        int ret = send_packet(guard.get_packet(0), guard.get_len());
        if (ret < 0)
        {
            return 0;
        }
    }
    else
    {
        LogManager::logFormat(1, "ExchangeServerNetwork.h",
            // [M1Free76to90 2026-09-11 · P139] 原手写串缺返回类型/类限定；ORIG 局部静态
            // `_ZZN15exchange_server8CSession4SendER11PacketGuardE19__PRETTY_FUNCTION__`
            // @0x08c79b40 size **0x33** = `bool exchange_server::CSession::Send(PacketGuard&)`
            // （声明已是 bool ⇒ 展开逐字节一致）。
            __PRETTY_FUNCTION__, 0x228,
            "Packet.is_finallized return false");
        return 0;
    }
    return 1;                                 // ORIG L1: mov $1（块在 logFormat 后）
}