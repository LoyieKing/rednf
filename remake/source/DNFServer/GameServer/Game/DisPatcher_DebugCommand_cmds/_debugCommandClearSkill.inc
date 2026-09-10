bool DisPatcher_DebugCommand::_debugCommandClearSkill(CUser* user,
                                                      MSG_DEBUG_COMMAND& /*msg*/)
{
    if (!user->isGMUser())                      // +0x08（P168a① 裸形）
        return false;
    DBG_TRACE(g_dbgCmdPrettyClearSkill, 0x8a, "%s:'%s' GM Mode Clear Skill.");
    if (user->ClearSkills())                    // +0x92 ClearSkills；==0 → ret 0
    {
        user->send_skill_info();                // +0xa1
        if (user->get_state() == 5)             // +0xb1 state==5
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
        goto ok;                                // P223：成功体尾 jmp 岛 + 失败块前
    }
    return false;                               // +0xe0 失败块（mov$0）
ok:
    return true;                                // +0xe7 成功块（mov$1）
