EventRewardItemArad FnRewardInterface::make_data(int eventCode)
{
    if (m_recordCharacNo != 0)
    {
        return EventRewardItemArad(eventCode, m_user->get_acc_id(),
                                   m_user->get_charac_no(-1));
    }
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
