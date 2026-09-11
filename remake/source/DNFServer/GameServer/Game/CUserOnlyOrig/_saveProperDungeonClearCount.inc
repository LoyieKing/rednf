// ---------------------------------------------------------------------------
// 15) CUser::_saveProperDungeonClearCount(uint, short)
//     报告：docs/class_func_reports/CUser/_saveProperDungeonClearCount.md。ORIG 0x08681b3a。
// ---------------------------------------------------------------------------
void CUser::_saveProperDungeonClearCount(unsigned int dungeonIdx, short subNo)
{
    // ORIG 0x08681b66：`call Acquire; movl $1,0x8(%esp); mov %eax,0x4(%esp);
    // lea -0x14(%ebp)` = Acquire 结果免槽直落 ctor 实参（技巧37）；具名 s 多 spill+
    // reload 两行并把 guard 槽挤到 -0x18。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x85cd), true);
    guard.operator*()->operator<<(0x1d4);
    guard.operator*()->operator<<((int)GetUID());
    SIG_SAVE_BLOOD_CLEAR_COUNT* bloodBody =
        guard.operator->()->GetInBuffer<SIG_SAVE_BLOOD_CLEAR_COUNT>();
    bloodBody->m_dungeonIdx = dungeonIdx;           // +0x00
    bloodBody->m_subNo = subNo;                     // +0x04
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}