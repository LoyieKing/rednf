int Inter_MonitorPayTaxToUpper::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax,eax; sete
        return 0;

    // ORIG 0x084d****：state 岛后、memberkey call 前 pPkt 才物化 -0x28（技巧13）。
    Inter_MonitorPayTaxToUpperPktViewI* pPkt = reinterpret_cast<Inter_MonitorPayTaxToUpperPktViewI*>(data);
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

    // ORIG 084ce38d `cmpl $0x0,-0x24(%ebp); jle`（有符号直比内存）⇒ fatigue 是 int，
    //   条件写作 `fatigue > 0`（带 (int) cast 会多一次载入，N22 形）。
    int fatigue = 0;
    bool capped = false;                               // 是否触达疲劳上限（回执 0x128 包）
    if (pPkt->m_16 != 0)
    {
        short memberBonus = user->getCurCharacMemberBonusFatigue();
        short accountBonus = user->getAccountMemberBonusFatigue();
        int maxCur = G_CDataManager()->m_serverParameter.m_488c;   // 当前角色奖励疲劳上限
        int maxAcc = G_CDataManager()->m_serverParameter.m_4890;   // 账号奖励疲劳上限
        // ORIG 084ce2fd/084ce300：maxAcc 落地（-0x14）后**立即**发 `movl $0x0,-0x10(%ebp)`，
        //   且 -0x10 正是 084ce359 `mov (%eax),%eax; mov %eax,-0x10(%ebp)`（max 返回值）的
        //   落点 ⇒ 原码在此处**声明并零初始化 n**，max 只做赋值（`n = std::max<int>(0, v);`），
        //   声明序在 maxAcc 之后、a/b 之前。
        int n = 0;
        int a = maxCur - memberBonus;
        int b = maxAcc - accountBonus;
        int v = std::min<int>(a, b);
        // ORIG 084ce345/084ce34c：`lea -0x38(%ebp)`（v）→0x4(%esp)、`lea -0x2c(%ebp)`（常量 0
        //   的临时）→(%esp) ⇒ 实参序是 max<int>(0, v)，非 (v, 0)。
        n = std::max<int>(0, v);
        if (n == 0)
        {
            fatigue = 0;
        }
        else if ((unsigned int)n > pPkt->m_16)
        {
            fatigue = pPkt->m_16;
        }
        else
        {
            fatigue = n;
            // ORIG 084ce3xx：`mov -0x10(%ebp),%eax; mov %eax,-0x24(%ebp)`（fatigue 落地）
            //   先于 `movb $0x1,-0x1d(%ebp)`（capped）⇒ 原码 fatigue 赋值在前。
            capped = true;
        }

        if (fatigue > 0)
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
}