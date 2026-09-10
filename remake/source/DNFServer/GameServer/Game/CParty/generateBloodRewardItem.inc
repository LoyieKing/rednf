void CParty::generateBloodRewardItem(int slot, int round, unsigned int param1,
                                     unsigned int param2, bool flag)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager + 非 const iterator de/begin +
    // CItemList::find_item 4 项 / EXTRA CDataManager::find_item + memcpy + CLuckPoint 临时。@0x85b840e
    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
