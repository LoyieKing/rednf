// ============================================================================
// CCreatureItem::Set @ 0x0833ef20：深拷贝 src 各字段（蛋 → 宠物项复制）。
// ============================================================================
// [b28d-solo-CCreatureMgr 检查点14 头改动回归·已报 Main] 37 vs 39：CInventory.h 把
//   m_noCharge 改 bool 而 GetNoCharge() 仍返 unsigned char ⇒ 本函数 `m_noCharge =
//   src->GetNoCharge();` 4.4 插 `test %al,%al; setne %dl`（+2 条）。ORIG 0x833ef86
//   `call GetNoCharge; mov 0x8(%ebp),%edx; mov %al,0x14(%edx)` 直存；getter 体
//   0x833efd6 `movzbl 0x14(%eax),%eax; ret`（P315/P136）⇒ GetNoCharge 应为 **bool**
//   （返回类型不入 mangled）。等待 Main 落头+定义同步；本 TU 不改头。
void user_creature::CCreatureItem::Set(user_creature::CCreatureItem* src)
{
    // ORIG 0x0833ef20：经 getter 逐字段读 src（-O0 产生 call），且
    // m_expireTime(+0x18) 置 0 而非拷贝（蛋→宠物项复制，新宠无到期时间）。
    m_itemId = src->GetItemId();
    m_creatureItemType = src->GetCreatureItemType();
    m_uid = src->GetUid();
    m_slotNo = src->GetSlotNo();
    m_noCharge = src->GetNoCharge();
    m_stat = src->GetStat();
    m_expireTime = 0;
}
