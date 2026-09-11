// ---------------------------------------------------------------------------
// canFatigueUp —— ORIG T 0x085a8c88
// 报告：docs/class_func_reports/CParty/canFatigueUp.md
// partyType==1 或无副本 → false；疲劳关闭(m_bloodDungeonType!=0)/血图(m_villageAttackEventFlag==1)/免疲劳副本
// → false；否则 true。flag 参数未消费（ORIG 形态）。
// ---------------------------------------------------------------------------
bool CParty::canFatigueUp(unsigned char flag)
{
    (void)flag;
    if (m_battleField.m_bloodState == 1)
        return false;
    if (m_battleField.m_pDungeon == 0)
        return false;
    // N96 清绑后普通形：三段守卫 && 汇聚（ORIG movzbl/test、xor$1/test、
    // call/xor$1/test 逐点 je→false 岛，尾 bool 归一 test;je;mov$1/mov$0）。
    if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
        m_battleField.m_pDungeon->m_villageAttackEventFlag == 0 &&
        m_battleField.m_pDungeon->isNoFatigueDungeon() == 0)
        return true;
    return false;
}