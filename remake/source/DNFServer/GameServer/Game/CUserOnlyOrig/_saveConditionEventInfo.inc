// ---------------------------------------------------------------------------
// 14) CUser::_saveConditionEventInfo(uint, short, short)
//     报告：docs/class_func_reports/CUser/_saveConditionEventInfo.md。ORIG 0x086819fa。
// ---------------------------------------------------------------------------
void CUser::_saveConditionEventInfo(unsigned int value, short a2, short a3)
{
    // [b28d-22 修法 P0/P48/P259] ORIG 0x08681a20-0x08681a34 免槽直调虚函数
    // （`call GetRepeatEvent; mov (%eax),%edx; add $0x34,%edx; call *%edx` 无 ev 槽）；
    // 0x08681a39 `xor $1; test %al; jne <ret>` 为内联取反早返形；
    // 0x08681a5c Acquire 结果免 s 槽直落 guard ctor（探针 /tmp/p22_g2_c.cpp b）。
    if (!GlobalData::s_event_manager->GetRepeatEvent(0x71)->IsEventing((CUser*)0))
        return;
    {
        CStreamGuard guard(
            GlobalData::s_stream_pool->Acquire("user.cpp", 0x8556), true);
        // ORIG：guard.operator*()->operator<< 两次 + guard.operator->()->GetInBuffer
        // 显式代理链（operator*/operator-> 调用点补齐）。
        guard.operator*()->operator<<((int)0x19f);
        guard.operator*()->operator<<((int)GetUID());
        SIG_SAVE_CONDITION_EVENT* sigBody =
            guard.operator->()->GetInBuffer<SIG_SAVE_CONDITION_EVENT>();
        sigBody->m_value = value;                 // +0x00 uint
        sigBody->m_a2 = a2;                       // +0x04 short
        sigBody->m_a3 = a3;                       // +0x06 short
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    }
}