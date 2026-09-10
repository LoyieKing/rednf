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
