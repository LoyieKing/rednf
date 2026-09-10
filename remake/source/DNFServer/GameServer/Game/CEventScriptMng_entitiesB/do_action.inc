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
