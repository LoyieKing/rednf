bool DisPatcher_DebugCommand::_debugCommandLevelUpDown(CUser* user, int delta)
{
    if (!user->isGMUser())
        return false;
    DBG_TRACE(g_dbgCmdPrettyLevelUpDown, 0x1ce, "%s:'%s' GM Mode Level Up.");

    if (delta * delta >= 2)                     // +0x93 imul/jle：仅 |delta|<=1 通过
        return false;

    bool changed = true;                        // -0x9 local_d
    // P0：ORIG 的 -0x14/-0x10 是 min 的**匿名临时槽**（表达式右值 + 常量物化），
    // 无具名局部（具名局部会占据取址槽并改变 4.4 分配序）。
    if (user->SetCharacLevel(std::min(200, user->get_charac_level() + delta)) != 1)
        changed = false;
    if (changed)
    {
        // ORIG `call get_state; cmp $3; je; cmp $5; je; jmp` 无槽直比链 ⇒ switch（P251）。
        switch (user->get_state())
        {
        case 3:                                 // +0x101 村庄态
        {
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(0x25), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                                 static_cast<ENUM_NOTIPACKET>(2), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            PacketGuard questPacket;            // -0x30
            user->getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&questPacket));
            user->Send(questPacket);
            break;
        }
        case 5:                                 // +0x109 战场态
        {
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(1),
                                 static_cast<ENUM_NOTIPACKET>(0x25), 0);
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            PacketGuard questPacket;            // -0x3c
            user->getCurCharacQuestR()->get_quest_info(
                reinterpret_cast<char*>(&questPacket));
            user->Send(questPacket);
            break;
        }
        default:
            break;
        }
    }
    return changed ? 1 : 0;                     // +0x286 movzbl local_d
