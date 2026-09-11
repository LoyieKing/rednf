// ============================================================================
// DB 连接状态同步 / 免 GameGuard 名单
// ============================================================================
void CGameManager::updateDBLinkCharacConnectState(
    unsigned int accId, unsigned int characNo,
    ENUM_CHARAC_LINK_ACTION_TYPE action)
{
    // ORIG：Acquire 结果直接作 CStreamGuard 第二实参（P52/P134 嵌套实参，
    // eax 不落栈槽）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("App.cpp", 0x125b), true);
    (**guard) << 0x1e5;
    (**guard) << -1;
    SIG_UPDATE_LINK_CHARAC_CONNECT_STATE* info =
        guard->GetInBuffer<SIG_UPDATE_LINK_CHARAC_CONNECT_STATE>();
    memset(info, 0, 0xc);
    info->m_accId = accId;
    info->m_characNo = characNo;
    info->m_action = (int)action;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}