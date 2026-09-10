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
