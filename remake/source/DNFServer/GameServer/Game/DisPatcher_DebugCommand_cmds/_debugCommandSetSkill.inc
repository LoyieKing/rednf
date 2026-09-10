bool DisPatcher_DebugCommand::_debugCommandSetSkill(CUser* user, int skillIdx,
                                                    int level)
{
    if (!user->isGMUser())                      // +0x08
        return false;
    // ORIG `call SetSkillLevel; test %al,%al; je <尾 mov $0>` ⇒ 正向包裹 + callee bool
    // （P248 档①：int callee 的一切调用点写法恒产 sete 四段）。
    if (user->SetSkillLevel(skillIdx, level))   // +0x38
    {
        cMyTrace(g_dbgCmdPrettySetSkill, 0xa2, 0)(
            "%s:'%s' GM Mode Set Skill(%d) Level(%d).",
            NumberToString(user->get_acc_id(), 0), user->getCurCharacName(),
            skillIdx, level);
        user->send_skill_info();                // +0xbb
        if (user->get_state() == 5)             // +0xd1
            user->SendNotiPacket(static_cast<CUser::eSendTarget>(2),
                                 static_cast<ENUM_NOTIPACKET>(2), 1);
        goto ok;                                // P223：成功体尾 jmp 岛 + 失败块前
    }
    return false;
ok:
    return true;
