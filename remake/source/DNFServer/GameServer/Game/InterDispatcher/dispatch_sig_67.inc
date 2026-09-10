int Inter_MonitorPayTaxToUpper::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084d****：state 岛后、memberkey call 前 pPkt 才物化 -0x28（技巧13）。
    Inter_MonitorPayTaxToUpperPktView* pPkt = reinterpret_cast<Inter_MonitorPayTaxToUpperPktView*>(data);
    if (user->get_charac_memberkey() == 0)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x29e7,
            "MEMBER : Inter_MonitorPayTaxToUpper::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
            user->get_charac_no(-1));
        return 0;
    }

    if (user->get_charac_no(-1) != pPkt->m_charID)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x29ed,
            "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1), pPkt->m_charID);
        return 0;
    }

    if (pPkt->m_12 != 0)     // 税额金币直接入账
    {
        user->getCurCharacInvenW()->gain_money(
            pPkt->m_12, (eMoneyAddReason)0xa, true, 0);
    }

    unsigned int fatigue = 0;
    bool capped = false;                               // 是否触达疲劳上限（回执 0x128 包）
    if (pPkt->m_16 != 0)
    {
        short memberBonus = user->getCurCharacMemberBonusFatigue();
        short accountBonus = user->getAccountMemberBonusFatigue();
        int maxCur = G_CDataManager()->m_serverParameter.m_488c;   // 当前角色奖励疲劳上限
        int maxAcc = G_CDataManager()->m_serverParameter.m_4890;   // 账号奖励疲劳上限
        int a = maxCur - memberBonus;
        int b = maxAcc - accountBonus;
        int v = std::min<int>(a, b);
        int n = std::max<int>(v, 0);
        if (n == 0)
        {
            fatigue = 0;
        }
        else if (pPkt->m_16 < (unsigned int)n)
        {
            fatigue = pPkt->m_16;
        }
        else
        {
            capped = true;
            fatigue = (unsigned int)n;
        }

        if ((int)fatigue > 0)
        {
            user->gainCurCharacFatigue((int)fatigue);
            user->incCurCharacMemberBonusFatigue((short)fatigue);
            user->EnableSaveMemberBonusFatigue();
            user->incAccountMemberBonusFatigue((short)fatigue);
        }
    }

    if (pPkt->m_12 != 0 || fatigue != 0)
    {
        PacketGuard packet;
        packet.put_header(0, 0x4c);
        packet.put_int(pPkt->m_12);
        packet.put_short((int)fatigue);
        const int strLen = strlen(pPkt->m_1a);
        packet.put_int(strLen);
        packet.put_str(pPkt->m_1a, strLen);
        packet.finalize(true);
        user->Send(packet);

        if (capped)
        {   // 奖励疲劳被上限截断 → 单独 0x128 回执
            PacketGuard capPacket;
            capPacket.put_header(0, 0x128);
            capPacket.finalize(true);
            user->Send(capPacket);
        }
    }
    return 0;
