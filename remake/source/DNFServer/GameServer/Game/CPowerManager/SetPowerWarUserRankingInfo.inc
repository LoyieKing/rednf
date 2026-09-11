// [b28d-solo-CPowerManager] IDENTICAL(104=104, ndiff 0) | ORIG 0x0847ebec | 本轮修复：
// P310 同法（int i + `(unsigned int)i < 3`）复现 ORIG 0x0847ec0e 尾测两段物化
// `mov -0xc(%ebp),%eax; cmp $0x2,%eax; setbe; test; jne`；P167 CLEAR 12/12。
void CPowerManager::SetPowerWarUserRankingInfo(unsigned int* charNos)
{
    // ORIG 0x0847ebec：characNo==0 -> 置 flag，否则写 +0x2c 并发 DB 消息 0x166。
    // [b28d-solo] P310：尾测须两段寄存器形 ⇒ 局部 int + 显式 unsigned 比较。
    for (int i = 0; (unsigned int)i < 3; ++i)
    {
        if (charNos[i] != 0)
        {
            m_rankings[i].m_characNo = charNos[i];
            CStreamGuard guard(
                GlobalData::s_stream_pool->Acquire("DNF_PowerManager.cpp", 0x7d), true);
            **guard << 0x166;
            **guard << -1;
            **guard << m_rankings[i].m_characNo;   // unsigned int → operator<<(uint)（ORIG P167）
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
        }
        else
        {
            m_rankings[i].m_flag = 1;
        }
    }
}