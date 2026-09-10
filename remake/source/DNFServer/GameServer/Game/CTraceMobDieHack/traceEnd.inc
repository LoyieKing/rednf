void CTraceMobDieHack::traceEnd(CUser* killer, CUser** users)
{
    if (!m_bTracing)
    {
        return;
    }
    long curSec = GlobalData::s_systemTime_.getCurSec();
    m_bTracing = false;
    if (killer == 0 || killer->GetParty() == 0 ||
        ((CParty*)killer->GetParty())->m_battleField.m_pDungeon == 0)
    {
        return;
    }
    int dungeonIndex = ((CParty*)killer->GetParty())->m_battleField.m_pDungeon->get_index();
    int specialDungeons[4];
    specialDungeons[0] = 1;
    specialDungeons[1] = 2;
    specialDungeons[2] = 0x29;
    specialDungeons[3] = 0x21;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        if (specialDungeons[i] == dungeonIndex)
        {
            return;
        }
    }
    for (int j = 0; j <= 3; ++j)
    {
        if (users[j] == 0 || users[j]->getCurCharacR() == 0 ||
            users[j]->GetParty() == 0)
        {
            continue;
        }
        if (users[j]->get_charac_level() >
            ((CParty*)users[j]->GetParty())->m_battleField.m_pDungeon->get_standard_level() + 10)
        {
            continue;
        }
        m_records[j].m_user = users[j];
        m_records[j].m_damage += (int)(curSec - m_traceBeginTime);
        m_records[j].m_count += 1;
    }
