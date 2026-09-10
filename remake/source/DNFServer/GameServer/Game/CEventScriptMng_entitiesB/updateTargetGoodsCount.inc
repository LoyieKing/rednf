void CPurchaseCashItemBonus::updateTargetGoodsCount(
    CUser* user, unsigned int characNo, stCeraShopIPGRequest_t* request)
{
    if (user == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__,
                 0x737, 0)(
            "Purchase Error Not Found User(charac_no:%d)", characNo);
        return;
    }
    unsigned int accId = (unsigned int)user->get_acc_id();
    // P293：ORIG find 先求值（sret -0x38）、end 后求值（sret -0x20）；
    // 4.4 对单表达式 `find()!=end()` 恒先 end 后 find ⇒ 具名迭代器固定求值序。
    std::set<unsigned int>::iterator it =
        static_cast<PurchaseCashItemBonusScript*>(m_script1c)
            ->m_set0c.find(request->m_itemIdx);
    if (it != static_cast<PurchaseCashItemBonusScript*>(m_script1c)->m_set0c.end())
    {
        DB_UpdatePurchaseCashItemBonus::makeRequest(user->GetUID(), accId);
        cMyTrace(__PRETTY_FUNCTION__,
                 0x742, 0)(
            "CPurchaseCashItemBonus::updateTargetGoodsCount(%d)", accId);
    }
