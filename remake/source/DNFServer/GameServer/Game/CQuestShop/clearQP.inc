void CQuestShop::clearQP(CUser* user)
{
    const int oldQp = getQp();          // -0x14
    user->setInitQuestPoint();
    const int initQp = getInitQp();     // -0x10
    setQp(initQp);
    clearBuyingStatuslist();
    // ORIG 0x85ef5d5：afterQp=getQp() 先于 getInitCount()（实参求值序），history = +0x79700
    const int afterQp = getQp();        // -0xc
    user->m_historyLog.ClearUsedQP(oldQp, afterQp, getInitCount(), 0);
