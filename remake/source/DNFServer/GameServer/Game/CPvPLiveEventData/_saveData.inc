bool CPvPLiveEventData::_saveData(CUser* user)
{
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire("PvPLiveEventData.cpp", 0x1e), true);
    guard.operator*()->operator<<(0x1e1);
    // [b28d-solo-CPvPLiveEventData] ORIG 0x08269131 选 `operator<<(int)`（_ZN12CStreamGuardlsEi）：
    // CUser::GetUID() 声明为 unsigned int（CUser.h 冻结），调用点按 P3 显式收窄为 int 复现重载选择
    // （同宽转换零指令，p167 差集 MISSING lsEi/EXTRA lsEj 归零）。
    guard.operator*()->operator<<((int)user->GetUID());
    guard.operator*()->operator<<((char)user->GetServerGroup());   // ORIG lsEc（movsbl 符号扩展，写 1 字节）
    guard.operator*()->operator<<(user->getCurCharacNo());
    guard.operator*()->operator<<(m_playCount);
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
    return true;
