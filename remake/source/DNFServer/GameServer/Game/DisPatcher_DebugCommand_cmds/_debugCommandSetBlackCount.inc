// ===========================================================================
// 15. _debugCommandSetBlackCount —— 0x0858fcc4
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE + 真语义 bug 修复：DbgCmdArgWordView
// 缺 pack(1) 使 m_argU 自然对齐到 +0x10（读到 msg+0x10 而非 ORIG `mov 0xe(%eax),%edx`
// 的 +0x0e）⇒ 补 pack(1)+static_assert，读偏移归位。
bool DisPatcher_DebugCommand::_debugCommandSetBlackCount(CUser* user,
