// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081cf1ac orig/ours=118/121 ndiff-blocks=23
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x18(%ebp) | mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp)
int Dispatcher_SaveGameOption_QuickChatting::process(CUser* user, MSG_BASE& msgBase, ParamBase& param)
{
    // [L7DispG4 2026-09-11 · 负结论] ORIG 0x081cf1ba 有 param 具名指针槽 -0x14
    //   （`mov 0x14(%ebp),%eax; mov %eax,-0x14(%ebp)`，之后从不回读；槽序
    //   msgBase -0x18 → param -0x14 → gameOption -0x10）。已试 P198 两段式
    //   死存储 `ParamBase* pp; pp = &param; (void)pp;` ⇒ 4.4 反而发 live-range
    //   存取（118→**120**，+2），不产 ORIG 的单次死存储 ⇒ 回滚。判 N（N82/P31）。
    (void)param;
    // ORIG 0x081cf1ac：保存快捷聊天选项；isLastMessage 且有变更才组 SIG 入 DB_Q。
    MSG_QUICK_CHAT& msg = *reinterpret_cast<MSG_QUICK_CHAT*>(&msgBase);
    CGameOption* gameOption = user->GetGameOptionRef();
    gameOption->SetQuickChatOption(msg.m_quick, (unsigned int)msg.m_11);
    // ORIG 0x081cf1fc 段：`test %al,%al; je <跳过>` ×2 后 `mov $0x1,%eax`/`mov $0x0,%eax`
    //   寄存器物化岛，随后 `test %al,%al; je <尾部 return 0>` —— **无 bool 具名槽**
    //   （我方原 `bool save` 落 -0x11 槽：多 movb $0x0/movb $0x1/cmpb $0x0 三条）。
    if (msg.isLastMessage() && gameOption->isChanged_QuickChatData())
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x291b), true);
        guard.operator*()->operator<<(0x239);
        guard.operator*()->operator<<(static_cast<int>(user->GetUID()));
        SIG_SAVE_GAME_OPTION_QUICKCHAT* sig = guard->GetInBuffer<SIG_SAVE_GAME_OPTION_QUICKCHAT>();
        sig->m_mId = user->get_acc_id();
        gameOption->CopyQuickChatting(*reinterpret_cast<CQuickChattingOption*>(sig->m_slotA), 0);
        gameOption->CopyQuickChatting(*reinterpret_cast<CQuickChattingOption*>(sig->m_slotB), 1);
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(2), guard);
        gameOption->resetFlag_QuickChatChnage();
    }
    return 0;
}