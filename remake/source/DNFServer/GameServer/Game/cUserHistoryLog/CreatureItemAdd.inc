// b28d-65 [N4/N102]：残余仅 m_traderCharac[0] add-vs-disp 1 条（同 TradeBegin 判词）；
//   ORIG 0x08684e3c，78=77+1 封顶。
// [b28d-solo-cUserHistoryLog 2026-09-09 N102 翻案] 同 TradeBegin ⇒ 本函数 AE。
void cUserHistoryLog::CreatureItemAdd(INVEN_TYPE invenType, int itemIdx, int count, int addInfo, int type, eItemAddReason reason)
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (type == 5) {
        if (reason == static_cast<eItemAddReason>(0x1a) || reason == static_cast<eItemAddReason>(0x1c)
            || reason == static_cast<eItemAddReason>(0x1d)) {
            if (*(0, m_traderCharac) != 0)
                m_user->LogHistory("CTItem+,%d,%d,%d,%d,\"%s\",\"%s\")", itemIdx, count, addInfo,
                                   static_cast<int>(reason), m_traderCharac, m_traderAccount);
        } else {
            m_user->LogHistory("CTItem+,%d,%d,%d,%d", itemIdx, count, addInfo, static_cast<int>(reason));
        }
    } else {
        Inven_Item item;
        item.reset();
        item.m_addInfo = itemIdx;   // ORIG local_47 = param_3（m_addInfo +0x02）
        ItemAdd(invenType, count, addInfo, item, reason);
    }
}