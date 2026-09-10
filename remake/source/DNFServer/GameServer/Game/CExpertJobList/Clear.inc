void STExpertJobScript::Clear()
{
    // b28d-61 [按 ORIG 0x0896dfec 调用序重排，120/120 逐条对齐]：
    // ORIG 顺序非声明序（0x248 在 0x230 前、0x18/0x30 等在标量清零之后）。
    m_expVec.clear();
    m_growSkillVec.clear();
    m_disjointResultMap.clear();     // 0x1bc
    m_baseConst = 0;                 // 0x1d4
    m_additionalConst = 0;           // 0x1d8
    m_clearQuests.clear();           // 0x1dc
    m_giveupCost.clear();            // 0x1e8
    m_compoundRateVec.clear();       // 0x1f4
    m_recipeInfoMap.clear();         // 0x200
    m_recipeIndexMap.clear();        // 0x218
    m_recipeItemMap.clear();         // 0x248
    m_recipeLevelMap.clear();        // 0x230
    m_enduranceRepairCost.clear();   // 0x260
    m_enduranceReduce1 = 0;          // 0x26c
    m_enduranceReduce2 = 0;          // 0x270
    m_enduranceInit = 0;             // 0x274
    m_upgradeCost.clear();           // 0x278
    m_gainExp1 = 0;                  // 0x284
    m_gainExp2 = 0;                  // 0x288
    m_limitDisjointGold = 0;         // 0x28c
    m_portableDisjointMap.clear();
    m_selfDisjoin.clear();   // ORIG：stSelfDisjoinMaterialInfo::clear @0x0896dbe6
    m_shopDollMap.clear();
    m_unsealScrollMap.clear();
    m_monsterCardBindRateMap.clear();
    m_commonBindInfo.clear();    // ORIG：this+0x84 BindInfo clear
    m_specialBindInfo.clear();   // ORIG：this+0xc4 BindInfo clear
    m_alchemistExtractInfo.clear();
    m_enchanterExtractInfo.clear();
    m_dollExtractInfo.clear();
    m_alchemistResultInfo.clear();
    m_enchanterResultInfo.clear();
    m_dollResultInfo.clear();
