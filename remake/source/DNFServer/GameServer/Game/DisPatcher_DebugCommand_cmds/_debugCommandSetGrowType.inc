bool DisPatcher_DebugCommand::_debugCommandSetGrowType(CUser* user, int kind,
                                                       int value)
{
    if (!user->isGMUser())
        return false;
    DBG_TRACE(g_dbgCmdPrettySetGrowType, 0x133, "%s:'%s' GM Mode Grow Type.");

    int firstGrow = 0;                          // -0x10
    int secondGrow = 0;                         // -0xc
    if (kind == 1)
    {
        firstGrow = value;
    }
    else if (kind == 2)
    {
        if ((char)user->getCurCharFirstGrowType() == 0)   // +0xae getCurCharFirstGrowType
            return false;
        firstGrow = (int)(char)user->getCurCharFirstGrowType();  // +0xd2 再调一次（ORIG 如此）
        secondGrow = value;
    }
    else
    {
        return false;                           // +0xe5 其余 kind 直接 ret 0
    }

    // ORIG `call ChangeGrowType_GM; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹 + callee bool。
    if (user->ChangeGrowType_GM(firstGrow, secondGrow))  // +0xef
    {
        // ORIG `call get_state; cmp $3; je; cmp $5; je; jmp` 无槽直比链 ⇒ switch
        // （P251；if/else-if 链在 4.4 产 sete 物化）。
        switch (user->get_state())              // +0x117
        {
        case 3:
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(0),
                                 static_cast<ENUM_NOTIPACKET>(2), 0);
            break;
        case 5:                                 // +0x11f
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
            break;
        default:
            break;
        }
        user->send_skill_info();                // +0x171
        return true;
    }
    return false;
