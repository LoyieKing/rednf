// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x081ea622 orig/ours=199/205 ndiff-blocks=14
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: movl   $0x1,-0x14(%ebp) | mov    &_ZN10GlobalData15s_event_managerE,%eax | mov    %eax,-0x10(%ebp)
int Dispatcher_Shop_Coin_Event::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x081ea622（PacketDispatcher_Impl_1.cpp，每日金币事件领取）
    if (user->get_state() <= 2)
    {
        return 0;
    }
    int coinLimit = 1;
    CCoinEventPerDay* event =
        (CCoinEventPerDay*)GlobalData::s_event_manager->GetRepeatEvent(4);
    if (event != 0 && event->IsEventing(0))   // 虚槽 +0x34（CEventBase::IsEventing）
    {
        coinLimit = event->GetCoinNoPerDay(1);
    }
    if (user->IsHavePremiumAdvantage())
    {
        coinLimit = user->GetPremiumInfo()->GetAdvantageCoin() + coinLimit;
    }
    unsigned int coin = user->getCurCharacInvenR()->GetCoin();
    bool open;
    if ((int)coin < coinLimit && user->GetOpencoin() != 1)
    {
        open = false;
    }
    else
    {
        open = true;
    }
    if (open)
    {
        PacketGuard guard;
        guard.put_header(1, 0xd2);
        guard.put_byte(1);
        guard.put_byte(0);
        guard.finalize(true);
        user->Send(guard);
        user->SetOpencoin(1);
        return 0;
    }
    int diff = coinLimit - (int)user->getCurCharacInvenR()->GetCoin();
    user->getCurCharacInvenW()->SetCoin((unsigned int)coinLimit);
    user->SetOpencoin(1);
    int curCoin = user->getCurCharacInvenR()->GetCoin();
    user->m_historyLog.CoinAdd(curCoin, diff, (eCoinAddReason)4);
    PacketGuard guard;
    guard.put_header(1, 0xd2);
    guard.put_byte(1);
    guard.put_byte(diff);
    guard.finalize(true);
    user->Send(guard);
    user->SendCoin();
    return 0;
}