int CQuestShop::clearUsedQp(CUser* user)
{
    int initQp = 0;                                 // -0x18
    const int cost = getInitCost(getInitCount());   // -0x14
    if (user->getCurCharacInvenR()->get_money() < cost)
        return 0xa;                                 // +0x45 setl
    if (!user->getCurCharacInvenW()->use_money(
            cost, (eMoneySubReason)0x2b, true))     // xor $1 判定 false→ret1
        return 1;                                   // +0x86
    const int beforeQp = getQp();                   // -0x10
    incInitCount();
    user->setInitQuestPoint();
    initQp = getInitQp();                           // -0x18
    setQp(initQp);
    clearBuyingStatuslist();
    const int afterQp = getQp();                    // -0xc
    // ORIG +0x10f：getInitCount() 直接进实参位（不存局部）
    user->m_historyLog.ClearUsedQP(beforeQp, afterQp, getInitCount(), cost);
    return 0;
