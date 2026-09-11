// [b28d-L8] P30/N4 | ORIG 数组两步寻址 add 0xc(%ebp)+scaled store；4.4 折 lea(%eax,%edx,1) 型（N4/P30 封顶，ORIG 0x08557xxx）
// [b28d-solo-CMailBoxHelper] 修法（N81 翻案，P311/P288）：具名数组成员视图
// `struct DelMailArrV { unsigned int m_arr[0x15]; }` 使 4.4 生 ORIG scaled 直存
// `mov %ecx,(%eax,%edx,4)`（裸指针下标恒折 shl+lea）。残余 = N76 装载时刻：
// ORIG RHS 先算（`mov -0xc,%eax; shl; add mailIdx; mov (%eax),%ecx` 后才载 v），
// 4.4 恒先载 LHS 基址 v 到 %eax；元素 unsigned/int 两档 + `*(mailIdx+i)` 三形
// 实测均 LHS-first（90=90 insn，仅 4 条顺序差）。
void CMailBoxHelper::ReqDBRemoveMail(CUser const* user, unsigned int* mailIdx, bool a)
{
    // ORIG 0x08556772
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MailBox.cpp", 0x8ce), true);
    guard.operator*()->operator<<(0x81);
    guard.operator*()->operator<<((int)user->GetUID());
    // [2026-09-09 Lane20 修，旧 N76/N81 判词作废] 原实现经本地 MailBoxDelMailV
    // 视图 cast 访问 ⇒ 4.4 恒 base-first（P235），与 ORIG 0x0855681c
    // `mov -0xc(%ebp),%edx; mov -0xc(%ebp),%eax; shl $2,%eax; add 0xc(%ebp),%eax;
    // mov (%eax),%ecx; mov -0x10(%ebp),%eax; mov %ecx,(%eax,%edx,4)`
    // （RHS-first + scaled 直存）不符。改用权威头 SIG_MAILBOX_DEL_MAIL 的
    // m_words[0x16] 数组视角（P235 完整类型数组成员，探针 /tmp/probe_cmh/p5.cpp
    // r5 同形；索引 = 自消息首起的 dword 号，[0]=count、[0x15]=type）。
    SIG_MAILBOX_DEL_MAIL* v = guard->GetInBuffer<SIG_MAILBOX_DEL_MAIL>();
    v->m_words[0] = mailIdx[0];
    for (unsigned int i = 1; i <= mailIdx[0]; ++i)
        v->m_words[i] = *(mailIdx + i);
    v->m_type = a ? 2 : 1;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}