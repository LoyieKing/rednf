// ============================================================================
// CParty::getMemberMaxLevel —— ORIG T 0x085b60be（getMemberMaxLevel.md）
// 汇编：maxLevel(-0x14)=0；i(-0xc)=0..3 循环：
//   call _checkValidUser(i)；xor $1 / test / jne → 无效槽跳过；
//   eax = *(int*)(this + i*0x18 + 0x78)（== m_member[i].m_pUser）作 this 调
//   CUserCharacInfo::get_charac_level()；std::max<int>(&maxLevel,&level) 回写。
// 返回 maxLevel。转写与同族 getMemberMaxLevelBeforeDungeon（CPartyDepsB2.cpp）一致。
// ============================================================================
int CParty::getMemberMaxLevel()
{
    int level;                                          // -0x10（槽位对齐 ORIG）
    int maxLevel = 0;                                   // -0x14
    for (int i = 0; i <= 3; ++i)                        // -0xc
    {
        if (!_checkValidUser(i))
            continue;
        level = m_member[i].m_pUser->get_charac_level();
        maxLevel = std::max(maxLevel, level);
    }
    return maxLevel;
}
