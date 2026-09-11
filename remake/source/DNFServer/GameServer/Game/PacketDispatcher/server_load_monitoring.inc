// ORIG 0x825f658：五队列余量 → CAppLoadChecker 检查/请求 DB。
void PacketDispatcher::server_load_monitoring()
{
    int remainQ3 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)3);
    int remainQ1 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)1);
    int remainQ0 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)0);
    int remainQ2 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)2);
    int remainQ4 = GlobalData::s_msgq_mgr->remain((MsgQueueMgr::QUEUE_IDX)4);
    // ORIG 0x0825f6d6/0x0825f703：两次独立 CAppLoadCheckerInstance 结果免槽直落
    // (%esp)（具名 checker 槽占 -0x24 使 5 个 remain 槽整体顶深 4B，技巧36/37）。
    if (CAppLoadCheckerInstance()->CheckLoad(remainQ2, remainQ4, remainQ0, remainQ1))
        CAppLoadCheckerInstance()->RequestDB(remainQ2, remainQ4, remainQ0, remainQ1);
}