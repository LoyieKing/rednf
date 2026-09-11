// ---- RegenVer_get_option_quantity —— T 0x085f8cb4（同名报告全文转写）------
// [Lane13] 返回类型 int→unsigned int（CDataManager.h 同步改）：ORIG 调用点
// 0x085f8cbd 段为 `call; mov %eax,%edx; mov 0x1c(%ebp),%eax; mov %edx,(%eax)`
// = RHS-first + eax→edx 中转（P211 符号性异型），callee 自身尾部
// `call vector::ix; mov (%eax),%eax; leave; ret` 无扩展 ⇒ 原码返回 unsigned。
unsigned int RegenerationROI::RegenVer_get_option_quantity(int lo, int hi)
{
    if (lo < 0 || 3 < lo)
    {
        return 0;
    }
    if (hi < 0 || 3 < hi)
    {
        return 0;
    }

    int total = 0;
    for (int i = lo - 1; i < hi; ++i)
    {
        total += m_regenerator.m_vec30[i];
    }

    /* ORIG 槽复用（/tmp/p26_reg.cpp probeV2 实证逐槽一致）：4.1 把 randInt
     * 返回值 r 写回 total 槽（lifetime 不重叠）；独立具名 r 会让 4.4 多占
     * 一槽、全帧槽号错位。 */
    unsigned long bound = (unsigned long)total;
    total = m_regeneratorPtr->randInt(bound);

    int acc = 0;
    for (int i = lo - 1; i < hi; ++i)
    {
        acc += m_regenerator.m_vec30[i];
        if (total <= acc)
        {
            return i + 1;
        }
    }
    return m_regenerator.m_vec30[lo];
}