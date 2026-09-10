int Dispatcher_RequestCharacSkillInfo::check_error(CUser* user, MSG_BASE& msg)
{
    MSG_REQUEST_CHARAC_SKILL_INFO& mref = reinterpret_cast<MSG_REQUEST_CHARAC_SKILL_INFO&>(msg);  // 裸偏移→具名字段（同偏移同宽度）
    if (user == 0)
    {
        return -1;
    }
    if (user->get_state() <= 2)
    {
        return -1;
    }
    int slot = static_cast<signed char>(mref.m_0d);
    if (slot < (int)user->m_characList.size())
    {
        if (user->m_characList[slot].m_characNo == 0)
        {
            return 0x15;
        }
        short level = user->m_characList[slot].m_level;
        if (level < G_CDataManager()->m_characLink.m_field0)
        {
            return 0xe;
        }
        return 0;
    }
    return 0x39;
