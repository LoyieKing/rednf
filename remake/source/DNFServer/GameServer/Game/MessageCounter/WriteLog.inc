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

    char* sig = reinterpret_cast<char*>(
        guard->GetInBuffer<SIG_INSERT_RESPONSE_TIME>());

    int nowSec = GlobalData::s_systemTime_.getCurSec();
    int mod = nowSec % UPDATE_PERIOD;
    nowSec = nowSec - mod;
    sig_init_head(sig, nowSec);

    for (std::map<MessageKey, MessageValue>::iterator it = m_map.begin();
         it != m_map.end(); ++it)
    {
        // [R29-3] 删 `key`/`value`/`slot`/`idField` 四个具名局部：ORIG 每字段
        // 各重读 sig->count（4 次）并逐次 `it->first`/`it->second`（ptEv）；
        // avg 按 ORIG `fistpl`（RC=11 截断，无 ±0.5 调整）写 `(int)`。
        sig_write_entry_id(sig, sig_entry_count(sig),
                           ((it->first.m_id == 0)
                                ? 0u
                                : (unsigned int)(it->first.m_id + m_idBias)) +
                               (unsigned int)it->first.m_size);
        sig_write_entry_count(sig, sig_entry_count(sig), it->second.m_count);
        sig_write_entry_bytes(sig, sig_entry_count(sig), it->second.m_bytes);
        sig_write_entry_avg(sig, sig_entry_count(sig), (int)it->second.m_avg);
        sig_bump_count(sig);

        if (sig_entry_count(sig) == 100)
        {
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);

            guard.Set(GlobalData::s_stream_pool->Acquire("MessageCounter.cpp", 0x7b));
            (*guard)->operator<<(0x26c);
            (*guard)->operator<<(-1);
            sig = reinterpret_cast<char*>(
                guard->GetInBuffer<SIG_INSERT_RESPONSE_TIME>());
            sig_init_head(sig, nowSec);
        }
    }

    if (sig_entry_count(sig) == 0)
    {
        GlobalData::s_stream_pool->Free(guard);
    }
    else
    {
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::LOGDB_Q, guard);
    }

    m_map.clear();
