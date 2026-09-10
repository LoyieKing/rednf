bool CMailBoxHelper::_DoGetPackageCoin(CUser* user, CMailBox::CMail const* mail,
                                       std::vector<std::pair<int, int> >& items,
                                       bool bReset, ENUM_ERROR& err)
{
    // ORIG 0x08553672
    int coin = user->getCurCharacInvenW()->GetCoin();
    int eventCoin = user->getCurCharacInvenW()->GetEventCoin();
    int payCoin = user->getCurCharacInvenW()->GetPayCoin();
    if (coin + eventCoin + payCoin >= 0x186a1)
    {
        err = (ENUM_ERROR)0x4f;
        return false;
    }
    if (bReset)
        return true;

    // ORIG 0x085536fa：`lea -0x69(%ebp),%eax; call Inven_Item 拷贝构造` ⇒
    // 按值局部（非引用；引用局部会多一枚指针槽 + 每处指针解引用）。
    Inven_Item pkg = mail->GetPackageItem();
    // ORIG 决策链 cmp $1/cmp $2/test（无槽）⇒ switch；amount=pkg.m_addInfo2
    // （Inven_Item +0x07）、coinType=pkg.m_fieldb（+0x0b，ushort movzwl 两段）。
    // ORIG 历史日志第二参均为 getCurCharacInvenR()->GetEventCoin() 重取。
    switch (pkg.m_fieldb)
    {
    case 0:
        user->getCurCharacInvenW()->SetCoin(
            user->getCurCharacInvenR()->GetCoin() + pkg.m_addInfo2);
        user->m_historyLog.CoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)0);
        break;
    case 1:
        user->getCurCharacInvenW()->SetEventCoin(
            user->getCurCharacInvenR()->GetEventCoin() + pkg.m_addInfo2);
        user->m_historyLog.EventCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)2);
        break;
    case 2:
        user->getCurCharacInvenW()->SetPayCoin(
            user->getCurCharacInvenR()->GetPayCoin() + pkg.m_addInfo2);
        user->m_historyLog.PayCoinAdd(
            (int)user->getCurCharacInvenR()->GetEventCoin(), pkg.m_addInfo2,
            (eCoinAddReason)3);
        break;
    }

    items.push_back(
        std::make_pair((ENUM_ITEMSPACE)0, (Inven_Item::INVEN_SLOT)1));
    return true;
