// ============================================================================
// getItemSellPrice @ 0x0898c86e（自由函数；nm 实查 size 0x8e）
//   【切片转写】game_func_report 对本符号的 ORIG 段取错（撞名，§6.3 假阳性清单
//   已报 Main），故按 objdump 0x0898c86e..0x0898c8fb 人工转写：
//     0x0898c874 `mov 0x14(%ebp),%eax; mov %al,-0x14(%ebp)` bool 实参落本地槽；
//     0x0898c87a `movb $0,-0x1` + `cmpb $0,-0x14; je` + `movb $2,-0x1` ⇒ sel 形；
//     0x0898c888 `cmp 0x10,%eax; jle; mov 0x10→0xc` ⇒ a=min(a,b)（写回形参槽）；
//     0x0898c896 `cmpl $0,0x10; jne` 后**两分支各自完整重发**
//     `movzbl sel; mov rates(,%eax,4); imul price; 0x10624dd3 magic /1000`
//     （0x0898c89c 与 0x0898c8c6 各 13 条）⇒ 提公共 base 局部会丢整段 13 insn
//     （P48/P183：-O0 无 CSE）。
// ============================================================================
int getItemSellPrice(int price, int a, int b, bool flag)
{
    unsigned char sel = 0;
    if (flag)
    {
        sel = 2;
    }

    if (a > b)
    {
        a = b;
    }

    if (b == 0)
    {
        return itemPriceRates_[sel] * price / 1000;
    }
    return itemPriceRates_[sel] * price / 1000 * a / b;
}