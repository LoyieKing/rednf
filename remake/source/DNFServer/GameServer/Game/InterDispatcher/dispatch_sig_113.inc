// [b28d-lane2] verdict=DIFF ndiff=86（ORIG 0x084d3364）：槽序/帧差(N82/P212/P221)+寄存器中转(N18a/N7)+混合残差；gap+8。
// [b28d-solo-InterDispatcher] verdict=DIFF ndiff=110（ORIG 0x84d3364）：N101 imul/lea 折叠+N18a 单段 regmove+N82/P112 帧槽平移。
int Inter_DeathTowerLoadTopRanker::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    SIG_DEATH_TOWER_LOAD_TOP_RANKER* load = reinterpret_cast<SIG_DEATH_TOWER_LOAD_TOP_RANKER*>(data);
    if (load->m_flag == '\0')
    {
        for (unsigned int t = 1; t <= 4; ++t)
        {
            // ORIG `push %edi/%esi/%ebx`（帧 0x4c+0xc）与我方 `sub $0x58`（无 push）
            //   总量相同 ⇒ 我方旧具名 `table` 等槽顶替了 ORIG 的 callee-saved 寄存器；
            //   原码是链式单表达式（P0/P183）。
            static_cast<WongWork::CDeathTowerRanking*>(
                WongWork::CDeathTowerRanking::getRankTable(t))->resetRanking();
        }
    }

// §P183/P331：ORIG 每次字段访问都从 data 重物化 `imul $0x6c,%eax` + `add $0xfa0`
// （imul$0x6c 5 处 / add$0xfa0 3 处，我方原 1/0 —— 被 rec + pWin_rec 两个具名指针
// 吸收掉 5×(3~6) 条地址计算，our_insns 80 vs ORIG 88）。改宏每次直取后 our=89；
// i 为循环变量、循环体不改 i/data ⇒ 寻址结果逐次相同，无可观察行为变化。
//
// [G2B-Monitor] 本 .inc 到此止，其余（宏 + 主环 + 尾部）由聚合器 InterDispatcher.cpp
//   在 `#include` 之后续写 ⇒ g2_norm 中 `imul $0x6c`/`add $0x1010` vs `mov 0x1010(%eax)`
//   等段不在本任务写域；本件已做：resetRanking 循环去 `table` 具名改链式单表达式
//   （ORIG `push %edi/%esi/%ebx` 与我方旧 `sub $0x58` 帧总量相同 ⇒ 旧槽顶替了寄存器）。
//   残余留 N：续写段寄存器/寻址折叠（N18a/N4 族）。
#define pWin_rec \
    (reinterpret_cast<Inter_DeathTowerLoadTopRankerRecWinView*>(data + i * 0x6c))
    for (unsigned int i = 0; i < load->m_count; ++i)
    {
        const unsigned int stage = pWin_rec->m_1010;
        const unsigned int playTime = pWin_rec->m_100c;
        const unsigned int key =
            *reinterpret_cast<unsigned short*>(data + 6 + i * 2);
        const unsigned int towerIdx = pWin_rec->m_fac;
        const unsigned int tableIdx = pWin_rec->m_fae;
        WongWork::CDeathTowerRanking* table =
            (WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(tableIdx);
        table->registRanker(
            towerIdx, key,
            reinterpret_cast<const stDeathTowerRecordMemberInfo_t*>(&pWin_rec->m_padfb0),   // §4.3 具名槽替代裸偏移 rec+0xfb0
            playTime, stage);
    }
#undef pWin_rec
    return 0;
}