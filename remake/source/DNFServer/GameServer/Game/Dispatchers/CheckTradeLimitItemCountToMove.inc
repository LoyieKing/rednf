// ORIG T 0x081202a9：交易限次物品移动资格。
// b28d-solo-Dispatchers verdict=DIFF(routed) [N-P112] ORIG 0x081202a9 orig/ours=50/52 ndiff-blocks=6
//   残余为跨版本寄存器分配/栈槽序差（P75/P112 结构性），语义已对齐
//   首差: mov    %eax,-0x10(%ebp) | cmpl   $0x0,-0x10(%ebp) | je     08120321 <+0x78>
void CheckTradeLimitItemCountToMove(const Inven_Item& item, bool& ok)
{
    // [laneE1 2026-09-11] 语义修复（ORIG 直证，两处极性反）：
    //   ① 0x081202e8 `xor $1,%eax; test %al,%al; jne` ⇒ 原码是
    //      `if (!proto->is_stackable())`（旧译写成 `if (proto->is_stackable())`）；
    //   ② 0x08120312 `test %al,%al; sete %al; test %al,%al; je` ⇒ 原码是
    //      `GetTradeLimitCount() == 0` 时置 ok=false（旧译写成 `!= 0`）。
    //   结构：ORIG = 三守卫各带 nop 中继岛（0x08120321/0x08120324/0x08120327）+
    //   第四守卫直落共享尾 0x08120328；P223/BM8 复现形 = 独立 goto + 尾标签串
    //   （标签序=守卫序，含初始化声明包入内层复合块以合法跨越）。
    //   旧译的四条 `return ok;` 使每出口多一条 `movzbl (%eax),%eax`；ORIG 全函数
    //   不读 ok（P159：Ghidra 亦为 void）。
    //   [L7 2026-09-11 · Main 批准] 返回类型已按 P159 落 void（Inven_Item.h:352 +
    //   .inc 首行同刻改；ORIG 0x081202a9 尾 `add $0x24,%esp; pop %ebx; pop %ebp; ret`
    //   无 eax 设置），故删尾部 `return ok;`（其两项读回即我方多出的 2 条）。
    //   第二指针槽 -0xc：ORIG 0x081202ed/0x081202f0 `mov -0x10(%ebp),%eax;
    //   mov %eax,-0xc(%ebp)` ⇒ 原码有第二个具名指针（-0x10=proto、-0xc=该副本）。
    ok = true;
    CItem* proto = G_CDataManager()->find_item(item.m_addInfo);
    if (proto == 0)
        goto L_early;
    if (!proto->is_stackable())
        goto L_mid;
    {
        CItem* pItem = proto;
        if (pItem->GetAttachType() != 7)
            goto L_late;
        if (item.GetTradeLimitCount() == 0)
            ok = false;
    }
L_early:;
L_mid:;
L_late:;
}