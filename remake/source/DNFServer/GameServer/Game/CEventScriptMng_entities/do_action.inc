// ============ CLevelUpReward（0x194 → LevelUpRewardScript）============
// ORIG T 0x810e4ea：以绑定脚本 m_mail（+0x28）为 Action_SendMail&，
// flag=true 走 process_action_send_mail 发奖。
void CLevelUpReward::do_action(CUser* user)
{
    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(static_cast<LevelUpRewardScript*>(m_script1c)->m_mail),
        true);
}