// ============================================================================
// read —— ORIG 0x081d3c62
//   get_short(msg+0x0d) / get_int(msg+0x0f) / get_short(msg+0x13)
//   任一失败 → LineFunc 报错并返回该值（dispatch_template 将其作为 handler1 结果）。
// ============================================================================
int Dispatcher_UseLimitCube::read(PacketBuf& packet, MSG_BASE& msg)
{
    // 具名字段（MSG_USE_LIMIT_CUBE 布局见 PacketTypes.h + read.md 汇编：
    // get_short@msg+0xd / get_int@msg+0xf / get_short@msg+0x13）。
    // 调用点直接 cast 表达式：ORIG 为 `mov msg; add $N` 传址（无栈溢出），
    // O0 下引用局部变量会多 store/load 破坏 identical，故不引入具名引用。
    if (!packet.get_short(reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg)->m_slotA))
    {
        return LineFunc(0x337d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_int(reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg)->m_itemIdx))
    {
        return LineFunc(0x3381,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(reinterpret_cast<MSG_USE_LIMIT_CUBE*>(&msg)->m_slotB))
    {
        return LineFunc(0x3385,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
}