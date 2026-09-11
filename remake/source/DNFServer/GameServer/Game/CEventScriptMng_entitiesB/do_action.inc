// ORIG T 0x0810dfe8：发脚本邮件（AccountFirstLoginScript::m_mail @ +0xc）后
// 向 DB 插入账号每日首次登录标记（GetUID, acc_id, charac_no, flags）。
void CAccountFirstLoginDaily::do_action(int flags, CUser* user,
                                        std::bitset<32u>)
{
    if (user == 0)
        return;

    CEventActionMng mng;
    mng.process_action_send_mail(
        user,
        EVENT_MAIL_REF(
            static_cast<AccountFirstLoginScript*>(m_script1c)->m_mail),
        true);
    DB_InsertAccountFirstLoginDailyFlag::makeRequest(
        user->GetUID(), user->get_acc_id(), user->getCurCharacNo(), flags);
}