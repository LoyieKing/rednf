bool CUser::getTagCharacInventory(TAG_CHARAC_INVEN_DATA* out) const
{
    // b28c-L5 判词（N30 族·lea-vs-add 单点残留）：本波已翻正两处——
    //   ① GetInvenData 的 `add $0x1c(%esp)` 出参组（原 cast 形，与 ORIG 同）；
    //   ② `lea 0x3ff8(%eax),%ebx`（把 TAG_CHARAC_INVEN_DATA::m_clearAvatar 由
    //      char[0x28] 改真实类型 unsigned long[10] 后免 cast，4.4 即折单条 lea，
    //      与 CInventory::GetClearAvatar(unsigned long*) 权威形参同型）。
    //   唯一残留：`lea 0x1e47(%eax),%ebx` vs 4.4 `add $0x1e47,%eax; mov %eax,%ebx`。
    //   同一基址的前一个取址 `lea 0x1e43(%eax),%esi` 两侧一致 ⇒ 差异由 4.4 的
    //   第二次 reload+目标寄存器选择决定（实测 `out->m_avatarDetail` 与
    //   `&out->m_avatarDetail[0]` 两写法产物相同），属 P30 尾判的
    //   「穷举后 add-vs-lea 不齐 ⇒ 结构性伪影不强求」。
    if (getTagCharacR() == 0)
        return false;
    // ORIG：getTagCharacInvenR() 返回 CInventory*（位于 _Charac_info.m_inven，本源码返回 Inven_Item* 同址）
    // P48 逐点重调（ORIG 0x86906b1/0x86906d8/0x8690708/0x8690731 共 4 次）；
    // cargo/GetItem 各 call 结果寄存器直传（无中间槽）。
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetInvenData(2, out->m_items2, 0x1905);
    memcpy(out->m_items,
           reinterpret_cast<const CInventory*>(getTagCharacInvenR())->m_invenItem, 0x53e);
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetClearAvatar(out->m_clearAvatar);   // b28d-solo-CUser：数组退化
    reinterpret_cast<const CInventory*>(getTagCharacInvenR())
        ->GetAvatarItemMgrR()->GetItem(
        out->m_avatarDetail,   // b28d-solo-CUser：真实类型数组退化（免 cast 出 lea）
        &out->m_avatarCount);
    out->m_cargoCapacity =
        (unsigned int)getTagCharacCargoR()->GetCapacity();
    return true;
