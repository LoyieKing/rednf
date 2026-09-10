int Dispatcher_UpgradeItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    if (user == 0 || user->CheckInTrade())
    {
        return 0;                                    // ORIG 空用户/交易中静默丢弃
    }
    if (!(user->get_state() == 3 && user->getCurCharacR() != 0))
    {
        return LineFunc(
            0x9518,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (user->CheckInTrade())                        // ORIG 二次检查
    {
        return 0;
    }

    MSG_ITEM_UPGRADE req;                            // ORIG ctor @0x0822dcea

    if (!packet.get_byte(req.m_upgradeType))   // +0x1a byte
    {
        return LineFunc(
            0x9530,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(req.m_targetSlot))   // +0x1b word（槽位）
    {
        return LineFunc(
            0x9532,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    // ORIG 0x0820082c 调 get_int(unsigned long&)（Rm）：按 ORIG 字段宽度取重载。
    if (!packet.get_int(reinterpret_cast<unsigned long&>(req.m_targetItemIdx)))  // +0x1d dword
    {
        return LineFunc(
            0x9534,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(req.m_materialSlot)) // +0x23 word
    {
        return LineFunc(
            0x9536,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_short(req.m_ticketSlot))   // +0x21 word
    {
        return LineFunc(
            0x9539,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    // ORIG 0x082008bc..0x08200957：-0x14=0x80（随后被 get_int 覆盖）、-0x18=0（dead）、
    // -0x99..-0x19 的 0x81 字节 dead 缓冲（内联对齐剥离慢路）。ORIG 无独立 memset(req+0x28)：
    // ctor 0x0822dcea 已 memset(this+0x28,0,0x81)，此处旧码的 memset 是 EXTRA（p167）。
    unsigned int strLen = 0x80;                      // ORIG -0x14
    int deadLen0 = 0;                                // ORIG -0x18（dead）
    char deadBuf[0x81] = {0};                        // ORIG -0x99..-0x19（dead）
    (void)deadLen0;
    (void)deadBuf;
    if (!packet.get_int(strLen))                     // 字符串长度前导
    {
        return LineFunc(
            0x9540,
            __PRETTY_FUNCTION__,
            0, 0);
    }
    if (!packet.get_str(req.m_28, (int)sizeof(req.m_28), (int)strLen))
    {
        return LineFunc(
            0x9542,
            __PRETTY_FUNCTION__,
            0, 0);
    }

    ServiceRestrictManager* restrictMgr = ARAD::Singleton<ServiceRestrictManager>::Get();
    if (restrictMgr->isRestricted(user, RestrictCategory::Enum_1, 7))
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53, 0xd1);
        return 0;
    }

    const unsigned int secu =
        GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x25);
    if (secu != 0)
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53,
                                      (unsigned short)(secu & 0xffff));
        return 0;
    }

    if (user->CheckItemLock(1, req.m_targetSlot))
    {
        user->SendCmdErrorPacket2Byte((ENUM_CMDPACKET)0x53, 0xd5);
        return 0;
    }

    G_CDataManager()->m_itemUpgrade->item_upgrade_system(user, &req, 0x53);
    return 0;
