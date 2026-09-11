// b28d-40: P223 双守卫分落（同 item_reward_quest 形）；0x086aca16，AE。
int UserQuest::get_rescue_questmap(int mapIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] > 0)
        {
            if (m_trigger[i] == 0)
            {
                continue;
            }
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest == 0)
            {
                return 0;
            }
            int map = quest->get_appearmap(mapIdx, (int)m_authenData.m_characClass);
            if (map > 0)
            {
                return map;
            }
        }
    }
    return 0;
}