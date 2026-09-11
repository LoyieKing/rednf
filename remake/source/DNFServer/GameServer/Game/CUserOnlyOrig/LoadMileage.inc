// ----------------------------------------------------------------------------
// ORIG 0x08650722 CUser::LoadMileage() void  (报告: CUser/LoadMileage.md)
// StreamPool->Acquire("user.cpp",0x18f2); guard<<0x1fd; <<uid;
// SIG_MILEAGE_STRUCT{accId, characNo, mileage=0}; msgq put(QUEUE_IDX=2)
void CUser::LoadMileage()
{
    // ORIG 0x08650722+0x??：Acquire 结果 %eax 直落 guard ctor 0x4(%esp)，无 s 槽
    // （技巧36/37：删中转变量整帧平移，guard 槽 -0x18→-0x14 对齐 ORIG）
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x18f2), true);
    guard.operator*()->operator<<((int)0x1fd); // 流类型
    guard.operator*()->operator<<((int)GetUID());
    SIG_MILEAGE_STRUCT* u = guard.operator->()->GetInBuffer<SIG_MILEAGE_STRUCT>();
    // 具名字段 disp 折叠形（ORIG `mov -0xc,%edx; mov (%edx)` / `mov %eax,0x4(%edx)` /
    // `movl $0x0,0x8(%eax)`）
    u->m_accId = get_acc_id();
    u->m_characNo = getCurCharacNo();
    u->m_mileage = 0;                          // mileage 由 DB 侧维护
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)2, guard);
}