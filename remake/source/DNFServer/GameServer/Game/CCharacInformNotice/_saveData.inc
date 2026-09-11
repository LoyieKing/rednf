// [b22判词·N-codegen] _saveData 语义逐条忠实 ORIG 0x080e4b52（报告汇编+Ghidra C 全读）：
// Acquire(s_stream_pool,"CharacInformNotice.cpp",0x77) → CStreamGuard(,true) →
// operator*()->operator<<(0x311) → operator*()->operator<<(GetUID)（GetUID 先求值）→
// operator->()->GetInBuffer<SIG> → 空则返 false → memset(sig,0,0x14) →
// sig->m_characNo=get_charac_no(-1) → memcpy(sig+4,this+5,0x10) → s_msgq_mgr->put(DB_Q=2,guard)
// → 返 true（含 CStreamGuard EH 析构岛 + _Unwind_Resume）。orig87/our86 唯一真差：
// `*sig=get_charac_no` 存储序列 ORIG 走 `mov %eax,%edx; mov sig,%eax; mov %edx,(%eax)`
// （结果先让位 %edx 再载 sig，4 条），4.4.7 -O0 走 `mov sig,%edx; mov %eax,(%edx)`
// （结果留 %eax，3 条）——跨版本寄存器分配次序差，源码层无法稳定复现（试过结果具名
// 临时量：4.4 落槽反多 2 条，已回退）。其余 diff（call 目标/分支偏移/数据地址
// 0x940bd6c、0x8b2dd20、s_msgq_mgr）全属未 relink .o 重定位占位，relink 后转 AE。语义等价。
bool CCharacInformNotice::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("CharacInformNotice.cpp", 0x77), true);
    // ORIG（idx51 报告）：每次 << 先 operator*()（de）取流再 operator<<；GetUID
    // 先求值（ORIG +0x057 call GetUID → +0x064 operator* → +0x076 operator<<）。
    guard.operator*()->operator<<(0x311);
    guard.operator*()->operator<<((int)user->GetUID());
    SIG_SAVE_CHARAC_INFORM_NOTICE* sig =
        guard->GetInBuffer<SIG_SAVE_CHARAC_INFORM_NOTICE>();
    if (sig == 0)
        return false;
    memset(sig, 0, 0x14);
    sig->m_characNo = user->get_charac_no(-1);
    memcpy(sig->m_flags, m_flags, 0x10);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
}