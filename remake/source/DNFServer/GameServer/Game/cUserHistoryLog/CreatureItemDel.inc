void cUserHistoryLog::CreatureItemDel(INVEN_TYPE invenType, int itemIdx, int count, int addInfo, int type, eItemDelReason reason)
{
    // [N102 翻案] `*(0, m_traderCharac)` 复现 ORIG 数组退化两步寻址。
    if (type == 5) {
        if (reason == static_cast<eItemDelReason>(0x14) || reason == static_cast<eItemDelReason>(0x15)) {
            if (*(0, m_traderCharac) != 0)
                m_user->LogHistory("CTItem-,%d,%d,%d,%d,\"%s\",\"%s\"", itemIdx, count, addInfo,
                                   static_cast<int>(reason), m_traderCharac, m_traderAccount);
        } else {
            m_user->LogHistory("CTItem-,%d,%d,%d,%d", itemIdx, count, addInfo, static_cast<int>(reason));
        }
    } else {
        Inven_Item item;
        item.reset();
        item.m_addInfo = itemIdx;   // ORIG local_47 = param_3（m_addInfo +0x02）
        ItemDel(invenType, count, addInfo, item, reason);
    }
