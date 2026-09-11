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
    // [L6S5 P248 三档复核·封顶] 按 P321/T5，残差仅「call 值中转 + 字段双基址装载」的
    //   寄存器角色差（19 vs 18 条，gap=1）⇒ 类型轴**不**适用（禁为装载序改类型）：
    //   · 档① callee 返回类型：getCurSellNum() 声明 int 与 ORIG 尾 `mov 0x48(%eax),%eax`
    //     （getCurSellNum 体）一致；改 unsigned 会把 setge 翻成 setae（语义变）✗；
    //   · 档③ 字段类型：m_totalSellNum 为 int（同 ORIG `mov 0x1c(%eax),%eax`）✓；
    //   · 返回类型 bool ✓（两端尾均为 `setge %al; leave; ret` 无 movzbl 归一化，P9/P292）。
    //   ⇒ 若必须再试，唯一候选是 **.h 侧** 把 getCurSellNum() 返回类型异型化（enum），
    //     需 Main 裁决（本泳道不改 .h）；当前判 N18a+N22 封顶。
    return getCurSellNum() >= m_totalSellNum;   // ORIG call getCurSellNum
}