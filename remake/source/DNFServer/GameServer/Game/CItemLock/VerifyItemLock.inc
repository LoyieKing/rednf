// ORIG 0x8542e98：核对 map 中所有锁定/倒计时条目是否仍被 m_lockList 引用；
// 未被引用的（角色扩展数据加载不全时）退回空闲池并从 map 删除（state==2 时
// 同时 -1 计时器）；发生过修正则 alter。
// [b28d-41] AE（compare_tu 口径，离开 DIFF）：三处修复——①`state==1||state==2`
// 正向包裹复现 ORIG 0x8542ede `cmp $1; je; cmp $2; jne`（4.4 对 `!=1 && !=2` 恒
// 合并 je/je 同落点）；②`erase(it++)` 复现 0x8542f47 ppEi（P28）；③_findItem
// 返回类型 bool 复现 0x8542f0e `xor $1,%eax; test %al,%al; je`（P248 档①）。
void CItemLock::VerifyItemLock(CUser* user)
{
    if (user == 0)
        return;

    bool changed = false;
    std::map<unsigned char, stItemLockInfo>::iterator it = m_lockMap.begin();
    while (it != m_lockMap.end())
    {
        unsigned int state = (unsigned int)it->second.m_state;
        // [b28d-41] ORIG 0x8542ede：`cmp $1; je <继续块>; cmp $2; jne <++it 块>`
        // = 正极性 `state==1 || state==2` 短路；4.4 对 `!=1 && !=2` 恒合并
        // `je/je` 同落点（少一条 jne 分支），故改正向包裹。
        if (state == 1 || state == 2)
        {
            if (!_findItem(it->first))
            {
                m_lockStack.push(it->first);
                // [b28d-41] ORIG 0x8542f47 `call ppEi`（sret 临时 -0x14）⇒
                // 原码 erase(it++)（P28 后置增量）；`next=it; ++next; erase(it)`
                // 形产 ppEv 且多拷贝槽。
                m_lockMap.erase(it++);
                if (state == 2)
                    m_lockTimer.DecreaseItemLockTimer();
                changed = true;
            }
            else
            {
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }

    if (changed)
        alter();
}