// ---- 依赖：CItemGeneratorMgr::onTryGenerateRandomItem（ORIG T 0x085342c6）----
void CItemGeneratorMgr::onTryGenerateRandomItem(eGenerateType_t genType,
                                                eItemDropType_t dropType,
                                                ENUM_RARITY rarity)
{
    // ORIG T 0x085342c6 索引 = (dropType*6 + genType*30 + rarity)*4 ⇒ 行主序
    // [genType][dropType][rarity]，即权威 CItemGeneratorMgr::m_dropStat（与
    // m_head 同起点的 union 视图；`mov (%esi,%eax,4)`/`mov %esi,(%ebx,%eax,4)`
    // 均 4 字节 ⇒ unsigned int 宽一致，未对齐/越界风险随具名化消除）。
    ++m_dropStat[genType][dropType][rarity];
}