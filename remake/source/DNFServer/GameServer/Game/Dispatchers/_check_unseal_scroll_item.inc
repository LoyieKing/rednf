// ORIG T 0x08229644：解封卷轴资格链（0xffff=金币通道直接通过）。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x08229644 orig/ours=81/89 ndiff-blocks=14
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: movzwl -0x6c(%ebp),%ebx | lea    -0x5d(%ebp),%edx | mov    %ebx,0xc(%esp)
int Dispatcher_UnsealRandomOption::_check_unseal_scroll_item(
    CUser* user, const Inven_Item* item, unsigned short slot)
{
    if (slot != 0xffff)
    {
        // ORIG 0x0822969a `movzwl -0x6c(%ebp),%ebx` + 直 call getCurCharacInvenR：
        // 无 inven 具名缓存槽（P183/P37 去缓存）。
        Inven_Item scroll =
            ((CInventory*)(const CInventory*)user->getCurCharacInvenR())
                ->GetInvenSlot(1, (int)slot);
        // [g6 2026-09-11] ORIG 0x0822969a `mov -0x5b(%ebp),%eax; mov %eax,%ebx`
        //   = 成员直读单条 disp（-0x5d 槽基 + 0x2）；旧码 `*(const int*)&m_addInfo`
        //   取址+cast 产 `lea -0x59; add $0x2; mov (%eax)` 三条（N102 族）。
        CItem* proto =
            G_CDataManager()->find_item((int)scroll.m_addInfo);
        if (proto == 0)
        {
            return 0x15;
        }
        if (!proto->is_stackable())
        {
            return 0x11;
        }
        // [g6 2026-09-11] 语义修复（ORIG 直证 0x08229703）：`mov -0x1c(%ebp),%eax;
        //   mov %eax,0x4(%esp)` —— GetUnsealScrollPossibleLevel 的实参是
        //   proto 的 +0x328 值（同一具名 limit，先判 ==-1 再传），旧译传常量 1
        //   属形参错（真语义 bug）。同时恢复 ORIG 的具名槽（`cmpl $0xffffffff,
        //   -0x1c(%ebp)` 内存直比 / `cmpl $0xffffffff,-0x14(%ebp)`）。
        int limit = reinterpret_cast<const CStackableItem*>(proto)->m_int328;  // +0x328
        if (limit == -1)
        {
            return 1;
        }
        int script =
            (int)(size_t)G_CDataManager()->GetExpertJobScript(2);   // ORIG 参数常量 2
        int possibleLevel =
            ((STExpertJobScript*)script)->GetUnsealScrollPossibleLevel(limit);
        if (possibleLevel == -1)
        {
            return 1;
        }
        // [R10a06 第十轮 ④#3 · C 轴「多余判空」] ORIG 0x0822973e `mov %eax,-0x10(%ebp)`
        //   （find_item 返回值落具名槽）→ 0x08229741 `mov -0x10(%ebp),%eax; mov %eax,(%esp)`
        //   → 0x08229747 `call getUsableLevel`（**无 cmpl/test/je**）→
        //   0x08229752 `cmp -0x14(%ebp),%eax` → 0x08229755 `jle`。
        //   我方原写 `base != 0 && …`（0x081e3af5 落槽 → cmpl $0x0 → je 0x081e3b15）
        //   多出判空且路径分叉（ORIG 恒直接解引用 base 调用 getUsableLevel）。
        //   保留 ORIG 的具名槽语义（P0：ORIG 有 -0x10(%ebp) 槽 ⇒ 保留 base 变量）。
        CItem* base = G_CDataManager()->find_item(item->m_addInfo);
        if (limit < base->getUsableLevel())
        {
            return 0x11;
        }
    }
    return 0;
}