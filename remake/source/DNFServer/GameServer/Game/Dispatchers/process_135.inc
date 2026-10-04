// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081ceb38 orig/ours=149/143 ndiff-blocks=30
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x18(%ebp) | cmpl   $0x0,-0x18(%ebp) | mov    -0x18(%ebp),%ebx
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
    // [L7DispG4 2026-09-11 · P0/P183] ORIG 无 `CETCOption&` 引用槽（-0x18）：全函数
    //   一律 `mov -0x14(%ebp),%eax; add $0x11,%eax` 现算 m_option 地址（08222 段
    //   0x081ceb80/0x081cebb0/0x081cec07/0x081cec2d/0x081cecdb 五处）⇒ 删具名引用。
    if ((short)saveMsg.m_option.getETCOption((ENUM_OPTION_ETC)1) !=
        (unsigned char)user->get_charac_visible_grow_avatar())
    {
        user->set_charac_visible_grow_avatar(
            (short)saveMsg.m_option.getETCOption((ENUM_OPTION_ETC)1) != 0);
        user->send_charac_visible_data();
        user->m_characterView.enableSaveCharacView();
    }
    CGameOption* gameOption = user->GetGameOptionRef();
    // [L7DispG4 2026-09-11] ORIG 0x081cec17 段 `call IsEtcOptionChanged; xor $0x1,%eax;
    //   test %al,%al; je <body>` ⇒ bool callee 裸判断形（`== 1` 数值形产 cmp+sete 两条）。
    if (gameOption->IsEtcOptionChanged(saveMsg.m_option))
    {
        gameOption->SetEtcOption(saveMsg.m_option);
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x2899),
                           true);
        // ORIG：guard.operator*()->operator<< 两次 + guard->GetInBuffer
        guard.operator*()->operator<<(0x130);
        guard.operator*()->operator<<((int)user->GetUID());
        SIG_SAVE_GAME_OPTION_1* save =
            guard->GetInBuffer<SIG_SAVE_GAME_OPTION_1>();
        save->m_mId = user->get_acc_id();
        memcpy(save->m_option, &saveMsg.m_option, 0x48);
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
    }
    return 0;
}