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
    // ORIG 段直证：`call <T>; xor $0x1,%eax; test %al,%al; je`（布尔取反；`!= 1` 形退化
    // 为 `cmp $0x1; setne` 两条）。
    if (!decryptBVHackPacket(payload, info))
        return LineFunc(0xa224, __PRETTY_FUNCTION__, 0, 0);

    // 任一前五个值非零 → 记录日志（六个值按 +0x28..+0x00 顺序入 varargs）
    // ORIG 段直证：各比较直接 `fldl -0x40/-0x38/…(%ebp)`（info 本体槽）⇒ 删 v0..v5
    // 具名 double（原 6 具名令帧 +0x38，并多出 6 组 fldl/fstpl 槽搬运）。
    if (info.m_values[0] != 0.0 || info.m_values[1] != 0.0 ||
        info.m_values[2] != 0.0 || info.m_values[3] != 0.0 ||
        info.m_values[4] != 0.0)
    {
        char* idStr = NumberToString(user->get_acc_id(), 0);
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp", __PRETTY_FUNCTION__, 0xa228,
            "(m_id: %s)(%5f)(%5f)(%5f)(%5f)(%5f)(%5f)",
            idStr, info.m_values[5], info.m_values[4], info.m_values[3],
            info.m_values[2], info.m_values[1], info.m_values[0]);
    }

    if (info.m_values[1] >= 50.0 &&
        (info.m_values[2] != 0.0 || info.m_values[4] != 0.0))
    {
        user->getHackAnalyzer()->addServerHackCnt(
            user, (WongWork::ENUM_HACKTYPE)0x328, 1, 0, 0);
        if (user->get_state() != 5)
            user->SetTradePunishType(0x10);
    }
    return 0;
