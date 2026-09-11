// ---------------------------------------------------------------------------
// CItemUpgrade::_getAdditionalDisjointItem(bool 版) —— ORIG T 0x08546a96。
// [b28d-solo-WongWorkMiscB_A] ORIG 0x08546a96 逐点对应：
//   ①at() 结果为**值拷贝**局部 `STResultItemCnt elem`（trivial 拷贝展开 4+2 两段
//     `mov (%eax),%edx; mov %edx,-0x2e; movzwl 0x4(%eax),%eax; mov %ax,-0x2a`），
//     非引用（引用多 1 指针槽 + 每次回读，P0）；
//   ②slotIdx/count 类型由 make_pair mangled `IRmRjE` 定：`unsigned long`/`unsigned int`，
//     count 预清 0 + 分支赋值（P198①）；
//   ③PRETTY 改 `__PRETTY_FUNCTION__`（ORIG `movl $sym,0x4(%esp)` 单条，P139）。
void CItemUpgrade::_getAdditionalDisjointItem(
    unsigned int up, const CItem* itemScript,
    IntPairVector& items,          // [P3 2026-09-11 P139] ORIG prett 实串用 typedef
                                   // 名（`... const CItem*, IntPairVector&, ...`，
                                   // 0x82 字节）⇒ 拼写改 typedef（mangled/ABI 不变，
                                   // 仅 __PRETTY_FUNCTION__ 串由 0xbc→0x82 与 ORIG 同）。
    const upgrade_table_t& table, bool flag)
{
    (void)itemScript;
    try
    {
        STResultItemCnt elem = table.m_destruction.m_vec.at(up);   // at 越界抛异常 → catch 吞掉并 trace
        unsigned long slotIdx;
        unsigned int count;
        if ((int)(short)elem.m_a != up)
            goto L_island1;
        slotIdx = table.m_destruction.m_field58;
        count = 0;
        if (flag)
            count = (unsigned int)(short)elem.m_c;
        else
            count = (unsigned int)(short)elem.m_b;
        if (count == 0)
            goto L_island2;
        items.push_back(std::make_pair(slotIdx, count));
    L_island1:;
    L_island2:;
    }
    catch (...)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x121, 5)("3RD_CHRONICLE : OUT OF RANGE!!");
    }
}