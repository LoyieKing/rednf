bool DisPatcher_DebugCommand::_debugCommandOff(CUser* user,
                                               MSG_DEBUG_COMMAND& /*msg*/)
{
    // P168a①：ORIG `call isGMUser; xor $1; test %al; je <成功块>` ⇒ bool callee 裸形。
    if (!user->isGMUser())
        return false;
    DBG_TRACE(g_dbgCmdPrettyOff, 0x41, "%s:'%s' GM Mode OFF.");
    user->SetGameMasterMode(false);             // +0x94 SetGameMasterMode(0)
    return true;
