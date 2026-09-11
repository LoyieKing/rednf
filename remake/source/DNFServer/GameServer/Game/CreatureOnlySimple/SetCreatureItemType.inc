// 【objdump】ORIG _ZN13user_creature13CCreatureItem19SetCreatureItemTypeENS_18CREATURE_ITME_TYPEE
// W 0x0833efe2（size 0x0e）：this+0x08 = type。
// 调用点 CCreatureMgr::Hatch @0x0833a11d 传 1（孵化成宠物）。枚举参数版本为
// worklist 权威符号（现头 inline int 版产生的 Ei 符号 ORIG 不存在，由 decl
// 条目替换掉）。
void CCreatureItem::SetCreatureItemType(user_creature::CREATURE_ITME_TYPE type)
{
    m_creatureItemType = type;
}