// ORIG 0x081c727c T _ZN23DisPatcher_SelectCharac15RequestDBTicketEP5CUserR17MSG_CHARAC_SELECT
// （报告 RequestDBTicket.md）：opcode 0xb / uid → DisableCharacInfo(4) →
// 有效最后登录角色时追加 channelNo/characNo → SIG_LOAD_TICKET(0x19c 清零，
// +0=get_charac_no(msg.m_slot)) → 有效登录 put Q5 否则 Q2。
// SIG 缓冲首字 = charac_no（SigTypes_C.h SIG_LOAD_TICKET::m_characNo 权威字段；
// 视图 cast 已按 P0「无具名局部」律删除：ORIG 该 store 为右值内联直写）。
// [b28d-29] N93附/P222：同 RequestDBSkill 尾块布局差（4.4 就地 epilogue vs 4.1 jmp 尾岛）  (ORIG 0x081c727c insn 120->119)
// [b28d-solo-DisPatcherCore] 复核 DIFF：ndiff 55（ORIG 120 / ours 123），残差=P211/N18a 中转/store 序/P30/N4 槽与寻址/N82 帧差；沿用上方 b28d-29 归因，封顶。
int DisPatcher_SelectCharac::RequestDBTicket(CUser* user, MSG_CHARAC_SELECT& msg)

{
    // [b28c-L9] fresh 复验 ndiff 21→18；p167 25=25 全清。残差同 RequestDBCharac：
    // guard D1 共享尾块与 unwind 块的相对发射序（4.1 `jmp tail` / 4.4 fall-through）
    // ⇒ 跨版本 EH 块序 N，语义完成。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "PacketDispatcher_Impl_1.cpp", 0x17db), true);
    **guard << 0xb;
    **guard << static_cast<int>(user->GetUID());
    user->DisableCharacInfo((CACHE_SECTION)4);
    if (user->IsValidLastLoginCharac())
    {
        **guard << user->GetLastLoginChannelNo();
        **guard << user->GetLastLoginCharacNo();
    }
    SIG_LOAD_TICKET* sig = guard->GetInBuffer<SIG_LOAD_TICKET>();
    memset(sig, 0, sizeof(SIG_LOAD_TICKET));
    // ORIG 081c73a6/081c73ab（报告 RequestDBTicket.md）：`call get_charac_no;
    // mov %eax,%edx; mov -0xc(%ebp),%eax; mov %edx,(%eax)`——无 vNo 局部、
    // 无视图指针，get_charac_no 右值经 %edx 直落 sig 首字（P0 双向对应律）。
    sig->m_characNo = static_cast<unsigned int>(
        user->get_charac_no(static_cast<int>(static_cast<signed char>(msg.m_slot))));
    // [C05-① 复查] ORIG 0x081c73dd/0x081c7400 两分支均为 `mov $0x1,%ebx; jmp <共享尾块>`
    // （尾块 0x081c7422 = guard D1Ev + `mov %ebx,%eax` + epilogue），且**全函数无 res 栈槽**
    // ⇒ 原码是两分支各自 `return 1;`（不引入具名 res 局部，P0）；4.1 用 %ebx 汇聚返回值。
    if (user->IsValidLastLoginCharac())
    {
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)5, guard);
        return 1;
    }
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return 1;
}
