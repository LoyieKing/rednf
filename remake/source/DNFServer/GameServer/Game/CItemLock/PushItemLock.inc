// ORIG 0x8542a8a：
//   新条目 state==0                → 号回收进空闲池；
//   state==2 且已过等待期          → 清 info 后回收 + alter；
//   其余                           → 继续计时再入 map（state==2 时续上计时器）。
// [b28d-solo-CItemLock] VERDICT: AE —— insert 实参内联直构（无具名 value 临时）。
void CItemLock::PushItemLock(CUser* user, unsigned char lock,
                             stItemLockInfo* info)
{
    if (info->m_state == 0)
    {
        m_lockStack.push(lock);
        return;
    }

    if (info->m_state == 2)
    {
        if (info->m_unlockSec <=
            GlobalData::s_systemTime_.getCurSec() - ITEM_UNLOCK_WAITING_TIME)
        {
            info->m_state = 0;
            info->m_unlockSec = 0;
            m_lockStack.push(lock);
            alter();
            return;
        }
        m_lockTimer.IncreaseItemLockTimer(user);
    }

    // [b28d-solo-CItemLock] AE：旧 NEAR 残差（tmp/value/ret 三槽置换，判 N82）
    // 作废 —— 真因是原码**无具名 value**、insert 实参为内联直构临时：
    // ORIG 0x8542b2d make_pair@-0x14 → converting ctor(pair&&)@-0x20 →
    // insert(const&) sret@-0x28（探针 /tmp/b28d_solo_CItemLock/probe_pair.cpp
    // fB 逐条同形；具名 value 形 fA 恰产我方旧槽序）。语义等价（返回值本即丢弃）。
    m_lockMap.insert(
        std::pair<std::map<unsigned char, stItemLockInfo>::key_type,
                  std::map<unsigned char, stItemLockInfo>::mapped_type>(
            std::make_pair(lock, *info)));
}