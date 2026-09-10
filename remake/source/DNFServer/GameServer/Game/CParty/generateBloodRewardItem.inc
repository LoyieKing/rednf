void CParty::generateBloodRewardItem(int slot, int round, unsigned int param1,
                                     unsigned int param2, bool flag)
{
    // b28d-49 判词：N 封顶 + 缺口。P167 MISS G_CDataManager + 非 const iterator de/begin +
    // CItemList::find_item 4 项 / EXTRA CDataManager::find_item + memcpy + CLuckPoint 临时。@0x85b840e
    // P183：ORIG 各使用点重取址（无引用缓存槽）⇒ 宏直用。
    // [G7-sub] 本 .inc 域内**无任何语句**（仅函数头 + 判词；主体在聚合器 cpp，非本泳道写域）
    // ⇒ 无法在本轮改动。读数 495(ORIG)/484(OURS)：缺口 11 = P167 MISS 调用项
    // （G_CDataManager 重调 / CItemList::find_item / 迭代器 de+begin），属 cpp 侧待补。
