void Secu_HackLogCheckByParty::DieMob(const MSG_MONSTER_DIE* msg,
                                      map_monster* mob)
{
    if (msg->m_fieldF != -1)
    {
        Secu_DungeonAverageCheck::setKillMonsterInfo(msg, mob);
    }
