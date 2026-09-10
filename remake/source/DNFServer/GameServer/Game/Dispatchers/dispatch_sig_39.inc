int Dispatcher_AssertManager::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0821e1f4：0x214 连续缓冲（Packet_Assert_Manager_Info 头 + 负载数据区）。
    // 负载布局视图（ORIG Ghidra：category@+0xa / desc[256]@+0xe / field10e@+0x10e
    // / field110@+0x110 / detail[0x100]@+0x114；总长 0x214 = sizeof 断言）。
    struct __attribute__((packed)) AssertPayloadView
    {
        char m_head[0xa];
        int m_category;             // +0x0a
        char m_desc[0x100];         // +0x0e
        unsigned short m_field10e;  // +0x10e
        int m_field110;             // +0x110
        char m_detail[0x100];       // +0x114
    };
    char buffer[0x214];
    Packet_Assert_Manager_Info* header = new (buffer) Packet_Assert_Manager_Info();
    (void)header;
    AssertPayloadView& view = *reinterpret_cast<AssertPayloadView*>(buffer);
    int& category = view.m_category;
    if (!packet.get_int(category)) {
        return LineFunc(0xd275, __PRETTY_FUNCTION__, 0, 0);
    }
    if (category > 0xff) {
        return LineFunc(0xd279, __PRETTY_FUNCTION__, 0, 0);
    }
    char* desc = view.m_desc;
    if (!packet.get_binary(desc, category)) {
        return LineFunc(0xd27a, __PRETTY_FUNCTION__, 0, 0);
    }
    unsigned short& field10e = view.m_field10e;
    if (!packet.get_short(field10e)) {
        return LineFunc(0xd27d, __PRETTY_FUNCTION__, 0, 0);
    }
    int& field110 = view.m_field110;
    if (!packet.get_int(field110)) {
        return LineFunc(0xd27f, __PRETTY_FUNCTION__, 0, 0);
    }
    if (field110 > 0xff || field110 < 0) {
        return LineFunc(0xd280, __PRETTY_FUNCTION__, 0, 0);
    }
    char* detail = view.m_detail;
    if (field110 > 0) {
        if (!packet.get_binary(detail, field110)) {
            return LineFunc(0xd283, __PRETTY_FUNCTION__, 0, 0);
        }
    }
    else {
        memset(detail, 0, 0x100);
    }
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(user->GetServerGroup());
    proxy->SendPacket(buffer, 0x214);
    return 0;
