// ---- CheckInventoryEmptyCount —— T 0x0827108c（同名报告全文转写）-----------
// [b28d-solo-GlobalData] 改善 103→96（p167 16/16 CLEAR）：P63 共享出口改写为
// 嵌套正向 if 包裹（消 4.4 每处独立 `mov $1;jmp` 中继）。残差 N：ORIG 7 处
// `xor $1;test;jne +0x15d` 全部直达同一 `mov $1` 尾块，4.4 恒产 `je NEXT`
// 极性（P222 正向包裹/短路链/goto 三形均实测不翻）⇒ 极性恒折封顶。
// `je next; mov $1; jmp`（+14 条）；goto 尾标签形（探针 /tmp/b28d_probe_pol.cpp t2）
// 得 jne 但每源多 1 个 nop 岛（+7），嵌套正向 if/`||` 链（t3/t6）丢 xor 或末项
// 物化 ⇒ 无源码解。**真语义 bug 修正**：ORIG jne 落点 +0x15d 即 return 1 ⇒
// 任一 check 为假返回 1（旧重建 `return false` 与 ORIG 相反，Ghidra C 同证）。
bool CheckInventoryEmptyCount(CUser& user, int need)
{
    // ORIG（0x0827108c，report 汇编+反编译 C 全读）：每次 check 前重调
    // getCurCharacInvenR()。**真语义 bug 修正（b28d-solo-GlobalData）**：ORIG
    // 0x082710b4 族为 `xor $1,%eax; test %al,%al; jne +0x15d`，而 +0x15d 即
    // `mov $0x1,%eax; jmp leave` ⇒ **任一 check 为假即 return 1**（Ghidra C 同形：
    // `if (cVar1 == 1) { … } return 1;`）；旧重建写 `return false` 与 ORIG 相反。
    // 末检查 0x082711e7 `jl +0x164(=mov $0,%eax)` ⇒ GetRemainCapacity < need 才
    // return 0，否则 return 1。照抄 ORIG 可观察行为（P171）。
    // [GlobalData-判据①] 0x082710b9 族 7 处 `xor $1;test %al,%al;jne 0x082711e9`
    // 全部直达**同一** `mov $1` 尾块（0x082711e9，同时也是末比较 `jl` 的直落臂）
    // ⇒ 原码是**单个三元表达式全表达式链**（4.1 短路 `||` 逐项 `jne L_true`，真臂
    // 与末项真臂共尾）：
    //   return (!c1 || … || !c7) ? true : (GetRemainCapacity(2,8) >= need);
    // 探针 /tmp/l7/probe1.cpp D 形逐条同形（A 嵌套 `if(!c)` 形反产 `je RET1`、
    // B/C/E 形各多一级 bool 物化或 setcc）。旧嵌套形另含**真语义 bug**：其运行语义
    // 为「任一 check 为真即 return true」，与 ORIG「任一 check 为假即 return 1」
    // 相反（旧注释自述即 ORIG 语义，代码却反写）。
    return (!user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)1, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)2, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)3, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)10, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)9, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)5, need)
            || !user.getCurCharacInvenR()->check_empty_count((Inven_Item::ITEM_TYPE)6, need))
               ? true
               : (user.getCurCharacInvenR()->GetRemainCapacity(
                      (INVEN_TYPE)2, (Inven_Item::ITEM_TYPE)8) >= need);
}