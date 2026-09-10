int Dispatcher_AchievementTrigger::process(CUser* user, MSG_BASE& msg,
                                           ParamBase& param)
{
    (void)param;
    if (user->get_state() <= 2)
    {
        return 0;
    }
    MSG_ACHIEVEMENT_TRIGGER& m = reinterpret_cast<MSG_ACHIEVEMENT_TRIGGER&>(msg);
    unsigned short t15 = (unsigned short)m.m_field15;
    unsigned short t13 = (unsigned short)m.m_field13;
    unsigned short t11 = (unsigned short)m.m_field11;
    unsigned int id = m.m_field0d;
    CAchievement* ach =
        (CAchievement*)user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)0xf);
    ach->setTrigger(id, t11, t13, t15);
    return 0;
