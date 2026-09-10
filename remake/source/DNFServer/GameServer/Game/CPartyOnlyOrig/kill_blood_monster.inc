void CParty::kill_blood_monster(CUser* user, int monsterIdx,
                                unsigned short* param)
{
    reinterpret_cast<CBattle_Field*>(&m_battleField)
        ->kill_blood_monster(user, monsterIdx, param);
