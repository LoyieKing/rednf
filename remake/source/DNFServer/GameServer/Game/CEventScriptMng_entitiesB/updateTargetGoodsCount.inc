// ORIG 0x0810f9ce：目标商品购买计数更新。
// user 为空走 cMyTrace 告警；否则取 acc_id，当 IPG 请求条目 m_itemIdx
// 命中脚本 m_set0c（[item idx start]/[item idx end] 区间导入的目标商品集合）
// 时向 DB 队列发 DB_UpdatePurchaseCashItemBonus（m_id, acc_id）请求，
// 并以 cMyTrace 记 "updateTargetGoodsCount(%d)"（acc_id）。
// [b28d-solo-CEventScriptMng_entitiesB] AE（compare_tu 口径，71=71，帧 0x48 无
// callee-saved）。修法：P293 具名迭代器固定 find→end 求值序（ORIG sret -0x38/-0x20；
// 4.4 单表达式 `find()!=end()` 恒先 end 后 find）；accId 声明 unsigned 消 %ebx
// 跨 GetUID 保活（P290 反向）；两处 cMyTrace 改全表达式临时链复现 ORIG 槽深（P221）。
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
}