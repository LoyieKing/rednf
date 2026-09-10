int DisPatcher_IncreStatus::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user->get_state() <= 2)
    {
        return 0;
    }
    // [b28e-L5] P250：ORIG 载荷为 pack(1) 结构体局部（槽基 -0x2f、val@+0x0d，
    // `lea -0x2f(%ebp),%eax; add $0xd,%eax` 两步取址；0x081ede5a/0x081ede7c 等多处
    // `movzwl -0x22(%ebp),%eax` 即 m_val 重读）。
    IncreStatusSigView sig;
    if (!packet.get_short(sig.m_val))
    {
        return LineFunc(0x70fc, __PRETTY_FUNCTION__, 0, 0);
    }
    CItem* pItem = GlobalData::s_pSecuProtectionField->GetItemPtr(
        user, (char)0, (short)sig.m_val);
    if (pItem == 0)
    {
        return LineFunc(0x7101, __PRETTY_FUNCTION__, 0, 0);
    }
    int secu = GlobalData::s_pSecuProtectionField->CheckItem(
        user, static_cast<SECURITY_PROTCTION>(0x2a), pItem);
    if (secu != 0)
    {
        PacketGuard guard;
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x20, (unsigned char)secu, guard);
        return 0;
    }
    if (sig.m_val < 0x99)
    {
        user->increase_status((short)sig.m_val);
    }
    else
    {
        user->UseQuestItem((int)sig.m_val);
        // [b28e-L5] P227 全表达式临时链（ORIG 0x081ede9x 在 C1 call 前右起求 varargs
        // 并 `movswl %ax,%ebx` 跨 ctor 保活 ⇒ 单语句链式）。
        cMyTrace(__PRETTY_FUNCTION__, 0x710f, 5)("__ USE QUEST ITEM __ %d",
                                                 (int)sig.m_val);
    }
    return 0;
