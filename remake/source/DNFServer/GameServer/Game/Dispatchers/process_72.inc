// b28d-solo-Dispatchers verdict=DIFF(routed) [N82] ORIG 0x081e3dca orig/ours=178/165 ndiff-blocks=22
//   帧差 sub $30 vs $40 + 槽号漂移（4.1 不按 lifetime 复用栈槽）
//   首差: sub    $0x30,%esp | mov    0x14(%ebp),%eax | mov    %eax,-0x14(%ebp)
int Dispatcher_SelectStriker::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
    // [DispLaneB 2026-09-11 实测回滚] ORIG 直证：入口先缓存 param（-0x14）、msg 形参
    //   直用（0x10(%ebp) 6 次）。但把 msg 引用改编译期内联式后实测 ndiff 14→17
    //   （ours 174→172）劣化 ⇒ 按 codegen 轴闸门回滚（同族第 3 例）。
    const MSG_SELECT_STRIKER& m = reinterpret_cast<const MSG_SELECT_STRIKER&>(msg);
    int err = check_error(user, msg);
    *reinterpret_cast<int*>(reinterpret_cast<char*>(&param) + 0x4) = err;   // ParamBase+0x04
    if (err >= 1)
    {
        return 0;
    }
    if (err < 0)
    {
        return LineFunc(0x5b93,
                        __PRETTY_FUNCTION__,
                        err, 0);
    }
    char sel = (char)m.m_0d;
    // [b28d-solo-Dispatchers] P183：ORIG 0x081e3e60/0x081e3e8e/0x081e3ee7/0x081e3ffd
    //   4 处各自完整重发 `user->m_characList[sel]`（p167 MISSING
    //   `_ZNSt6vectorI12_Charac_infoSaIS0_EEixEj` ×3 即此项）；旧码 entry 引用缓存 ⇒ 删。
    if (user->m_characList[sel].m_field14BC == 0)               // +0x14bc（ORIG movzbl 判 0）
    {
        // [DispA2 P0/P183] ORIG 0x081e3f3f..0x081e3f6e：Acquire 回值直接进
        //   CStreamGuard ctor 的 0x4(%esp)（无 stream 具名槽）。
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                               "PacketDispatcher_Impl_1.cpp", 0x5ba6),
                           true);
        **guard << 0x2fe;
        **guard << (int)user->GetUID();   // ORIG 0x081e3f89 调 lsEi（非 lsEj）
        SIG_REQUEST_TAG_CHARAC_INFO* sig = guard->GetInBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
        memset(sig, 0, 0x414c);
        sig->m_characNo =
            user->m_characList[sel].m_characNo;
        sig->m_pad04 =
            m.m_0e;
        // ORIG 0x081e402d: s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard)
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        return 0;
    }
    user->setCurCharacTagCharacNo(user->m_characList[sel].m_characNo);
    user->setCurCharacStrikerSkillIndex(
        m.m_0e);
    user->m_characterView.enableSaveCharacView();               // +0x796f4
    user->setTagCharac(&user->m_characList[sel]);
    // [DispA2 P0/P183] ORIG 0x081e3efb/0x081e3f06 与 0x081e3f15/0x081e3f20：
    //   getTagCharacInvenR/getTagCharacSkillR 回值直作 set_parent 的 receiver。
    user->getTagCharacInvenR()->set_parent(user);
    user->getTagCharacSkillR()->set_parent(user);
    user->SendTagCharacInfo();
    return 0;
}