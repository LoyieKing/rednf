int Inter_DeathTowerLoadTopRanker::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_DeathTowerLoadTopRankerPktView* pPkt = reinterpret_cast<Inter_DeathTowerLoadTopRankerPktView*>(data);
    if (pPkt->m_4 == '\0')
    {
        for (unsigned int t = 1; t <= 4; ++t)
        {
            WongWork::CDeathTowerRanking* table =
                (WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(t);
            table->resetRanking();
        }
    }

// §P183/P331：ORIG 每次字段访问都从 data 重物化 `imul $0x6c,%eax` + `add $0xfa0`
// （imul$0x6c 5 处 / add$0xfa0 3 处，我方原 1/0 —— 被 rec + pWin_rec 两个具名指针
// 吸收掉 5×(3~6) 条地址计算，our_insns 80 vs ORIG 88）。改宏每次直取后 our=89；
// i 为循环变量、循环体不改 i/data ⇒ 寻址结果逐次相同，无可观察行为变化。
