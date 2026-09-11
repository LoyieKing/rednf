// ORIG 0x85426a6：未锁定 → put_byte(0)；否则 put_byte(state)，state==2 时
// 再 put_int(剩余秒数 = unlockSec + 3天 - now，负值截 0)。
void CItemLock::MakeItemLockPacket(PacketGuard& packet, unsigned char b) const
{
    std::map<unsigned char, stItemLockInfo>::const_iterator it =
        m_lockMap.find(b);
    // P222 正向包裹：ORIG `test;je <then>; …; jmp end; 尾块 put_byte(0)`
    // ⇒ true 侧包块、失败块归共享尾（早返 `if(it==end){put_byte(0);return;}`
    // 会把失败块前置于 then 之前）。
    if (it != m_lockMap.end())
    {
        packet.put_byte(it->second.m_state);
        if (it->second.m_state == 2)
        {
            int remain = it->second.m_unlockSec + ITEM_UNLOCK_WAITING_TIME -
                         GlobalData::s_systemTime_.getCurSec();
            if (remain < 0)
                remain = 0;
            packet.put_int(remain);
        }
    }
    else
    {
        packet.put_byte(0);
    }
}