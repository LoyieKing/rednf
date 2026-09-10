int Dispatcher_SaveGameOption_QuickChatting::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081cf0a6：msg typed 指针槽 -0xc、len 无独立局部槽——
    // get_int 实参 add $0xd、比较/传参一律 `mov 0xd(%eax)` 回读（技巧14/32，
    // 同 SGO_1/2 形）。unsigned len 局部会占 -0x10 并破坏重载链。
    // 视图须 packed：ORIG 回读 `mov 0xd(%eax),%eax`，默认对齐会把 m_len 推到 +0x10。
    struct __attribute__((packed)) QuickChatView {
        char m_head[0xd];
        unsigned int m_len;      // +0x0d
        unsigned char m_flag11;  // +0x11（get_byte 目标；MSG_QUICK_CHAT::m_11 同位）
    };
    QuickChatView* m = reinterpret_cast<QuickChatView*>(&msg);
    if (!packet.get_byte(m->m_flag11))
        return LineFunc(0x290a, __PRETTY_FUNCTION__, 0, 0);
    if (!packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(m) + 0xd)) ||
        m->m_len >= 0x12d)
        return LineFunc(0x290b, __PRETTY_FUNCTION__, 0, 0);
    // [b28d-solo-Dispatchers] P0：ORIG 无 lv/pbuf 槽（值 %edx / buf 地址 %eax）
    //   ⇒ 删具名局部改内联实参。
    // [b28d-solo-Dispatchers] P319：buf 实参整型加法 + 回 cast（探针 probe_gb2 d）。
    if (!packet.get_binary(
            reinterpret_cast<char*>(reinterpret_cast<unsigned int>(m) + 0x12),
            (int)m->m_len))
        return LineFunc(0x290c, __PRETTY_FUNCTION__, 0, 0);
    return 0;
