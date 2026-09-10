void ProcStage::ProcSummonObjectMs::debugLog() const
{
    // b28d-solo-ProcStage：ORIG 0x0812d834 起每字段重发 operator*（P48/P157 无 CSE，
    //   共 9 次 `call …deEv`）+ 判空 `sete/jne` + cMyTrace 每轮构造即调用
    //   （P221/P37 全表达式临时链，ctor 在循环体内 -0x4c）。
    {
        // P171 死计数器照抄：ORIG 0x0812d828 的 `movl $0,-0x20` 在 begin() 之后、
        //   `addl $1,-0x20` 在增量段 ppEi 之后（0x0812d94d）⇒ 源码是
        //   `it=begin(); n=0; for(; it!=end(); it++, ++n)`（continue 也走增量段）。
        std::vector<_SummonObjectMs*>::const_iterator it = m_summons.begin();
        int n = 0;
        for (; it != m_summons.end(); it++, ++n)   // P28：ORIG 增量段 ppEi（后置）
        {
            if ((*it) == 0)
            {
                continue;
            }
            cMyTrace(__PRETTY_FUNCTION__, 0x140, 0)(
                "\tVecotr Ms = %06d ActionDefineId=%d ObjectId=%d spawndId=%d isDead=%d ObjectType=%d ObjectIndex=%d\n",
                (*it)->m_ms, (*it)->m_actionDefineId,
                (*it)->m_summon.m_id,
                static_cast<int>((*it)->m_seq),
                static_cast<int>((*it)->m_deadFlag),
                (*it)->m_summon.m_type,
                (*it)->m_summon.m_field0c);
        }
    }
    {
        // P171 死计数器照抄：ORIG 0x0812d99b `movl $0,-0x1c` 在 begin() 之后。
        std::map<short, _SummonObjectMs*>::const_iterator it = m_summonMap.begin();
        int n = 0;
        for (; it != m_summonMap.end(); it++, ++n)   // P28：ORIG 增量段 ppEi（后置）
        {
            // b28d-solo-ProcStage：ORIG 0x0812d9a7 起为 `call operator->; mov 0x4(%eax),%eax`
            //   （P167 新判据：ptEv 而非 deEv ⇒ 源写 `it->second`，每字段重发；map 侧无判空）。
            cMyTrace(__PRETTY_FUNCTION__, 0x14d, 0)(
                "\tMap Ms = %06d ActionDefineId=%d ObjectId=%d spawndId=%d isDead=%d ObjectType=%d ObjectIndex=%d\n",
                it->second->m_ms, it->second->m_actionDefineId,
                it->second->m_summon.m_id,
                static_cast<int>(it->second->m_seq),
                static_cast<int>(it->second->m_deadFlag),
                it->second->m_summon.m_type,
                it->second->m_summon.m_field0c);
        }
    }
