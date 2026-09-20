// ORIG _ZN13user_creature12CCreatureMgr24UpdateCreatureItemResultEP17SIG_...
// T 0x0833b592（UpdateCreatureItemResult.md）：同载入链；成功且 field64==4
// 广播 SendCreatureItemInfo。
bool CCreatureMgr::UpdateCreatureItemResult(SIG_CREATURE_ITEM* sig)
{
    // ORIG 0x833b592：仅 `lea -0x45,%eax; mov (%esp); call Inven_Item::Inven_Item`
    // 一条——清零由 ctor（Inven_Item.cpp reset()）完成，无独立 memset。
    Inven_Item localItem;
    // b28d-61 [N107 反向可翻，ORIG 0x833b5a6 `lea 0x4(%eax),%edx`]：数组退化形
    // `(void*)sig->m_item` 在 4.4 恒折 `add $0x4,%eax`（探针 /tmp/b61_probe2
    // gB..gI + /tmp/b61_probe3 gJ 全 add）；改显式指针算术 `(char*)sig + 4`
    // 产 ORIG 的 lea 形（探针 gK/gL/gM）。+4 = m_item 区偏移（SigTypes_B.h）。
    sig->m_itemView.makeInvenItem(localItem);

    // ORIG tail `call; xor $0x1,%eax(dead flags-consumed); …mov$0`＝P168a② 直分支
    // 负判形（==0/! 早返失败），成功路径 fall-through 共享尾 mov$1。
    if (!LoadCreatureItem(&localItem,
                          reinterpret_cast<SIG_CREATURE_ITEM_DETAIL_INFO*>(&sig->m_slot)))
        return false;
    if (sig->m_field64 == 4)
        SendCreatureItemInfo(sig);
    return true;
}