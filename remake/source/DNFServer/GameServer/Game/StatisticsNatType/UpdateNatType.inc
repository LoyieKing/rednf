void StatisticsNatType::UpdateNatType()
{
    // ORIG 0860c968：入口先 _Rb_tree_iterator::C1Ev（-0x1c）⇒ 迭代器【先声明后赋值】
    // （P50/P314 复活形）；begin() 经 sret 临时 -0x2c 再拷贝进 it。
    std::map<NAT_TYPE_PEER_TO_PEER, RESULT_COUNT*>::iterator it;
    time_t now = time(0);
    it = m_counts.begin();
    // ORIG 条件块在尾部（0x0860cb61：end() -0x18 + operator!=），增量段 0x0860cb44
    // 为【后置】operator++（ppEi，sret 临时 -0x14）⇒ while + it++（P28）；
    // it->second 每处重读（ORIG 无 rc 缓存槽：条件 3 次 + 写字段 2 次 ptEv）。
    while (it != m_counts.end())
    {
        // ORIG 0x0860c9b5..0x0860cb44：null 检查落「空 then 占位岛」（0xcb43 nop）+
        // body 尾 `jmp <inc>`（P223 空块 nop 中继岛；标签置复合块外以合法跨越声明）。
        if (it->second == 0) goto L_inc;
        {
            if (it->second->m_countB > 0 || it->second->m_countA > 0)
            {
                CStreamGuard guard(
                    GlobalData::s_stream_pool->Acquire("Statistics.cpp", 0x1c7), true);

                **guard << 0x83;
                **guard << -1;
                SIG_NAT_TYPE* sig = guard->GetInBuffer<SIG_NAT_TYPE>();
                memset(sig, 0, sizeof(SIG_NAT_TYPE));
                sig->m_natType = it->first.m_a;        // ORIG 0x0860caa8（+0x00 ← m_a）
                sig->m_natTypeOther = it->first.m_b;   // ORIG 0x0860caaf（+0x04 ← m_b）
                sig->m_failCnt = it->second->m_countB;   // ORIG 0x0860cac3（+0x08）
                sig->m_successCnt = it->second->m_countA;// ORIG 0x0860cadc（+0x0c）
                sig->m_time = now;                     // ORIG 0x0860caf4（+0x10，同型直存）
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
            }
        }
    L_inc:;
        it++;
    }
    Reset();
