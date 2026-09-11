// ORIG _ZN13user_creature13CCreatureItem18GetCreatureItemTypeEv W 0x0833ef98
//（本批改为外联定义：SwapCreature 链按 ORIG 外联调用形态取类型）。
CREATURE_ITME_TYPE CCreatureItem::GetCreatureItemType()
{
    return m_creatureItemType;   // 同型直返（enum）
}
