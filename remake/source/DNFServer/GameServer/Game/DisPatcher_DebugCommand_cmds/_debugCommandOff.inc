// ===========================================================================
// 1. _debugCommandOff —— 0x0858e918
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：isGm 局部内联为 P168a① bool 裸形
// （ORIG `call isGMUser; xor $1; test %al; je`）+ DBG_TRACE 改 P227 全表达式临时链。
bool DisPatcher_DebugCommand::_debugCommandOff(CUser* user,
                                               MSG_DEBUG_COMMAND& /*msg*/)
{
    // P168a①：ORIG `call isGMUser; xor $1; test %al; je <成功块>` ⇒ bool callee 裸形。
    if (!user->isGMUser())
        return false;
    DBG_TRACE(__PRETTY_FUNCTION__, 0x41, "%s:'%s' GM Mode OFF.");
    user->SetGameMasterMode(false);             // +0x94 SetGameMasterMode(0)
    return true;
}