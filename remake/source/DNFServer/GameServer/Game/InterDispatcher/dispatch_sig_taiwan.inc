// [b28d-lane2] verdict=DIFF ndiff=449（ORIG 0x084d6cda）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap-26。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=560（ORIG 0x84d6cda）：N18a 单段 regmove+N7 间接调用寄存器+N82/P112 帧槽平移。
int Inter_AuctionResultBidding::dispatch_sig_taiwan(CUser* user, char* data, int len)
{
    // [c3] ORIG 只保留一个 data 槽（-0x2c）且逐处 disp 直读 ⇒ 用引用视图（引用别名不占新槽）。
    const Inter_AuctionResultBiddingPktView& pkt =
        *reinterpret_cast<const Inter_AuctionResultBiddingPktView*>(data);
    (void)len;                                         // ORIG 未用 param_3 长度
    if (pkt.m_4 != static_cast<int>(user->getCurCharacNo()))   // msg+0x04 角色号
        return 0;

    {   // GM 延迟追踪（cmpl $0x4 setle；gmList = ORIG _ZL6gmList）
        for (int i = 0; i <= 4; ++i)
        {
            if (gmList[i] == static_cast<int>(user->getCurCharacNo()))
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
        pkt.m_c));        // msg byte @0xc
    if (static_cast<char>(pkt.m_c) == '\0')                   // 低字节为 0 补错误码
        packet.put_byte(static_cast<int>(
            pkt.m_d));    // byte @0xd
    packet.put_byte(pkt.m_0);                                 // msg+0x00 pay 类型

    const PrePayInfoView* prePay = 0;                  // local_2c（FindPrePayInfo 返回 &PrePayInfo，
                                                       // 本 TU 以 PrePayInfoView 消费，ORIG disp 直读形）
    Taiwan::BillTransaction trans1;                    // -0x60..-0x54 段（+0x10/+0x14/+0x18）
    Taiwan::BillTransaction trans2;                    // -0x64..-0x58 段（+0x1c/+0x20/+0x24）
    unsigned int amount1 = 0;                          // local_24（PrePayInfo+0x08）
    unsigned int amount2 = 0;                          // local_20（PrePayInfo+0x0c）
    WongWork::IPG::CIPGMgr* mgr = 0;                   // local_28

    if (pkt.m_0 == 1)
    {
        user->SetBuyingGold(false);
        prePay = reinterpret_cast<const PrePayInfoView*>(
            G_CPrePayInfoMgr()->FindPrePayInfo(
                static_cast<int>(user->getCurCharacNo())));
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

        // PrePayInfo +0x10..0x1b → trans1；+0x1c..0x27 → trans2（各 12B 整块，
        // ORIG 0x084d6fcd/0x084d6ff2 逐段 disp 4/4/4 展开）；+0x08/+0x0c → amount1/2。
        trans1 = prePay->m_tran1;
        trans2 = prePay->m_tran2;
        amount1 = prePay->m_amount1;
        amount2 = prePay->m_amount2;

        G_CPrePayInfoMgr()->DeletePrePayInfo(
            static_cast<int>(user->getCurCharacNo()));
    }

    if (static_cast<char>(pkt.m_c) == '\0')
    {
        if (pkt.m_0 == 1)
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
            const int money = pkt.m_8;
            const int gained = user->getCurCharacInvenW()->gain_money(
                money, static_cast<eMoneyAddReason>(0x14), true, 0);
            if (gained < money)
                user->SendMoneyFullReason(
                    static_cast<ENUM_MONEY_FULL_REASON>(0),
                    static_cast<unsigned long>(money),
                    static_cast<unsigned long>(gained));
        }
    }
    else if (pkt.m_0 == 1)
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
    if (pkt.m_0 == 1)
    {   // vtable+0x2c（slot12 IPGQuery(CUser*)）：类未建模——asm 桥残留保留，按 §8 协议待建模清理
        typedef void (*QueryFn)(void*, CUser*);
        // [Round7·2026-09-11 Main 点名] ORIG 0x84d73c8/0x84d73d4 对该虚调用发**两次**
        //   `mov &s_pIPGHelper,%eax`（取 vptr 一次、压 this 一次）；我方旧稿具名 `void* helper`
        //   只加载 1 次（槽 -0x10）⇒ 全函数引用数 3 vs ORIG 4。按 P0/P183 去具名局部内联
        //   （与同 TU 第 3627 行先例同形）。
        QueryFn query = *(QueryFn*)((*(void***)GlobalData::s_pIPGHelper)[0x2c / 4]);
        query(GlobalData::s_pIPGHelper, user);
    }
    return 0;
}