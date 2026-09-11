// ORIG T 0x0864870e sz0xdc：LOGDB_Q(=4)：<<0xcc、<<-1、<<reason。
// b28d-lane5 N封顶: N82/P112 槽号分配 | ORIG 0x0864870e | _ZN5CUser29SendClientCrashDownLogToLogDBEi
// b28d-solo-CUser：返回类型 void→int（CUser.h:3228 Main 代落）——ORIG 尾
//   `mov $0x1,%ebx; …; mov %ebx,%eax; ret` 返回值 1（P159 尾形判返回类型）。
int CUser::SendClientCrashDownLogToLogDB(int reason)
{
    // b28d-lane5：ORIG 0x0864870e 无 Stream* 槽——Acquire 结果经 %eax 直传 ctor
    // （P0 右值内联；同 _UpdateUserInfo4Guild）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("user.cpp", 0x8ac), true);
    guard.operator*()->operator<<(0xcc);
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(reason);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    return 1;
}