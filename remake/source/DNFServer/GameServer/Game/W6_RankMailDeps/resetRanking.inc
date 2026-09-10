void WongWork::CDeathTowerRanking::resetRanking()
{
    // asm: 外层 cmp $0x4/setle ⇒ i<=4（有符号，P2）；内层 cmp $0xc7/setbe ⇒ s<=199。
    // [b28d-solo-W6_RankMailDeps] P209 附翻正：内层尾测 ORIG 为两段寄存器形
    //   `mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe; test; jne`；4.4 对 unsigned
    //   局部与常量比较恒折 `cmpl $0xc7,-0xc(%ebp)` 单段（旧判词「无源码解」）。
    //   局部改 int + 显式 (unsigned int) 比较即复现（探针
    //   /tmp/b28d_w6_probe5.cpp f2 实测逐条同形，语义仍为 0..199）。
    for (int i = 0; i < 5; ++i)
    {
        _getRankingTable(i)->clear();
        for (int s = 0; (unsigned int)s < 200; ++s)
            m_rankMap[i][s].clear();
    }
