// ORIG 0x08195b06：Guard<Mutex> 后删除 +0x3c list 中 +0x04 == get_acc_id(user) 的记录。
// [b28d-28] DIFF | ORIG 0x08195b06 | N86/P28：list erase 迭代器槽序
// （ORIG -0x18/-0x1c vs 4.4 -0x1c/-0x18）+ 比较方向（P220）；语义等价。
void Arad_EventPeriodDataManager::delUserRewardData(CUser* user)
{
    Guard<Mutex> guard(&m_mutex);
    std::list<EventRewardItemArad>::iterator it = m_rewardItemList.begin();
    while (it != m_rewardItemList.end())
    {
        // [L23-r2] ORIG 0x08195b48 每轮 `mov 0x4(%eax),%ebx; call get_acc_id;
        //   cmp %eax,%ebx`（无 accId 槽、无 CSE）⇒ P0/P48 去具名局部、实参位内联；
        //   if 体 ORIG 0x08195b5f `mov -0x18,%eax; mov %eax,-0x1c; call ppEv;
        //   call erase(旧值)` = 4.1 内联 `erase(it++)` 的拷贝+前置自增形
        //   （非 `it = erase(it)`，后者多 sret 回写 2 条）。
        if (it->m_accId == user->get_acc_id())
        {
            // ORIG 0x08195b5f..b87：`mov -0x18,%eax; mov %eax,-0x1c`（旧值落
            // 具名槽）+ 同一符号前置 operator++（0x819613c，else 分支同址调用）
            // + erase(旧值) ⇒ 原码是 tmp/++it/erase(tmp) 三句（`it++` 形会让
            // 4.4 发 out-of-line ppEi + dummy 实参多 2 条）。
            std::list<EventRewardItemArad>::iterator old = it;
            ++it;
            m_rewardItemList.erase(old);
        }
        else
        {
            ++it;
        }
    }
}