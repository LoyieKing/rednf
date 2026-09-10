int Inter_CeraShopPurchaseCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_CeraShopPurchaseCountPktView* pPkt = reinterpret_cast<Inter_CeraShopPurchaseCountPktView*>(data);
    if (pPkt == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6a96, 5)("[PURCAHSE BONUS] internal data is null.");
        return 0x6a97;
    }
    GlobalData::s_pItemVendingMachine->ProcessPurcahseBonusItem(
        user,
        pPkt->m_4,
        pPkt->m_c,
        pPkt->m_8);
    return 0;
