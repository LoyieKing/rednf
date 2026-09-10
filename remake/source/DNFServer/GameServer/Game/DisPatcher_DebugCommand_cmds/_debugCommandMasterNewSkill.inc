bool DisPatcher_DebugCommand::_debugCommandMasterNewSkill(CUser* user,
                                                          int skillIdx,
                                                          int level)
{
    if (!user->isGMUser())                      // +0x08
        return false;
    stBuySkillInfo info;                        // -0x1b 栈帧 3 字节
    info.m_field0 = 0;                          // +0x34 movb $0
    info.m_skillIdx = (char)skillIdx;           // +0x28 mov al
    info.m_count = (char)level;                 // +0x31 mov al
    user->master_new_skill(info, false);        // +0x49 master_new_skill(info,0)
    cMyTrace(g_dbgCmdPrettyMasterNewSkill, 0xbd, 0)(
        "%s:'%s' GM Mode Set Skill(%d) Level(%d).",
        NumberToString(user->get_acc_id(), 0), user->getCurCharacName(),
        skillIdx, level);
    return true;
