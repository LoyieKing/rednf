int Dispatcher_BVHackInfo::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x08207568..0x08207851 (0x2e9 字节)。
    // 阈值常量 _DAT_08bd6360 = 50.0（已 objdump 核实）。

    if (user->get_state() <= 1)   // cmp $0x1 / setle
        return LineFunc(0xa216, __PRETTY_FUNCTION__, 0, 0);

    int code = 0;
    if (packet.get_int(code) != 1 || code != 0x40)
        return LineFunc(0xa21a, __PRETTY_FUNCTION__, 0, 0);

    char payload[0x40];   // -0x80
    if (packet.get_binary(payload, 0x40) != 1)
        return LineFunc(0xa21c, __PRETTY_FUNCTION__, 0, 0);

    stBVHackInfo_t info;   // -0x40（ORIG 至少 0x30 字节，6 个 double）
    if (decryptBVHackPacket(payload, info) != 1)
        return LineFunc(0xa224, __PRETTY_FUNCTION__, 0, 0);

    const double v0 = info.m_values[0];
    const double v1 = info.m_values[1];
    const double v2 = info.m_values[2];
    const double v3 = info.m_values[3];
    const double v4 = info.m_values[4];
    const double v5 = info.m_values[5];

    // 任一前五个值非零 → 记录日志（六个值按 +0x28..+0x00 顺序入 varargs）
    if (v0 != 0.0 || v1 != 0.0 || v2 != 0.0 || v3 != 0.0 || v4 != 0.0)
    {
        char* idStr = NumberToString(user->get_acc_id(), 0);
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0xa228,
            "(m_id: %s)(%5f)(%5f)(%5f)(%5f)(%5f)(%5f)",
            idStr, v5, v4, v3, v2, v1, v0);
    }

    if (v1 >= 50.0 && (v2 != 0.0 || v4 != 0.0))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x328, 1, 0, 0);
        if (user->get_state() != 5)
            user->SetTradePunishType(0x10);
    }
    return 0;
