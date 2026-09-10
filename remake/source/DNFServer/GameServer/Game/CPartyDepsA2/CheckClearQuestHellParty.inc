void CParty::CheckClearQuestHellParty(std::vector<int>& out)
{
    out.clear();
    // [b28d-solo-CPartyDepsA2] ORIG 0x085a7a83 只有一条 `movl $0x0,-0x10`
    // （for-init 单次）⇒ 循环变量不在声明处初始化；且 ORIG 0x085a7b1b 段
    // getCurCharacQuestR 结果直传 isClearQuest 的 this、无 UserQuest* 具名局部
    // （P0 槽-局部律：我们的 -0xc 槽即该多出的局部）。
    for (int local_14 = 0; local_14 <= 3; ++local_14)
    {
        if (!_checkValidUser(local_14))
            continue;

        int questId = 0;
        std::vector<int>::iterator it = m_hellWorldMap->m_field1c.begin();
        std::vector<int>::iterator endIt = m_hellWorldMap->m_field1c.end();
        while (it != endIt)
        {
            questId = *it;
            if (!(m_member[local_14]
                      .m_pUser->getCurCharacQuestR()
                      ->isClearQuest(questId)))
            {
                out.push_back(local_14);
                break;
            }
            ++it;
        }
    }
