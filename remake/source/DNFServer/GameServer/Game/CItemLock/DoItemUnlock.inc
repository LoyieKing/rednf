// ORIG 0x854231a：物品已上锁且 map 命中时置 state=2 / unlockSec=now，
// 计时器 +1 并回发剩余秒数；日志 reason=1。
// [b28d-solo-CItemLock] 本轮修法：remain 改 P198① 预清两句式（ORIG 0x8542475
// `movl $0x0,-0xc(%ebp)`）+ history receiver 内联。既有权宜：具名 uchar lock
// 局部（find 复用同槽，消除二次 GetLock 与 sete 归一）。
// [b28d-solo-CItemLock] VERDICT: AE —— P198① remain 预清两句式 + 内联 history receiver。
int CItemLock::DoItemUnlock(CUser* user, int space, int slot)
{
    Inven_Item* item =
        user->GetCurCharacInventoryRef((unsigned short)space,
                                       (unsigned short)slot);
    if (item == 0)
        return 0;
    // [b28d-41] ORIG 0x854235a：具名 uchar 局部 -0x15（`mov %al,-0x15;
    // movzbl -0x15,%eax; test; jne`），find 复用同槽；内联两次调用形产 sete
    // 归一 + 帧 +0x10。
    unsigned char lock = item->m_amp.GetLock();
    if (lock == 0)
        return 0;

    std::map<unsigned char, stItemLockInfo>::iterator it =
        m_lockMap.find(lock);
    if (it == m_lockMap.end())
        return 0;

    it->second.m_state = 2;
    it->second.m_unlockSec = GlobalData::s_systemTime_.getCurSec();

    alter();
    m_lockTimer.IncreaseItemLockTimer(user);

    // [b28d-solo-CItemLock] P198① 预清两句式：ORIG 0x8542475 在 call 前有
    // `movl $0x0,-0xc(%ebp)`（remain 预清），合写初始化式会丢该行。
    int remain = 0;
    remain = it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME -
             GlobalData::s_systemTime_.getCurSec();
    if (remain < 0)
        remain = 0;

    SendRequestItemUnlock(user, space, slot, remain);

    // [b28d-solo-CItemLock] 内联 receiver（ORIG 0x8542488 无 history 槽）。
    user->m_historyLog.ItemLockUsingLog(1, space, slot, item->m_addInfo,
                           item->m_addInfo2);
    return 1;
}