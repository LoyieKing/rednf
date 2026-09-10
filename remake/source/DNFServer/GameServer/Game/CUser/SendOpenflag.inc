void CUser::SendOpenflag()
{
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
