// ===========================================================================
// 23. _debugCommandUserRepel —— 0x0859048e
// ===========================================================================
// [b28d-solo-DisPatcher_DebugCommand_cmds] AE：arg4/arg3 物化位置互换的钥匙 =
// `&msg.payload.nameBuf[0]`（取首元素地址 vs 数组退化改变 4.4 表达式树）——ORIG
// `mov 0xe(%eax),%edi`（m_arg→%edi）+ nameBuf 地址落槽 -0x1c；直接写 nameBuf 反致
// 两者互换（探针 r8/r9 实测）。P167 callsite CLEAR 5/5。
bool DisPatcher_DebugCommand::_debugCommandUserRepel(CUser* user,
                                                     MSG_DEBUG_COMMAND& msg)
{
    // ORIG：msg.m_arg 先求落 %edi，name 作内联实参由 4.1 落槽 -0x1c
    // （callee-saved 已被 %edi/%esi/%ebx 占满）；receiver 链式后置（P52）。
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())                                 // +0x4a GetServerProxy(group)
        ->SendUserRepel(user->get_acc_id(), user->GetUID(),
                        &msg.payload.nameBuf[0], msg.m_arg);    // +0x65 SendUserRepel
    return true;
}