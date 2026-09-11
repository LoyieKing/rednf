// [b28d-62] 修：P220 极性翻转（`it == end` 直落未命中分支）+ m_bLogDB 正极性。
//   ORIG 0x085ecc5e
// [P8S1 2026-09-11 本波复核] 维持封顶：N82/N95 槽相位(ORIG cur@-0x14 vs 我方 -0x20)；P139/P339/P341/disp 折叠/掩码 cast
//   诸新钥匙逐条核过，无翻案。证据见 res_1.tsv 同件 note。
void CQueryCounter::IncreaseQueryCount(const std::string& query)
{
    if (query.find("log_query_ref", 0) != std::string::npos)
    {
        return;
    }
    if (query.find("from db_connect", 0) != std::string::npos)
    {
        return;
    }
    std::map<std::string, std::map<int, QUERY_INFO>::iterator>::iterator it =
        m_queryIDMap.find(query);
    // [b28d-62] P220 极性翻转：ORIG 0x085eccfc 调 operator==（`it == end` 为真直落
    //   未命中分支），我方 `!=` 生相反块序；m_bLogDB 正极性（`test %al; je else`，
    //   ORIG 0x085ecd0f 实证）⇒ 未命中分支内先 stream 后 SaveQuery。
    if (it == m_queryIDMap.end())
    {
        if (m_bLogDB)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire("QueryCounter.cpp", 0x1d2), true);
            **guard << 0xbe;
            **guard << -1;
            **guard << (int)query.size();
            guard->put_binary((void*)query.c_str(), (int)query.size());
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            int qid = 0;
            if (SaveQuery(query.c_str(), qid))
            {
                RegisterQuery(query.c_str(), qid, 0);
            }
        }
    }
    else
    {
        // [2026-09-11] P0：ORIG 0x85eccxx `mov %eax,-0x14(%ebp);
        // mov -0x14(%ebp),%edx; mov %edx,0x34(%eax)`（值先落具名槽再写成员）
        // ⇒ 原码有具名 iterator 局部；4.4 直传 `mov %eax,0x34(%edx)` 少 2 条。
        std::map<int, QUERY_INFO>::iterator cur = it->second;
        m_current = cur;
        // [G5 2026-09-10 P167] ORIG 0x085ecc5e 调用差集：我方多 1×
        // `_ZNKSt17_Rb_tree_iteratorISt4pairIKiN13CQueryCounter10QUERY_INFOEEEptEv`。
        // 实测 `(*m_current).second` 形反劣（换出 +3 deEv 且 ORIG 需 ptEv×2）⇒ 维持
        // `->` 形；试后置自增（P38 反向：`= x+1` 单语句 SAVE_EXPR 合并成单次 ptEv，
        // 拆/`++` 形可出第二处访问）。
        // [2026-09-11] 后半段 ORIG 用**局部** -0x14 做 operator-> this
        // （`lea -0x14(%ebp),%eax; mov %eax,(%esp); call ptEv`），非
        // `mov 0x8(%ebp),%eax; add $0x34,%eax`（成员取址 2 条）⇒ 后续访问走 cur。
        if (cur->second.count == 0)
        {
            m_totalQueryCount = m_totalQueryCount + 1;
        }
        cur->second.count++;
    }
}