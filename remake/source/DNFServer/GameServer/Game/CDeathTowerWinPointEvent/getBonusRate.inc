// ORIG C1 0x081170e8：基类构造 + vptr + m_ratio(+0xc) 置 0；
// D1 0x0811710e 用户空析构（编译器生成 vptr+基类析构），D0 编译器自动。
// ORIG W 0x08469dcc _ZN24CDeathTowerWinPointEvent12getBonusRateEv：
// IsEventing(0) 虚调判活（vptr+0x3c）；活 → (float)m_ratio/100.0f。
// fildll 零扩展 + flds 100.0f + fdivrp（探针 f14 同形）→ 返回 float。
// b28c-L20: N7 — 残差仅虚调 IsEventing 的间接 call 目标寄存器互换（ORIG 目标 %edx /
// this %eax，我们目标 %eax / this %edx）；flds 100.0f 常量地址按 AE 归一已不计差。
// 4.4 间接 call 恒 %eax（probe4b 七形）⇒ N7 封顶，ndiff=8 仅寄存器互换。
// 4.4.6 后端实测（/tmp/tu_CDeathTowerWinPointEvent_446.o）：本件 ndiff=0 ⇒ 路由切 4.4.6 后可 AE。
float CDeathTowerWinPointEvent::getBonusRate()
{
    if (IsEventing((CUser*)0))
        return (float)m_ratio / 100.0f;
    return 0.0f;
}