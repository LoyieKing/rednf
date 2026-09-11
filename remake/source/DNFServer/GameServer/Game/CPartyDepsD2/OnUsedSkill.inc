// ---------------------------------------------------------------------------
// OnUsedSkill —— ORIG T 0x085b5f8e
// 报告：docs/class_func_reports/CParty/OnUsedSkill.md
// 匹配成员槽累计 BattleData.m_arr7d0[slot]（this+(i+0x2bc)*4+0xc）技能使用
// 次数；isCheckUseSkill(job, skillIdx) 通过再累加 m_field1aa4[slot]。
// ---------------------------------------------------------------------------
bool CParty::OnUsedSkill(CUser* user, int skillIdx, int param)   // ORIG 0x085b5f8e 单出口 mov $0x1,%eax
{
    IsCollectDungeonStatistic();                         // 结果未消费（ORIG 形态）

    for (int i = 0; i < 4; ++i)
    {
        if (m_member[i].m_pUser == user)
        {
            ++bd(this).m_arr7d0[i];                      // this+(i+0x2bc)*4+0xc

            // [P148 嵌套实参] ORIG 0x085b5f8e O@28：call 结果留 %eax 直调
            // outgoing arg，无 job 具名 spill。
            if (isCheckUseSkill(user->get_charac_job(), skillIdx))  // T 0x085bb5b0
                ++m_field1aa4[i];                        // this+(i+0x6a8)*4+4
        }
    }
    return true;
}