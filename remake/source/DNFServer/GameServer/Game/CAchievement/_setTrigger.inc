void CAchievement::_setTrigger(unsigned int id, unsigned short t0,
                               unsigned short t1, unsigned short t2)
{
    std::map<unsigned int, stAchievement>::iterator it =
        m_achievements.find(id);
    // ORIG 0x828b8fd：已找到分支为 fall-through（je 跳到未找到块），故用 != end 先行
    if (it != m_achievements.end())
    {
        // ORIG 0x828b916：setae（cur>=t0）为 fall-through 的 sub 分支，zero 分支在跳转目标
        if (it->second.m_trigger0 >= t0)
        {
            it->second.m_trigger0 -= t0;
        }
        else
        {
            it->second.m_trigger0 = 0;
        }
        if (it->second.m_trigger1 >= t1)
        {
            it->second.m_trigger1 -= t1;
        }
        else
        {
            it->second.m_trigger1 = 0;
        }
        if (it->second.m_trigger2 >= t2)
        {
            it->second.m_trigger2 -= t2;
        }
        else
        {
            it->second.m_trigger2 = 0;
        }
    }
    else
    {
        stAchievement ach;
        ach.m_id = id;
        Quest* quest = G_CDataManager()->find_quest((int)id);
        if (quest == 0)
        {
            return;
        }
        // ORIG 0x828b9dd：get_init 直接写 stAchievement 临时对象字段
        if (!quest->get_init_achievement_trigger(ach.m_trigger0, ach.m_trigger1,
                                                 ach.m_trigger2))
        {
            _sendSetTrigger(id, 1);
            return;
        }
        // ORIG 0x828ba5a：与已找到分支相同，setae 编码（sub 分支 fall-through）
        if (ach.m_trigger0 >= t0)
        {
            ach.m_trigger0 -= t0;
        }
        else
        {
            ach.m_trigger0 = 0;
        }
        if (ach.m_trigger1 >= t1)
        {
            ach.m_trigger1 -= t1;
        }
        else
        {
            ach.m_trigger1 = 0;
        }
        if (ach.m_trigger2 >= t2)
        {
            ach.m_trigger2 -= t2;
        }
        else
        {
            ach.m_trigger2 = 0;
        }
        m_achievements.insert(std::make_pair(ach.m_id, ach));
    }
    alter();
