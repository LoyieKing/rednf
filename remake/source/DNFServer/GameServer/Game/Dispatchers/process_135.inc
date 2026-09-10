int Dispatcher_SaveGameOption_1::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    int err = check_error(user, msg);
    if (err > 0)
    {
        return err;
    }
    if (err < 0)
    {
        return -1;
    }
    // [Lane1Dispatchers 2026-09-10 · Main 判据②/P286 + P0] ORIG 0x081cea? 为
    //   `mov 0x10(%ebp),%eax; mov %eax,-0x14(%ebp)`（具名 **msg 指针**槽）+ 每处
    //   `mov -0x14(%ebp),%eax; add $0x11,%eax` 重算 m_option 地址；我方存 `&etc`
    //   则每处退化为单次 -0x18 回读且整帧 +4。另 ORIG 无 short 具名局部
    //   （`call getETCOption; test %ax,%ax`），我方具名 visible 融成
    //   `mov %ax,-0xe; cmpw $0x0,-0xe`（cmpw ×1，ORIG cmpw=0）。
    MSG_SAVE_GAME_OPTION_1& saveMsg =
        reinterpret_cast<MSG_SAVE_GAME_OPTION_1&>(msg);              // -0x14
    CETCOption& etc = saveMsg.m_option;                               // +0x11
    if ((short)etc.getETCOption((ENUM_OPTION_ETC)1) !=
        (unsigned char)user->get_charac_visible_grow_avatar())
    {
        user->set_charac_visible_grow_avatar(
            (short)etc.getETCOption((ENUM_OPTION_ETC)1) != 0);
        user->send_charac_visible_data();
        user->m_characterView.enableSaveCharacView();
    }
    CGameOption* gameOption = user->GetGameOptionRef();
    if ((unsigned char)gameOption->IsEtcOptionChanged(etc) == 1)
    {
        gameOption->SetEtcOption(etc);
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x2899),
                           true);
        // ORIG：guard.operator*()->operator<< 两次 + guard->GetInBuffer
        guard.operator*()->operator<<(0x130);
        guard.operator*()->operator<<((int)user->GetUID());
        SIG_SAVE_GAME_OPTION_1* save =
            guard->GetInBuffer<SIG_SAVE_GAME_OPTION_1>();
        *(unsigned int*)save = user->get_acc_id();
        memcpy(reinterpret_cast<char*>(save) + 4, &etc, 0x48);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
