// ORIG _ZN21Inven_Item_Compatible13makeInvenItemER10Inven_Item W 0x833f418。
// [b28c-30] N76 封顶：末段 `*reinterpret_cast<int*>(&item.m_amp) = m_amp0;` ORIG
// 为 `mov %edx,0x11(%eax)` disp 直写；取址形反产 `add $0x11,%eax; mov %edx,(%eax)`
//（+1 条）。具名 int 视角可去 add 但装载序变 RHS-first（ORIG LHS-first），净差 1
// ⇒ 封顶（前两段 guid 结构赋值 + 两字段同型化已 AE）。
// [b28d-23 NEAR·N76] m_amp0Int 具名视角消 add $0x11（54→47 条）；残余 1 = +0x11 store 装载序（ORIG LHS-first vs union 视角 RHS-first，int/unsigned 同异型试尽）⇒ N76。ORIG 0x0833f418
// [b28d-solo-CCreatureMgr AE] P247 stAmplifyHeadView 4B 结构赋值复现 +0x11 LHS-first；ORIG 0x833f418
void Inven_Item_Compatible::makeInvenItem(Inven_Item& item)
{
    item.m_field0 = m_field0;
    item.m_field1 = m_type;
    item.m_addInfo = m_itemIdx;
    item.m_addInfo2 = m_addInfo2;
    item.m_fieldb = m_endurance;
    item.SetItemAttr(m_attr);
    item.m_fieldd = m_fieldd;
    // ORIG 0x833f476..0x833f497：guid 三写（0x15/0x19/0x1d）以单一 %eax 基址
    // 连续（共享装载），m_amp0(→+0x11) 独立重载置后；中间变量 guid 反产 -0xc 槽。
    // ORIG 0x833f476：单条结构赋值（双基址各载一次 + 三连续 store）——具名
    // guid 视图成员直赋值（三独立字段赋值 4.4 恒逐条重载双基址）。
    item.m_amp.m_guid = m_guid;                                      // → item+0x15/0x19/0x1d
    // [b28d-solo-CCreatureMgr P247] 同 setCopy：4B 结构赋值形（LHS-first）。
    item.m_amp.m_amp0Head = m_amp0;                                  // → item+0x11（ORIG 置后独立重载）
}
