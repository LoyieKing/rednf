int Dispatcher_SaveGameOption_QuickChatting::process(CUser* user, MSG_BASE& msgBase, ParamBase& param)
{
    (void)param;
    // ORIG 0x081cf1ac：保存快捷聊天选项；isLastMessage 且有变更才组 SIG 入 DB_Q。
    MSG_QUICK_CHAT& msg = *reinterpret_cast<MSG_QUICK_CHAT*>(&msgBase);
    CGameOption* gameOption = user->GetGameOptionRef();
    gameOption->SetQuickChatOption(msg.m_quick, (unsigned int)msg.m_11);
    bool save = false;
    if (msg.isLastMessage() && gameOption->isChanged_QuickChatData())
    {
        save = true;
    }
    if (save)
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x291b), true);
        guard.operator*()->operator<<(0x239);
        guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
        SIG_SAVE_GAME_OPTION_QUICKCHAT* sig = guard->GetInBuffer<SIG_SAVE_GAME_OPTION_QUICKCHAT>();
        char* buf = reinterpret_cast<char*>(sig);
        // SIG_SAVE_GAME_OPTION_QUICKCHAT 共享头仅 m_pad[0x25c]；§8.3 本 TU 具名视图
        // （证据=ORIG 0x081cf0xx：accId@0、快捷栏 A@4、快捷栏 B@0x130，各 0x12c）。
        struct QuickChatSigView
        {
            int m_accId;                      // +0x000
            CQuickChattingOption m_slotA;     // +0x004（sizeof 0x12c）
            CQuickChattingOption m_slotB;     // +0x130
        };
        static_assert(sizeof(QuickChatSigView) == 0x25c, "QuickChatSigView size");
        *reinterpret_cast<int*>(buf + 0) = static_cast<int>(user->get_acc_id());
        gameOption->CopyQuickChatting(*reinterpret_cast<CQuickChattingOption*>(buf + 4), 0);
        gameOption->CopyQuickChatting(
            reinterpret_cast<QuickChatSigView*>(sig)->m_slotB, 1);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        gameOption->resetFlag_QuickChatChnage();
    }
    return 0;
