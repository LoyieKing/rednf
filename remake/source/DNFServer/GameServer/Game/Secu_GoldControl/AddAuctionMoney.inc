void Secu_GoldControl::AddAuctionMoney(unsigned int value)
{
    // P63：ORIG if 块出口统一 → 尾部 jmp;nop 中继；early-return 形使 4.4
    // 同产 jbe→中继（现形 jbe 直落 epilogue 少中继）。
    if (value <= 4999)
        return;
    {
        m_cur.m_auctionAdd += value;
        m_cur.m_count[0]++;
        m_total.m_auctionAdd += value;
        m_total.m_count[0]++;

        // ORIG 0x828793f/0x828794a：getHades() 三次 fresh call（P0 不缓存）。
        ((XNuclear::CHades*)m_pUser->getHades())->
            GoldPlus(value, (XNuclear::ENUM_GOLD_CONTROL_REASON)2);
        // GoldTracking
        char ipBuf[16];
        ((XNuclear::CHades*)m_pUser->getHades())->GetIP(ipBuf);

        // [b24ab] P197②：ORIG fatigue(→%esi)/accId(→%ebx) callee-saved 缓存跨 getHades。
        // fatigue 用 unsigned int 承接 unsigned short 返回值：ORIG 为单条
        // movzwl %ax,%esi（零扩展直存），声明成 unsigned short 会拆成 mov+movzwl 两步。
        register unsigned int fatigue = m_pUser->getCurCharacUsedFatigue();
        register unsigned int accId = m_pUser->get_acc_id();

        // ORIG 0x828793f..0x828794a：GoldTracking(type=4, money=accId, ip=ipBuf,
        // param4=value, fatigue=fatigue)
        ((XNuclear::CHades*)m_pUser->getHades())->
            GoldTracking(4, accId, ipBuf, value, fatigue);
    }
}