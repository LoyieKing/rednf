int Dispatcher_SelectStriker::process(CUser* user, MSG_BASE& msg, ParamBase& param)
{
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
        Stream* stream = GlobalData::s_stream_pool->Acquire(
            "PacketDispatcher_Impl_1.cpp", 0x5ba6);
        CStreamGuard guard(stream, true);
        **guard << 0x2fe;
        **guard << (int)user->GetUID();   // ORIG 0x081e3f89 调 lsEi（非 lsEj）
        SIG_REQUEST_TAG_CHARAC_INFO* sig = guard->GetInBuffer<SIG_REQUEST_TAG_CHARAC_INFO>();
        memset(sig, 0, 0x414c);
        *reinterpret_cast<int*>(sig->m_pad) =
            user->m_characList[sel].m_characNo;   // sig+0x00 = entry 首字段
        *reinterpret_cast<char*>(sig->m_pad + 0x4) =
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
    CInventory* inven = user->getTagCharacInvenR();
    inven->set_parent(user);
    SkillSlot* skill = user->getTagCharacSkillR();
    skill->set_parent(user);
    user->SendTagCharacInfo();
    return 0;
