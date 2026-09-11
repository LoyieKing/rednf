// ---- CheckTotalItemPrice（ORIG T 0x85c76f0，报告 CheckTotalItemPrice.md 汇编 L15-103/反编译C L105-160 全文）----
// 仅统计 state==2 在售条目；可堆叠经 COperator<int>::Product（ORIG 覆写 total 非累加），
// 不可堆叠累加且非正即失败。返回 total>0。
// [b28d-24] AE（P167 归零，clean 后 0 残差）。ORIG 0x085c76f0
// [b28d-solo-CPrivateStoreMgr] **AE（clean 后 0 残差，83=83）**：P0 清绑（删
// `register int addInfo asm("ebx")` / `register bool nullp __asm__("al")`，N96）；
// 槽比零物化改 **P225 while(pItem==0){return 0;}** 复现 ORIG `cmpl $0,-0xc;
// sete %al; test %al,%al; je`（新范式：4.4 对 if 槽比恒直跳，while 形才出四段）。
// [L14] 本轮复核：83/84，唯一差异 = ORIG(0x085c76f0) `jg +0xe3` 目标前我方多
// 1 个 4.4 循环回边对齐 nop（其余 jcc 目标下标逐位对齐、槽号/立即数全同）⇒ N112 封顶。
bool CPrivateStore::CheckTotalItemPrice()
{
    int total = 0;
    for (int i = 0; i <= 0xd; ++i)
    {
        PrivateStoreItem& entry = m_items[i];
        if (entry.m_flag3d != 2)
            continue;
        if (entry.m_field40 == 0)
            return 0;
        CItem* pItem = 0;                          // ORIG +0x55 movl $0,-0xc
        // P0 清绑（原 register asm("ebx") 已删）：ORIG +0x5c..+0x62
        // `mov 0x2(%eax),%eax; mov %eax,%ebx` 是表达式右值经寄存器直传，
        // 原码无具名中转 ⇒ 实参内联。
        pItem = G_CDataManager()->find_item((int)entry.m_item.m_addInfo);
        // ORIG +0x78 `cmpl $0x0,-0xc(%ebp); sete %al; test %al,%al; je`：4.1 槽比零
        // 物化；4.4 对 if 槽比恒 cmp/jne 直跳（P197④）⇒ 试 P225 while 形复现四段。
        while (pItem == 0)
        {
            return 0;
        }
        if (pItem->is_stackable())
        {
            if (!COperator<int>::Product(entry.m_field40,
                                         (int)entry.m_item.m_addInfo2, total))
                return 0;
        }
        else
        {
            total += entry.m_field40;
            if (total <= 0)
                return 0;
        }
    }
    // ORIG +0xf6 岛序：jg 跳过 mov$0 岛、mov$1 位于尾部 ⇒ `<= 0 ? 0 : 1` 写法。
    // 探针 x13 f2：`if (x <= 0) return 0; return 1;` 复现 test/jg/mov0/jmp/mov1 岛序。
    if (total <= 0)
        return 0;
    return 1;
}