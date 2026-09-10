int Dispatcher_SaveCharacterOption::read(PacketBuf& packet, MSG_BASE& msg)
{
    // ORIG 0x081ce8bc：同 SaveGameOption_1/2 混合形——typed 指针槽 -0xc、
    // get_int 实参 add $0xd、len typed 单级回读。MSG 头 +0xd 无具名字段
    //（PacketTypes.h 冻结），本 TU 局部视图承接。视图须 packed：ORIG 回读形
    // `mov 0xd(%eax),%eax`（+0x0d 起 len），默认对齐会把 m_len 推到 +0x10。
    struct __attribute__((packed)) SaveCharacOptView {
        char m_head[0xd];
        unsigned int m_len;      // +0x0d
    };
    SaveCharacOptView* m = reinterpret_cast<SaveCharacOptView*>(&msg);
    if (!packet.get_int(*reinterpret_cast<int*>(reinterpret_cast<char*>(m) + 0xd)) ||
        m->m_len >= 0x201)
        return LineFunc(0x2829, __PRETTY_FUNCTION__, 0, 0);
    // [b28d-solo-Dispatchers] P0：ORIG 无 lv/pbuf 槽 ⇒ 删具名局部改内联实参
    //   （同 SGO_2 形，右→左求值先 len 后 buf）。
    // [b28d-solo-Dispatchers] P319：buf 实参整型加法 + 回 cast（探针 probe_gb2 d）。
    if (!packet.get_binary(
            reinterpret_cast<char*>(reinterpret_cast<unsigned int>(m) + 0x11),
            (int)m->m_len))
        return LineFunc(0x282b, __PRETTY_FUNCTION__, 0, 0);
    return 0;
