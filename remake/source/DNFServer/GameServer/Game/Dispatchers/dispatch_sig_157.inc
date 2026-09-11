// b28d-solo-Dispatchers verdict=DIFF(routed) [N7] ORIG 0x0825fc3a orig/ours=169/178 ndiff-blocks=36
//   虚/间接调用 callee 与目标寄存器角色互换（4.4 恒目标 %eax；N7 限于 4.4.7 后端）
//   首差: sub    $0x40,%esp | mov    $0xa49,%eax | mov    $0x0,%eax
int Dispatcher_LevelupSupportReqestItem::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0825fc3a（docs/class_func_reports/Dispatcher_LevelupSupportReqestItem/dispatch_sig.md）
    // [L7DispG1 2026-09-11] 预清逐点核：ORIG 在 get_int 前有 `movl $0x0,-0x10(%ebp)`（本例
    //   唯一一处预清）⇒ requestLevel 带 `= 0`；err 只在 +0xf9 段显式赋 0（预清可去）。
    int requestLevel = 0;                     // local_14 -0x10
    ENUM_ERROR err;                           // local_18 -0x14
    int stepIdx;                              // local_10 -0xc

    // +0x00：user 为空 → 0xa49
    if (user == 0)
        return (int)0xa49;
    // +0x18：state < 3 → 0
    if (user->get_state() < 3)
        return 0;

    // +0x37：0x9e 重复事件未进行中 → 错误 1 + 日志（ORIG 无判空直调 vptr+0x34 IsEventing(0)）
    // [L7DispG1] ORIG 0x0825fc8x 段：`mov (%eax),%edx; add $0x34,%edx; mov (%edx),%edx`
    //   直接以 GetRepeatEvent 的返回（%eax）做虚调用 ⇒ 无具名 event 指针槽（我方 -0x14
    //   多 1 槽 + 3 条槽往返）。历史判词「无判空直调 vptr+0x34」与此同源 ⇒ 全内联链。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x9e)->IsEventing(0))
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
        // [G1Csig] g1_calls 实证：ORIG 调用序为 Singleton::Get ×1 → GetStepIndex → InsertItem
        // （我方原写两次 Get ⇒ p167 EXTRA `_ZN4ARAD9SingletonI25LevelupSupportEventMangerE3GetEv` ×1）
        // ⇒ 复用已缓存的 mgr（ORIG 确为具名 mgr：Get 只 1 次）。
        stepIdx = mgr->GetStepIndex(requestLevel);  // 0x08147362
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
}