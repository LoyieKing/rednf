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
