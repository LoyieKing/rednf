bool CItemLimitEdition::isSellComplete() const
{
    // [b28d-51] 判词 N（ORIG 0x0822cac8，N7/N22 寄存器角色族）：P210 帧继承已翻正
    // （sub $0x18 保守帧）。残余：ORIG `call; mov %eax,%edx; mov 0x8(ebp),%eax;
    // mov 0x1c(eax),%eax; cmp %eax,%edx`（call 值先中转 %edx、字段双基址载入 %eax）
    // vs 4.4 `mov this,%edx; mov 0x1c(edx),%edx; cmp %edx,%eax`（免中转、字段单载入
    // %edx）= N18a+N22。探针 8 形（>=/<=/!/<、显式 int cast、具名 int 局部、
    // `> x-1`、`x-1 <`；/tmp/b28d_51_probe_sell.cpp）全部不可复现，语义等价。
    // ORIG 0x0822cac8：若 totalSellNum == -1 返回 false；否则 curSellNum >= totalSellNum。
    if (m_totalSellNum == -1)
        return false;
    // [b28d-51 + 本轮复验] 操作数交换形（m_totalSellNum <= getCurSellNum()）实测 4.4 转
    //   `push %ebx; sub $0x14; mov 0x1c(%eax),%ebx; call; cmp %eax,%ebx; setle`（跨 call 保活
    //   字段 + 帧破）⇒ 更劣，已回退。ORIG @0x0822cae0 `mov %eax,%edx`（call 结果中转）为
    //   4.1 寄存器分配固有形（N18a/N22 族），8+2 形不可复现，语义等价。
    return getCurSellNum() >= m_totalSellNum;   // ORIG call getCurSellNum
