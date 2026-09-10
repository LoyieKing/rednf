int Dispatcher_CreatureSendMessage::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08204710 [0x08204710, 0x08204b0b]
    // 局部载荷区（-0x157 起）：+0x0d type / +0x0e target / +0x10 uint /
    // +0x14 msg2len / +0x18 msg2[0x1e] / +0x36 msg1len / +0x3a msg1[0x100]
    // §8.3 TU 局部 packed 视图具名（内联视图指针，零新增槽）。
    struct CSMsgView               // 自然对齐；0x36 非对齐 int 用 4 字节区+显式视图
    {
        char m_head[0xd];
        char m_type;               // +0x0d get_byte(char&)/send_chat_msg char 实参
        unsigned short m_target;   // +0x0e
        unsigned int m_field10;    // +0x10
        int m_msg2Len;             // +0x14
        char m_msg2[0x1e];         // +0x18
        unsigned char m_msg1LenB[4];  // +0x36（ORIG 非对齐 int；字节区宽 4，
                                      //  int 视图经 reinterpret 读写同 ORIG movl）
        char m_msg1[0x100];        // +0x3a
    };
    char buf[0x13a];
    // 内联视图（零新增局部槽；VEXP(f) ≡ buf 上具名字段）
