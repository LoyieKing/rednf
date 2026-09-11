// ORIG 0x85e372c：段位区间判定；两次独立 get_pvp_grade（P48 无 CSE），
// && 短路产生 jg/jge 直跳失败块 + 布尔返回归一。
bool CMission_rank::isAcceptCondition(const CUser& user) const
{
    // [b28d-70] 判词：AE 30/31（m_rankRange.first/.second 同址）。
    // [b28c-35] ORIG 0x085e372c：第一比较先装载 m_rankRange.first 到 %ebx（跨 call
    // 保活）再 call get_pvp_grade ⇒ 左操作数是成员（交换操作数序，P23/P66）；第二比较
    // 仍左 call 右成员。原 `grade >= m_rankRange.first` 形 4.4 恒先 call 后读成员
    // （22 vs 30 insn）。
    // 尾段 ORIG 为两层物化（&& 结果 al → test %al,%al; je; mov $1/$0）⇒ P56
    // `if(cond)return true; return false;` 形保留第二层。
    if (m_rankRange.first <= user.get_pvp_grade() &&
        user.get_pvp_grade() < m_rankRange.second)
        return true;
    return false;
}