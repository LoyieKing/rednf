int Timer_OneADayItemShop::dispatch_sig(int p1, int p2, unsigned int p3)
{
    // ORIG 0x863a916: G_CDataManager()->m_itemShop->updateOneADayItemList(); RegistNextTimer(); return 1
    G_CDataManager()->m_itemShop->updateOneADayItemList();   // m_itemShop @+0x8cbc
    RegistNextTimer();
    return 1;
