// ORIG 0x85424c2：物品已上锁且 map 命中时置 state=1 / unlockSec=now，
// 计时器 -1；日志 reason=2。
// [b28d-solo-CItemLock] 本轮修法：history receiver 内联（去多余具名槽，帧
// 0x44→0x34 与 ORIG 一致）。既有权宜：具名 uchar lock 局部 + find 复用同槽。
// [b28d-solo-CItemLock] VERDICT: AE —— 内联 history receiver（去多余具名槽）。
int CItemLock::DoItemUnlockCancel(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    // [b28d-41] ORIG 0x85424f0：`call GetLock; mov %al,-0x11; movzbl -0x11,%eax;
    // test %al,%al; jne` ⇒ 具名 uchar 局部（P0 槽往返），且 find 复用同一槽
    // （`lea -0x11,%edx`）；内联两次调用形多一次 GetLock 调用 + 帧 +0x10。
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0;

    std::map<unsigned char, stItemLockInfo>::iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end())
        return 0;

    it->second.m_state = 1;
    it->second.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    alter();
    m_lockTimer.DecreaseItemLockTimer();

    SendRequestItemUnlockCancel(user, space, slot);

    // [b28d-solo-CItemLock] 内联 receiver（ORIG 0x85425e5 无 history 槽）。
    ((cUserHistoryLog*)((char*)user + 0x79700))
        ->ItemLockUsingLog(2, space, slot, item->m_addInfo,
                           item->m_addInfo2);
    return 1;
}