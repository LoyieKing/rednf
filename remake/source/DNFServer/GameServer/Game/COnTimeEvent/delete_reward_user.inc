// ORIG @0x081bbc64。
bool COnTimeEvent::delete_reward_user(CUser* user)
{
    if (user == 0)
        return false;

    if (get_ontime_id() == user->get_ontime_last_recv_idx())
    {
        m_rewardUsers.delete_mark(user->get_acc_id());
        return true;
    }
    return false;
}