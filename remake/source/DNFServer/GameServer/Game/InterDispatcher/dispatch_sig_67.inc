#pragma pack(push, 1)
// P248 同型化：ORIG 084ce1xx 比较处发 4 行
//   `mov %eax,%edx; mov -0x28(%ebp),%eax; mov 0xe(%eax),%eax; cmp %eax,%edx`
//   ⇒ +0x0e 字段与 get_charac_no(int) 返回值失配 = unsigned int（探针 t3 实证）。
//   聚合器 Inter_MonitorPayTaxToUpperPktView.m_charID 为 int（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned int）。
struct Inter_MonitorPayTaxToUpperPktViewI
{
    char m_pad0[0xe];               // +0x0..未消费
    unsigned int m_charID;          // +0x0e
    // ORIG 084ce279 `mov 0x12(%eax),%eax; mov %eax,%ebx`（gain_money 前中转）
    //   ＝与 int 形参失配；084ce415 `put_int(pPkt->m_12)` 单段 ⇒ put_int 形参同为 unsigned
    //   （探针 t4/t6 实证）。
    unsigned int m_12;              // +0x12
    // ORIG 084ce36b `mov -0x10(%ebp),%edx; mov -0x28(%ebp),%eax; mov 0x16(%eax),%eax;
    //   cmp %eax,%edx; jbe` ⇒ 比较左=n、右=m_16 且为**异型**（故 gcc 不交换书写序）；
    //   探针 t5/t7：同型（unsigned）会归一化成先 m_16。故本字段为 int，条件写 n 在左。
    int m_16;                       // +0x16
    char m_1a[1];                   // +0x1a 串/字节区首元素（容量未建模）
};
#pragma pack(pop)

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
        int a = maxCur - memberBonus;
        int b = maxAcc - accountBonus;
        int v = std::min<int>(a, b);
        int n = std::max<int>(v, 0);
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
            capped = true;
            fatigue = n;
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
