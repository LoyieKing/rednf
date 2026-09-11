// ---- 简单访问器（ORIG 证据见各注释；宠物侧字段未建模部分显式标注）----
// ORIG GetCreatureItemId @ 0x833a638：m_equippedCreature 为空返回 0，
// 否则 CCreatureItem::GetItemId（W 0x833efa4，镜像同址转换调用）。
int CCreatureMgr::GetCreatureItemId() const
{
    // ORIG 0x833a638：test;je→mov0 尾块；call fall-through = 三元 ?: 形态。
    return m_equippedCreature != 0
               ? reinterpret_cast<CCreatureItem*>(m_equippedCreature)->GetItemId()
               : 0;
}
