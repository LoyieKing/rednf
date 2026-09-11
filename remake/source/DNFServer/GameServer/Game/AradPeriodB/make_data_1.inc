EventRewardItemArad FnRewardLethe::make_data(int eventCode)
{
    return EventRewardItemArad(eventCode, m_user->get_acc_id(), 0);
}