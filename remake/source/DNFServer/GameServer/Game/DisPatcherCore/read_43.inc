int DisPatcher_GetItem::read(PacketBuf& packet, MSG_BASE& msg)
{
    // 载荷字段具名（MSG_GET_ITEM，PacketTypes.h:582 pack(1)；宽度按 ORIG 081c335c：
    //   get_shortERt @+0xd/+0x11/+0x13/+0x15/+0x1a/+0x1c/+0x18 → ushort 字段；
    //   get_byteERc @+0xf/+0x10 → uchar 字段以 char& 适配；
    //   081c34d9 `mov %al,0x17(%edx)` → uchar 字段单字节直写）
    // ORIG 0x81c34d9 `mov %al,0x17(%edx)` = QI 直存：bool lvalue = bool 返回（probe 实证，
    // uchar/char 混搭 RHS 必产 %dl 中转）。以函数内同布局视图承载 m_17=bool。
    // pack(1) 对齐 MSG_GET_ITEM（PacketTypes.h 同为 pack(push,1)）。
    #pragma pack(push, 1)
    struct View
    {
        char m_head[0xd];
        unsigned short m_0d;
        unsigned char m_0f;
        unsigned char m_10;
        unsigned short m_11;
        unsigned short m_13;
        unsigned short m_15;
        bool m_17;
        union { unsigned char m_pad18[2]; unsigned short m_18; };
        unsigned short m_1a;
        unsigned short m_1c;
    };
    #pragma pack(pop)
    static_assert(sizeof(View) == 0x1e, "View layout");
    View& req = *reinterpret_cast<View*>(&msg);
    if (!packet.get_short(req.m_0d))
    {
        return LineFunc(0xe2d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_0f)))
    {
        return LineFunc(0xe2f,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_byte(*reinterpret_cast<char*>(&req.m_10)))
    {
        return LineFunc(0xe30,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_11))
    {
        return LineFunc(0xe33,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_13))
    {
        return LineFunc(0xe34,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x15 get_short 不检失败；返回值（bool）低字节直写 +0x17
    // ORIG 0x081c34d9 `mov %al,0x17(%edx)`：bool lvalue = bool 返回（QI 直存，probe 实证）
    req.m_17 = packet.get_short(req.m_15);
    if (!packet.get_short(req.m_1a))
    {
        return LineFunc(0xe38,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    if (!packet.get_short(req.m_1c))
    {
        return LineFunc(0xe39,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    // +0x18 在权威头仍是 char m_pad18[2]（ORIG 此处 mov 宽度 2B = ushort）：
    // 已列「待 PacketTypes.h 权威字段 m_18(ushort) 落地」清单，暂以具名成员 +
    // 同宽度引用视图表达，不用裸偏移。
    if (!packet.get_short(*reinterpret_cast<unsigned short*>(req.m_pad18)))
    {
        return LineFunc(0xe3d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }
    return 0;
