STQuickPartyPoolMap_Data::STQuickPartyPoolMap_Data()
{
    // [b28d-L1] 修法：ORIG 0x0826b8b7..0x0826b8e7 为**展开的 4 次**
    // `mov 0x8(%ebp),%eax; [add $K,%eax;] mov %eax,(%esp); call list::clear`
    // （无循环变量/无回边）；原 for 形使 4.4 发 `movl $0,-0x1c` + setle/jne
    // 回边并把帧撑到 0x3c（ORIG 0x2c）、EH 槽 -0x2c（ORIG -0x1c）。展开后逐条对齐。
    m_lists[0].clear();
    m_lists[1].clear();
    m_lists[2].clear();
    m_lists[3].clear();
