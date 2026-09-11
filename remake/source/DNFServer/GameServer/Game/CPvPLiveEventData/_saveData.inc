// [b28d-20] ORIG 0x082690d4：每处 operator<< 前调 CStreamGuard::operator*()（0x080c8c46）
// 并把返回 %eax 直接作 operator<< 的 this ⇒ 源形为 `guard.operator*()->operator<<(x)`
// （operator* 返回 CStreamGuard*，指针即 receiver）。实参跨 operator* 调用保活 ⇒ 4.4 自然
// 升格 %ebx（与 ORIG 的 mov %eax,%ebx 形一致）。
// [b28d-solo-CPvPLiveEventData] verdict=IDENTICAL/AE（ORIG 84 = OURS 84，逐条助记符全等，
// p167 调用点差集 CLEAR）；本波修法：GetUID() 调用点 (int) 收窄修重载选择（P3，
// ORIG 0x08269131 选 _ZN12CStreamGuardlsEi，原先误选 lsEj）。
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
}