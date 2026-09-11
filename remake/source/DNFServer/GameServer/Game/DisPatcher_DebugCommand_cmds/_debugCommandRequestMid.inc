// ===========================================================================
// 22. _debugCommandRequestMid —— 0x08590506
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：len 具名局部落槽（ORIG -0x1c）、
// name 内联实参（ORIG %edi 无槽）+ 链式 receiver（P52）。
bool DisPatcher_DebugCommand::_debugCommandRequestMid(CUser* user,
                                                      MSG_DEBUG_COMMAND& msg)
{
    if (msg.m_arg <= 0x1d)                                      // +0x12 len ≤ 29
    {
        // ORIG：len 落槽 -0x1c（具名局部），name 落 %edi（内联实参无槽），
        // receiver 链式后置（P52）。
        int len = msg.m_arg;
        GlobalData::s_monitor_proxy_mgr->GetServerProxy(
            user->GetServerGroup())                             // +0x57 GetServerProxy(group)
            ->SendRequestUserMid(user->get_acc_id(), user->GetUID(),
                                 msg.payload.nameBuf, len);      // +0x72
    }
    return true;
}