int Dispatcher_LevelupSupportReqestItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0825fc3a（docs/class_func_reports/Dispatcher_LevelupSupportReqestItem/dispatch_sig.md）
    int requestLevel = 0;                     // local_14 -0x10
    ENUM_ERROR err = (ENUM_ERROR)0;           // local_18 -0x14
    int stepIdx;                              // local_10 -0xc

    // +0x00：user 为空 → 0xa49
    if (user == 0)
        return (int)0xa49;
    // +0x18：state < 3 → 0
    if (user->get_state() < 3)
        return 0;

    // +0x37：0x9e 重复事件未进行中 → 错误 1 + 日志（ORIG 无判空直调 vptr+0x34 IsEventing(0)）
    CEventBase* repeatEvent = GlobalData::s_event_manager->GetRepeatEvent(0x9e);
    if (!repeatEvent->IsEventing(0))
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x255, 1);
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_2.cpp",
            __PRETTY_FUNCTION__,
            0xa50, "charac_no:%d, error:%d",
            user->getCurCharacNo(), 1);
        return 0;
    }

    // +0xcf：读请求等级
    if (!packet.get_int(requestLevel))
        return (int)0xa56;

    // +0xf9：档位判定
    err = (ENUM_ERROR)0;
    {
        LevelupSupportEventManger* mgr = ARAD::Singleton<LevelupSupportEventManger>::Get();
        stepIdx = ARAD::Singleton<LevelupSupportEventManger>::Get()->GetStepIndex(requestLevel);  // 0x08147362
        if (stepIdx == -1)
            err = (ENUM_ERROR)1;
        // +0x124：user->m_levelupEvents[stepIdx].m_state（+0x02）
        LevelupSupportEventManger::EventInfo& info =
            user->m_levelupEvents[stepIdx];
        // [Lane1Dispatchers 2026-09-10 · Main 判据②/P286 + P0] ORIG 0x0825fc3a 段为
        //   `movzwl 0x2(%eax),%eax; movzwl %ax,%eax; cmp $0x1,%eax; cmp $0x2,%eax;
        //   test %eax,%eax` —— **无具名 state 局部**（P0：值全程寄存器、零槽往返），
        //   各分支各自重读 + `(unsigned short)` 截断 ⇒ 寄存器比较；带具名 ushort 局部
        //   时 4.4 恒融 `cmpw $K,-0xa(%ebp)`（我方 cmpw×3、ORIG cmpw=0），改 switch 则
        //   4.4 走跳表（实测 +11 条）。正解 = 内联 cast 表达式逐次重读。
        if ((unsigned short)info.m_state == 1)
        {
            mgr->InsertItem(user, stepIdx, err);   // 0x081473cc
        }
        else if ((unsigned short)info.m_state == 2)
        {
            err = (ENUM_ERROR)0x12;
        }
        else if ((unsigned short)info.m_state == 0)
        {
            err = (ENUM_ERROR)0x13;
        }
    }

    // +0x17c：err==0 → 应答包；否则错误包 + 日志
    if (err == (ENUM_ERROR)0)
    {
        PacketGuard guard;                                            // 0858dd4c
        guard.clear();                                                // 080cb8e6
        guard.put_header(1, 0x255);
        guard.put_byte(1);
        guard.put_int(requestLevel);
        guard.finalize(true);
        user->Send(guard);
        return 0;                                                     // guard 析构 0858de80
    }

    user->SendCmdErrorPacket((ENUM_CMDPACKET)0x255, (unsigned char)err & 0xff);
    LogManager::logFormat(
        1, "PacketDispatcher_Impl_2.cpp",
        __PRETTY_FUNCTION__,
        0xa78, "charac_no:%d, request_level:%d, error:%d",
        user->getCurCharacNo(), requestLevel, (unsigned int)err);
    return 0;
