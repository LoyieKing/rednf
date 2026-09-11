void CPartyTelePort::process_leave_user_at_teleport(CUser* user)
{
    if (m_party == 0)
    {
        return;
    }
    if (get_teleport_state() < 0)
    {
        return;
    }
    if (get_teleport_state() > 3)
    {
        return;
    }
    if (user->get_state() != 3)
    {
        return;
    }
    // [b28-L14] ORIG 0x85bc19x：`call get_teleport_state; cmp $0xff,%al;
    // setne %al; test %al,%al; je <统一尾>` ⇒ 正向 bool 物化包裹整段主体
    // （P222 钥匙 + P84 物化形）；负极 `== -1` 早返产 sete/jne 反演。
    if (get_teleport_state() != -1)
    {
        int slot = m_party->GetMemberSlotNo(user);
        if (slot < 0)
        {
            return;
        }
        if (slot > 3)
        {
            return;
        }
        if (m_party->checkValidUser(slot) != 1)
        {
            return;
        }

        if (m_party->getManager() == user)
        {
            set_teleport_member_state(slot, 0);
            set_teleport_state(2);
            send_teleport_status(8);
            reset_teleport_data();
        }
        else
        {
            set_teleport_member_state(slot, 4);
            check_cur_teleport_state();
            set_teleport_member_state(slot, 0);
            send_teleport_status(0);
            set_teleport_member_state(slot, 4);
            process_teleport();
        }
    }
}