int Inter_AuctionResultBidding::dispatch_sig_taiwan(CUser* user, char* data, int len)
{
    Inter_AuctionResultBiddingPktView* pPkt = reinterpret_cast<Inter_AuctionResultBiddingPktView*>(data);
    (void)len;                                         // ORIG 未用 param_3 长度
    const int* msg =&pPkt->m_0;
    if (msg[1] != static_cast<int>(user->getCurCharacNo()))   // msg+0x04 角色号
        return 0;

    {   // GM 延迟追踪（cmpl $0x4 setle；g_auctionResultGmList = ORIG _ZL6gmList）
        for (int i = 0; i <= 4; ++i)
        {
            if (g_auctionResultGmList[i] == static_cast<int>(user->getCurCharacNo()))
            {
                cMyTrace(__PRETTY_FUNCTION__,
                    0x3c12, 0)("Trace Auction Delay, %s(%d)", "ACK AuctionResultBidding",
                      static_cast<int>(user->getCurCharacNo()));
                break;
            }
        }
    }

    PacketGuard packet;
    packet.put_header(1, 0xbc);
    packet.put_byte(static_cast<int>(
        pPkt->m_c));      // msg byte @0xc
    if (static_cast<char>(msg[3]) == '\0')                    // 低字节为 0 补错误码
        packet.put_byte(static_cast<int>(
            pPkt->m_d));  // byte @0xd
    packet.put_byte(msg[0]);                                  // msg+0x00 pay 类型

    PrePayInfo* prePay = 0;                            // local_2c（FindPrePayInfo 返回 &PrePayInfo）
    Taiwan::BillTransaction trans1;                    // -0x60..-0x54 段（+0x10/+0x14/+0x18）
    Taiwan::BillTransaction trans2;                    // -0x64..-0x58 段（+0x1c/+0x20/+0x24）
    unsigned int amount1 = 0;                          // local_24（PrePayInfo+0x08）
    unsigned int amount2 = 0;                          // local_20（PrePayInfo+0x0c）
    WongWork::IPG::CIPGMgr* mgr = 0;                   // local_28

    if (msg[0] == 1)
    {
        user->SetBuyingGold(false);
        prePay = G_CPrePayInfoMgr()->FindPrePayInfo(
            static_cast<int>(user->getCurCharacNo()));
        if (prePay == 0)
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x3c28, "PrePayInfo is null. character:%u",
                static_cast<unsigned int>(user->getCurCharacNo()));
            return 0;                                  // LAB_084d7407（guard 析构后返回）
        }
        // [b28d-solo-InterDispatcher] P282 槽序还原：ORIG 0x084d6ebe `add $0x3c`（vptr）⇒
        // _ZTVN8WongWork3IPG10CIPGHelperE @0x08b420a0 word17 = GetIPGMgr()（无参、仅压 this）。
        // 旧桥多传 characNo（p167 EXTRA getCurCharacNo）且绕过 vtable 派发。
        mgr = GlobalData::s_pIPGHelper->GetIPGMgr();

        // PrePayInfo +0x10/+0x14/+0x18 → trans1；+0x1c/+0x20/+0x24 → trans2
        trans1.m_tranNo = *reinterpret_cast<unsigned long long*>(
            reinterpret_cast<char*>(prePay) + 0x10);
        trans1.m_field0 = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(prePay) + 0x18);
        trans2.m_tranNo = *reinterpret_cast<unsigned long long*>(
            reinterpret_cast<char*>(prePay) + 0x1c);
        trans2.m_field0 = *reinterpret_cast<int*>(
            reinterpret_cast<char*>(prePay) + 0x24);
        amount1 = *reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(prePay) + 0x08);
        amount2 = *reinterpret_cast<unsigned int*>(
            reinterpret_cast<char*>(prePay) + 0x0c);

        G_CPrePayInfoMgr()->DeletePrePayInfo(
            static_cast<int>(user->getCurCharacNo()));
    }

    if (static_cast<char>(msg[3]) == '\0')
    {
        if (msg[0] == 1)
        {   // 失败：双段回滚（cancel）
            Taiwan::TaiwanCash* cash = mgr->getBillingCash();
            int rc = cash->cancel(trans1);
            if (CheckBillingErrorCode(rc))
                user->m_historyLog.BillingSuccess(
                    static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                    static_cast<int>(amount1));
            else
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x3c3c, "IPG_CANCEL user: %d, charac: %s reason: %d",
                    static_cast<unsigned int>(user->getCurCharacNo()),
                    user->getCurCharacName(), rc);
                user->m_historyLog.BillingFail(
                    static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                    static_cast<int>(amount1), rc, const_cast<char*>("rollback"));
            }
            cash = mgr->getBillingCash();
            rc = cash->cancel(trans2);
            if (CheckBillingErrorCode(rc))
                user->m_historyLog.BillingSuccess(
                    static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                    static_cast<int>(amount2));
            else
            {
                LogManager::logFormat(
                    1, "InterDispatcher.cpp",
                    __PRETTY_FUNCTION__,
                    0x3c47, "IPG_CANCEL user: %d, charac: %s reason: %d",
                    static_cast<unsigned int>(user->getCurCharacNo()),
                    user->getCurCharacName(), rc);
                user->m_historyLog.BillingFail(
                    static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                    static_cast<int>(amount2), rc, const_cast<char*>("rollback"));
            }
        }
        else
        {   // 成功退款：金币返还（msg+0x08 金额，reason 0x14）
            const int money = msg[2];
            const int gained = user->getCurCharacInvenW()->gain_money(
                money, static_cast<eMoneyAddReason>(0x14), true, 0);
            if (gained < money)
                user->SendMoneyFullReason(
                    static_cast<ENUM_MONEY_FULL_REASON>(0),
                    static_cast<unsigned long>(money),
                    static_cast<unsigned long>(gained));
        }
    }
    else if (msg[0] == 1)
    {   // 成功：双段确认（confirm）
        Taiwan::TaiwanCash* cash = mgr->getBillingCash();
        int rc = cash->confirm(trans1);
        if (CheckBillingErrorCode(rc))
            user->m_historyLog.BillingSuccess(
                static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                static_cast<int>(amount1));
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x3c5f, "IPG_CONFIRM user: %d, charac: %s reason: %d",
                static_cast<unsigned int>(user->getCurCharacNo()),
                user->getCurCharacName(), rc);
            user->m_historyLog.BillingFail(
                static_cast<ENUM_BILLING_TYPE>(2), trans1.m_tranNo,
                static_cast<int>(amount1), rc, const_cast<char*>("confirm"));
        }
        cash = mgr->getBillingCash();
        rc = cash->confirm(trans2);
        if (CheckBillingErrorCode(rc))
            user->m_historyLog.BillingSuccess(
                static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                static_cast<int>(amount2));
        else
        {
            LogManager::logFormat(
                1, "InterDispatcher.cpp",
                __PRETTY_FUNCTION__,
                0x3c6a, "IPG_CONFIRM user: %d, charac: %s reason: %d",
                static_cast<unsigned int>(user->getCurCharacNo()),
                user->getCurCharacName(), rc);
            user->m_historyLog.BillingFail(
                static_cast<ENUM_BILLING_TYPE>(2), trans2.m_tranNo,
                static_cast<int>(amount2), rc, const_cast<char*>("confirm"));
        }
    }

    packet.finalize(true);
    user->Send(packet);
    if (msg[0] == 1)
    {   // vtable+0x2c（slot12 IPGQuery(CUser*)）：类未建模——asm 桥残留保留，按 §8 协议待建模清理
        typedef void (*QueryFn)(void*, CUser*);
        void* helper = GlobalData::s_pIPGHelper;
        QueryFn query = *(QueryFn*)((*(void***)helper)[0x2c / 4]);
        query(helper, user);
    }
    return 0;
