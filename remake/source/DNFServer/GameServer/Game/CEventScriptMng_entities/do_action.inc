void CLevelUpReward::do_action(CUser* user)
{
    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<LevelUpRewardScript*>(m_script1c)->m_mail),
        true);
