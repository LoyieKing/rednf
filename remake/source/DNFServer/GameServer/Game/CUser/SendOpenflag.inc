void CUser::SendOpenflag()
{
    // 2026-09-10 gA（A 组）负结论 N（131 vs 133）：ORIG 0x086568fc 主体逐条同形
    //   （GetRepeatEvent 具名槽 -0xc + `cmpl $0x0,-0xc; je` 判空、两处局部 0 预置
    //   -0x14/-0x10、getCurCharacInvenR/GetCoin 无具名槽），残差为我方全帧槽号 -4 与
    //   +2 条搬运/对齐 ⇒ 槽分配/布局族，无源码级缺口可下刀，N 封顶。
    if (getCurCharacR() == 0)
        return;
    unsigned int coinNeed = 0;
    int lack = 0;
    CCoinEventPerDay* pEvent =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    if (pEvent != 0 && pEvent->IsEventing((CUser*)0) != 0)
        coinNeed = (unsigned int)pEvent->GetCoinNoPerDay(1);
    if (IsHavePremiumAdvantage())
        // ORIG 0x086568fc：GetPremiumInfo() 转发后 GetAdvantageCoin，不直读 m_premium
        coinNeed = coinNeed + (unsigned int)GetPremiumInfo()->GetAdvantageCoin();
    unsigned int coin = getCurCharacInvenR()->GetCoin();
    // b28d-lane5 复测（P0/P214①）：ORIG 0x08656a7x 该判据为**零槽** `mov $0x0,%eax;
    //   test %al,%al; je`（我方旧形 `mov %al,-0x11(%ebp); cmpb $0,-0x11` 多一具名
    //   bool 槽并致全帧 +0xc）⇒ 内联条件、删具名变量。
    if (!(coin < coinNeed && GetOpencoin() != (char)1))
    {
        lack = 0;
    }
    else
    {
        lack = (int)(coinNeed - getCurCharacInvenR()->GetCoin());
    }
    PacketGuard packet;
    ((InterfacePacketBuf*)&packet)->put_header(0, 0xb1);
    ((InterfacePacketBuf*)&packet)->put_byte((int)GetOpencoin());
    ((InterfacePacketBuf*)&packet)->put_int(lack);
    ((InterfacePacketBuf*)&packet)->finalize(true);
    Send(packet);
