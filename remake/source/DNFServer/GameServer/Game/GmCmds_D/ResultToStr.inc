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
        if (copy.m_rate[10] == 0)
        {
            PrintItemDropList(copy, out, false);
        }
        else if (copy.m_rate[10] != 0)                      // ORIG 二次 test/je（P184）
        {
            PrintItemDropList(copy, out, true);
        }
    }
