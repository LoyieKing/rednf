int CUserPlayTimeStatisticMgr::InsertUserPlayTime()
{
    // ORIG 0x0860cd14：map 为空直接返回 1；否则按 ≤20 条/批经 LOGDB 队列
    // 写 SIG_USER_PLAY_TIME_STATISTIC（+0 计数，+4 起每条 {accId, playSec, count}）。
    // [b28d-62] P197④：ORIG 调 map::size()（非 empty()，后者 out-of-line 且无
    //   sete 段；p167 差集 MISSING size / EXTRA empty 实证）。
    // [b28d-63] 修：ORIG 0x0860cd27 `test;setne;test;je +0x282` 是 `if(size!=0){体}`
    //   正向包裹，非早返回；末尾单一 `mov $0x1,%eax`（0x0860cf96）。
    if (m_userPlayInfo.size() != 0)
    {
        // [b28d-63] 修：ORIG 0x0860cd34/0x0860cd49 先 begin()/end() 再 guard ctor
        //   （0x0860cd74）⇒ 迭代器在 guard 之前声明并直构（P13 end 缓存）。
        std::map<unsigned int, stUserPlayInfo_t>::iterator it = m_userPlayInfo.begin();
        std::map<unsigned int, stUserPlayInfo_t>::iterator itEnd = m_userPlayInfo.end();
        CStreamGuard guard(static_cast<Stream*>(0), false);
        SIG_USER_PLAY_TIME_STATISTIC* sig = 0;
        int n = 0;
        // [b28d-63] 修：ORIG 0x0860cd87 循环前 `movl $0x0,-0x10(%ebp)` ⇒ accId 是
        //   循环外具名局部（P0 槽-局部对应律），循环内赋值。
        unsigned int accId = 0;
        for (; it != itEnd; ++it)
        {
            accId = it->first;
            stUserPlayInfo_t* info = &it->second;
            if (guard.Get() != 0)
            {
                // [b28d-63] 修：ORIG 0x0860cde4 Acquire 返回值直接作 Set 实参
                //   （P0：无槽往返 ⇒ 无 `Stream* stream` 局部）。
                guard.Set(GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x24a));
                // [b28d-62] P103/P132：ORIG 走 operator* 2 次（`**guard << ...`，
                //   CStreamGuard::operator* 返回 this 指针）+ `guard->GetInBuffer<...>()`
                //   （operator-> 1 次）——p167 差集实证（旧形 `guard << ...` 0 次）。
                **guard << 0xb6;
                **guard << -1;
                sig = guard->GetInBuffer<SIG_USER_PLAY_TIME_STATISTIC>();
                memset(sig, 0, 0xf4);
            }
            sig->m_entries[n].m_accId = accId;
            sig->m_entries[n].m_playSec = info->m_playSec;
            sig->m_entries[n].m_count = info->m_count;
            n = n + 1;
            if (0x13 < n)
            {
                sig->m_count = n;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
                guard.Set(static_cast<Stream*>(0));
                n = 0;
            }
        }
        if (0 < n)
        {
            sig->m_count = n;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
            guard.Set(static_cast<Stream*>(0));
            n = 0;
        }
        m_userPlayInfo.clear();
    }
    return 1;
