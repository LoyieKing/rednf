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
    // [G1Frest 2026-09-10] ORIG 0x081e5xx：m_0d 在三个使用点各自重读
    //   （`mov -0x10(%ebp),%eax; movzbl 0xd(%eax),%eax; movsbl %al,%eax` ×3），
    //   level 亦无独立槽（`movswl %ax,%ebx` 直落 callee-saved）
    //   ⇒ 旧码 slot/level 两个具名局部（-0x10/-0xa）为发明，且把 mref 槽挤到 -0x14。
    if ((int)static_cast<signed char>(mref.m_0d) < (int)user->m_characList.size())
    {
        if (user->m_characList[(int)static_cast<signed char>(mref.m_0d)].m_characNo == 0)
        {
            return 0x15;
        }
        if (user->m_characList[(int)static_cast<signed char>(mref.m_0d)].m_level <
            G_CDataManager()->m_characLink.m_field0)
        {
            return 0xe;
        }
        return 0;
    }
    return 0x39;
