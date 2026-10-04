int Inter_CeraShopPurchaseCount::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    SIG_CERASHOP_PURCHASE_COUNT* cnt = reinterpret_cast<SIG_CERASHOP_PURCHASE_COUNT*>(data);
    if (cnt == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x6a96, 5)("[PURCAHSE BONUS] internal data is null.");
        return 0x6a97;
    }
    GlobalData::s_pItemVendingMachine->ProcessPurcahseBonusItem(
        user,
        cnt->m_itemUid,
        cnt->m_count,
        cnt->m_purchaseCnt);
    return 0;
}