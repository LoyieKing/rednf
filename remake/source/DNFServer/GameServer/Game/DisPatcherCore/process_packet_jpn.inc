// ORIG 0x081cb30a T _ZN20DisPatcher_AddFriend18process_packet_jpnEP5CUserR8MSG_BASER9ParamBase
// （docs/class_func_reports/DisPatcher_AddFriend/process_packet_jpn.md 全文转写）。
// m_field4=check_error；>0 返 0；<0 logFormat(1,file,pretty,0x20d8,
// "Dispatcher_Impl Check_Error No(%d)",err) 返 0x20d9；==0：StreamPool 取流构造
// CStreamGuard → <<0x33e <<GetUID → GetInBuffer<SIG_CHECK_ADD_FRIEND> 判空置 m_field4=1；
// [b28d-29] N82/P211：store 装载序 + 临时对象块位置（P183 反向），调用点全等  (ORIG 0x081cb30a insn 126->125)
// 否则 sig[+4]=!isGM(acc_id)、strncpy(sig+5, msg+0x11, 0x1e)、MsgQueueMgr put(DB_Q)；返 0。
// [b28d-solo-DisPatcherCore] 复核 AE：ORIG 126 / ours 126 insn 全等（AE 口径，重定位归一）。
int DisPatcher_AddFriend::process_packet_jpn(CUser* user, MSG_BASE& msg, ParamBase& param)

{
    stAddFriendParam& out = static_cast<stAddFriendParam&>(param);
    // [b23S3C] ORIG 槽集 {-0x10 req, -0x14 out, -0x1c guard, -0xc sig}；
    // kFile/kPretty 各用点均字符串立即数（无 static 局部指针槽）；
    // Acquire 结果直落 ctor 0x4(%esp)（无 stream 局部，P0 寄存器直传）；
    // 写流为 operator* + operator<< 两 call 链（+0x0c7/+0x0ef 各一次），
    // GetInBuffer 走 operator-> 链（+0x10c/+0x114，P103）——GmCmds_C
    // CDeleteGuild DONE 同族形。

    out.m_field4 = check_error(user, msg);
    // [N8BigTURest 2026-09-11] 本 TU 的 prett 缺口审计（nm 逐符号差集）：
    // ① `_ZZN20DisPatcher_AddFriend18process_packet_jpn…E19__PRETTY_FUNCTION__` 原缺
    //    ⇒ 已把下方手写串（与 ORIG .rodata 实字节逐字节相同）改为 __PRETTY_FUNCTION__，
    //    符号补齐且串内容不变（编译后 .rodata 实读一致）。
    // ② `_ZZN20DisPatcher_AddFriend7process…E19__PRETTY_FUNCTION__`（ORIG 0x8bd5080）
    //    在 ORIG 全二进制内**零引用**（对全库 disasm 搜 `$0x8bd5080` = 0 hit）⇒ 4.1
    //    发射的死符号；我方 process 体 14 条与 ORIG 逐条相同（fdiff blocks=0）且无串引用
    //    ⇒ 不可补（补=发明代码），判 N。
    if (out.m_field4 > 0)
    {
        return 0;
    }
    if (out.m_field4 < 0)
    {
        LogManager::logFormat(
            1, "PacketDispatcher_Impl_1.cpp",
            __PRETTY_FUNCTION__,
            0x20d8, "Dispatcher_Impl Check_Error No(%d)", out.m_field4);
        return 0x20d9;
    }

    MSG_ADD_FRIEND& req = *reinterpret_cast<MSG_ADD_FRIEND*>(&msg);
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PacketDispatcher_Impl_1.cpp", 0x20ed),
        true);
    (*guard)->operator<<(0x33e);                 // inter-server SIG 消息号
    (*guard)->operator<<(static_cast<int>(user->GetUID()));
    SIG_CHECK_ADD_FRIEND* sig =
        guard.operator->()->GetInBuffer<SIG_CHECK_ADD_FRIEND>();
    if (sig == 0)
    {
        out.m_field4 = 1;
        // [b28d-solo-DisPatcherCore] ORIG +0x12c `movl $0x1,0x4(%eax); mov $0x0,%ebx;
        // jmp <尾块>`（返回值物化到 %ebx 后跳共享尾析构块）⇒ if 块内是独立 return 0。
        return 0;
    }
    else
    {
        // ORIG +0x153 分支岛 mov$0/mov$1（非 setne 归一），直落 sig+0x04。
        sig->m_notGmFlag =
            GlobalData::s_pGMAccounts_->isGM(user->get_acc_id()) ? 0 : 1;
        strncpy(sig->m_characName,                               // sig+0x05..0x22
                reinterpret_cast<MSG_ADD_FRIEND*>(&req)->m_name, // msg+0x11
                0x1e);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);    // QUEUE_IDX 2=DB_Q
    }
    return 0;
}
