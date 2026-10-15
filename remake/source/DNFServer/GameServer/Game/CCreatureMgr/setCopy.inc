// ORIG _ZN21Inven_Item_Compatible7setCopyERK10Inven_Item W 0x833f38e。
// [b28d-23 NEAR·N76] 语句序照 ORIG 0x833f38e 重排 + m_amp0Int 具名视角（54→47 条）；残余 1 = +0x11 装载序 ⇒ N76。ORIG 0x0833f38e
// [b28d-solo-CCreatureMgr AE] P247 stAmplifyHeadView 4B 结构赋值复现 +0x11 LHS-first；ORIG 0x833f38e
void Inven_Item_Compatible::setCopy(const Inven_Item& item)
{
    // [b28d-23 翻正] 语句序照 ORIG 0x833f38e 逐条发射序（-O0 书写序=机器码序）：
    // +0x0/+0x1/+0x4/+0xc/+0x10 → GetItemAttr(+0x8) → +0x14 → guid 结构赋值
    // (+0x1c/0x20/0x24) → +0x18（m_amp0 置后）。原按偏移升序书写致整段错位。
    m_seal = item.m_seal;
    m_type = item.m_itemType;
    m_itemIdx = item.m_addInfo;
    m_addInfo2 = item.m_addInfo2;
    m_endurance = item.m_endurance;
    m_attr = item.GetItemAttr();
    m_extendInfo = item.m_extendInfo;
    // ORIG（objdump 0x833f3ef..0x833f412）：guid 三字段为单条结构赋值形
    // （双基址各载一次 + 三连续 store）；+0x11 的 m_amp0 独立重载置后。
    m_guid = item.m_amp.m_guid;                                     // item+0x15/0x19/0x1d
    // [b28d-solo-CCreatureMgr P247] 4B 结构赋值形（LHS-first）复现 ORIG 尾段
    // `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov 0x11(%edx),%edx;
    //  mov %edx,0x18(%eax)`；标量 int 形恒 RHS-first（残余 2 行根因）。
    m_amp0 = item.m_amp.m_amp0Head;                                 // item+0x11（4B 结构视角）
}
