IDispatcher* PacketDispatcher::get_dispatcher(int code)
{
    if (code > 0x25d) return 0;
    if (code < 0) return 0;
    // ORIG 0x0859490b `test; je +0x38(null 尾岛)`：非空体作 fall-through（技巧17
    // 块序=ORIG fall 在前），null 返 0 置尾岛。
    if (m_dispatchers[code] != 0) return m_dispatchers[code];
    return 0;
