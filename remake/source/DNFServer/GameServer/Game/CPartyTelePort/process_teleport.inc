bool CPartyTelePort::process_teleport()
{
    char result;
    if (m_party == 0)
    {
        return 0;
    }

    result = check_cur_teleport_state();
    int moveResult = 0;
    int auraAvatarTime = 0;
    if (result != 0)
    {
        for (int i = 0; i <= 3; ++i)
        {
            if (m_party->checkValidUser(i) != 1)
            {
                continue;
            }
            if (get_teleport_member_state(i) == 4)
            {
                continue;
            }
            CUser* user = (CUser*)m_party->get_user(i);
            if (user != 0)
            {
                // [b28d-solo-CPartyTelePort] ORIG 0x85bbc68：dir/posy 有槽
                // （-0x78/-0x74，P0 具名局部）；posx/area/village 无槽往返
                // ⇒ 源码在实参位内联 getter。
                int dir = (int)get_teleport_direction();
                int posy = (int)get_teleport_posy();
                moveResult = G_GameWorld()->check_move_area(user,
                    (int)get_teleport_village(), (int)get_teleport_area_index(),
                    (int)get_teleport_posx(), posy, dir, true, 0);
                if (m_party->getManager() == user)
                {
                    moveResult = user->is_equip_aura_avatar(0, auraAvatarTime);
                }
                if (moveResult > 0)
                {
                    set_teleport_member_state(i, 0);
                    set_teleport_state(2);
                    break;
                }
            }
        }

        if (get_teleport_state() == 1)
        {
            send_teleport_effect_at_different_place(1);
            for (int i = 0; i <= 3; ++i)
            {
                if (m_party->checkValidUser(i) != 1)
                {
                    continue;
                }
                if (get_teleport_member_state(i) == 4)
                {
                    continue;
                }
                CUser* user = (CUser*)m_party->get_user(i);
                if (user != 0)
                {
                    user->set_charac_visible_teleport_(true);
                    int dir = (int)get_teleport_direction();
                    int posy = (int)get_teleport_posy();
                    // [b28d-solo-CPartyTelePort] ORIG 0x85bbeb4：move_area 返回
                    // bool 直测（无 moved 槽）；trace 用 P227 全表达式临时链
                    // （实参 getCurCharacNo 在 ctor 前求值并跨 ctor 保活 %ebx）。
                    if (G_GameWorld()->move_area(user, (int)get_teleport_village(),
                            (int)get_teleport_area_index(), (int)get_teleport_posx(),
                            posy, dir, true, 1, 0, 0) != 1)
                    {
                        cMyTrace(__PRETTY_FUNCTION__, 0x43f9, 5)
                            ("party_teleport_error:(char_no:%d)", user->getCurCharacNo());
                    }
                    user->set_charac_visible_teleport_(false);
                }
            }
            send_teleport_effect_at_equal_place(2);
        }
        else
        {
            send_teleport_status((unsigned char)moveResult);
        }
    }

    if (get_teleport_state() == 1)
    {
        reset_teleport_data();
        CUser* manager = (CUser*)m_party->getManager();
        if (manager != 0)
        {
            // [b28d-solo-CPartyTelePort] ORIG 0x85bbf95：`movl $&GlobalData::
            // s_systemTime_,(%esp)`（对象直调，P304）；curSec/optValue/charNo
            // 均无槽往返 ⇒ 内联表达式 + P227 临时链。
            int newTime = GlobalData::s_systemTime_.getCurSec() + auraAvatarTime;
            manager->set_aura_avatar_option_value(0, newTime);
            manager->UpdateAuraAvatarOption(0, manager->get_aura_avatar_option_value(0));
            cMyTrace(__PRETTY_FUNCTION__, 0x4419, 0)
                ("party_teleport_success:(char_no:%d)", manager->getCurCharacNo());
        }
    }
    return result;
