// ORIG WriteLog @0x0856fbb0：LOGDB_Q(#4) 投递 SIG_INSERT_RESPONSE_TIME；
// 满 100 条续包；SIG: int baseSec@0 / int count@4 /
// entries[100]@+8 每 0x14。收尾 map.clear()。
// [b28d-51] 判词 N（ORIG 0x0856fbb0，N82/N89 族）：map 迭代器 + snprintf 变参
// 装载序跨版本差；语义/调用集齐，byte 层封顶。
void CMessageCounter::WriteLog()
{
    if (m_map.empty())
        return;

    // [R29-3] 按 ORIG 0x0856fbb0 重写：删 3 个「ORIG 无槽」具名局部
    // （`StreamPool& pool` / `MsgQueueMgr& queueMgr` / `Stream* stream`，P0/P183：
    // ORIG 0x0856fbcb 直接 `mov 0x940bd6c,%eax` 作 this 即调 Acquire，无槽往返）；
    // CStreamGuard 构造用 Acquire 内联实参（ORIG 0x0856fbe8：Acquire 结果直接
    // `mov %eax,0x4(%esp)`）；`<<` 走 `(*guard)`（P132：ORIG `call CStreamGuard::deEv`
    // 先于 `call CStreamGuard::lsEi`）；GetInBuffer 走 `guard->`（P103：ORIG
    // `call CStreamGuard::ptEv` 先于 GetInBuffer）。baseSec 复用 nowSec 槽
    // （ORIG -0x10 被 `sub 余数` 改写后即 baseSec）。
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire("MessageCounter.cpp", 99),
                       false);

    (*guard)->operator<<(0x26c);
    (*guard)->operator<<(-1);

    // [N2-S5] ORIG 0x0856fcb5 起一律 `mov 0x4(%eax),%ebx` / `movl $0x0,0x4(%eax)`
    // 单条 disp；而 TU 宏的数组下标形 `((int*)s)[1]` 在 4.4 -O0 恒发射
    // `add $0x4,%eax; mov (%eax)` 两条（探针 /tmp/probe2.cpp：f1 下标形 3 条 vs
    // f2 成员访问 2 条、g1 两条 vs g2 两条直存）⇒ 本函数体改走**函数局部视图
    // 结构体**成员访问（TU 级宏/sig 视图保持不动）：+0 baseSec、+4 count、
    // entries[100] @+8 每 0x14（8+100*0x14 = 0x7d8 与 SIG 视图吻合）。
    SIG_INSERT_RESPONSE_TIME* sig =
        guard->GetInBuffer<SIG_INSERT_RESPONSE_TIME>();

    int nowSec = GlobalData::s_systemTime_.getCurSec();
    int mod = nowSec % UPDATE_PERIOD;
    nowSec = nowSec - mod;
    sig->m_baseSec = nowSec;
    sig->m_count = 0;

    for (std::map<MessageKey, MessageValue>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        // [R29-3] 删 `key`/`value`/`slot`/`idField` 四个具名局部：ORIG 每字段
        // 各重读 sig->count（4 次）并逐次 `it->first`/`it->second`（ptEv）；
        // avg 按 ORIG `fistpl`（RC=11 截断，无 ±0.5 调整）写 `(int)`。
        sig->m_entries[sig->m_count].m_id =
            ((it->first.m_id == 0)
                 ? 0u
                 : (unsigned int)(it->first.m_id + m_idBias)) +
            (unsigned int)it->first.m_size;
        sig->m_entries[sig->m_count].m_count = it->second.m_count;
        sig->m_entries[sig->m_count].m_bytes = it->second.m_bytes;
        sig->m_entries[sig->m_count].m_avg = (int)it->second.m_avg;
        ++sig->m_count;

        if (sig->m_count == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

            guard.Set(GlobalData::s_stream_pool->Acquire("MessageCounter.cpp", 0x7b));
            (*guard)->operator<<(0x26c);
            (*guard)->operator<<(-1);
            sig = guard->GetInBuffer<SIG_INSERT_RESPONSE_TIME>();
            sig->m_baseSec = nowSec;
            sig->m_count = 0;
        }
    }

    if (sig->m_count == 0)
    {
        GlobalData::s_stream_pool->Free(guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    m_map.clear();
}