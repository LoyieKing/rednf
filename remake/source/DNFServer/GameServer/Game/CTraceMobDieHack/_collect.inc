void CTraceMobDieHack::_collect(CUser** users, int param_2)
{
    for (int i = 0; i <= 3; ++i)
    {
        if (users[i] == 0)
        {
            continue;
        }
        if (users[i]->IsGameMasterMode())
        {
            continue;
        }
        if (users[i]->GetParty() == 0 ||
            ((CParty*)users[i]->GetParty())->m_battleField.m_pDungeon == 0)
        {
            continue;
        }
        if (users[i]->get_charac_level() <= 10)
        {
            continue;
        }
        int diff = users[i]->get_charac_level() -
                   ((CParty*)users[i]->GetParty())->m_battleField.m_pDungeon->get_standard_level();
        if (diff > 5)
        {
            continue;
        }
        stHackLog_t log;
        log.m_accId = users[i]->get_acc_id();
        log.m_characNo = users[i]->getCurCharacNo();
        log.m_hackType = 0x193;
        log.m_value = param_2;
        log.m_count = 1;
        std::vector<stHackLog_t>::iterator found =
            std::find(m_hackLogs.begin(), m_hackLogs.end(), log);
        if (found == m_hackLogs.end())
        {
            m_hackLogs.push_back(log);
        }
        else
        {
            found->m_count += 1;
        }
    }
