// ORIG ResultToStr 0x084b165c：逐条拷贝 stMonsterIndependentDrop_t，
// 按 copy+0x2c（m_rate[10] 主掉率）非 0 决定 PrintItemDropList 详细模式。
// 【b28d-solo-GmCmds_D 判词 AE】p167 CLEAR（11=11）：循环前缓存 begin/end（P13）、
// `else if (copy.m_mainRate != 0)` 二次 test/je（P184 死块照抄）均按 ORIG 复现。
void CIndependentDrop::ResultToStr(
    const std::vector<stMonsterIndependentDrop_t>& drops, char* out)
{
    // b28d-solo-GmCmds_D：ORIG 在循环前一次性求值 begin(-0xc)/end(-0x10)（P13
    // end 缓存），4.4 对 `it != drops.end()` 恒每次循环重调 end() ⇒ 手动缓存 end。
    std::vector<stMonsterIndependentDrop_t>::const_iterator it = drops.begin();
    std::vector<stMonsterIndependentDrop_t>::const_iterator last = drops.end();
    for (; it != last; ++it)
    {
        stMonsterIndependentDrop_t copy(*it);                // ORIG copy ctor 0x8376bb2
        if (copy.m_mainRate == 0)
        {
            PrintItemDropList(copy, out, false);
        }
        else if (copy.m_mainRate != 0)                      // ORIG 二次 test/je（P184）
        {
            PrintItemDropList(copy, out, true);
        }
    }
}