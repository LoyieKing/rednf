int CCompound_ExtreamDun_Item::_exception_Rare(                     // ORIG T 0x832cffe
    const CEquipItem** refs, char select, ENUM_OUTPUTITEMS_RARITY outRarity,
    unsigned int baseLevel) const
{
    // [b28d-solo-CItemList] AE（106/106，ndiff 0）| 本轮 3 处真语义修正：
    //   ① 第三实参传**循环变量 c**（ORIG `mov -0xc(%ebp),%ebx; mov %ebx,0xc(%esp)`），
    //   非 outRarity 原值；② c>1 分支第二实参 = baseLevel + g*5u（ORIG 0x832d0bc
    //   段 `shl $2; add; add baseLevel`），原写 baseLevel+g 使上界错位；
    //   ③ pair 顺序 (+1,+5)（ORIG ctor 0x4/0x8(%esp) = -0x28/-0x2c），原 (+5,+1)
    //   使 range.first/second 颠倒。另：`c <= 0` 复现 `cmpl $0; jg <else>` 布局、
    //   分支内各自 found 检查复现 ORIG 逐分支 `cmpl/je/mov/jmp`。
    // [b28d-20] 真语义修正：ORIG 0x832d01e `mov 0x14(%ebp),%eax; mov %eax,-0xc(%ebp)`
    //   —— 内层 c 初值取自第 3 参 outRarity（0x14），非 baseLevel（0x18）；
    //   baseLevel 仅参与 pair 计算（0x832d03d/0x832d050 `add 0x18(%ebp),%eax`）。
    //   原实现以 baseLevel 为初值使 _FindItemAll_BindSphere 重复调用 baseLevel+1 次
    //   （该 callee 带随机语义），行为与 ORIG 不符。
    int found = 0;
    for (unsigned int g = 0; g <= 1u; ++g)
    {
        for (int c = (int)outRarity; c >= 0; --c)
        {
            // [b28d-solo-CItemList] 真语义修正 ×2（ORIG 0x832cffe 汇编为准）：
            //   ① 第三实参传**循环变量 c**（ORIG `mov -0xc(%ebp),%ebx;
            //      mov %ebx,0xc(%esp)`），非 outRarity 原值；
            //   ② c>1 分支第二实参 = baseLevel + g*5u（ORIG 0x832d0bc 段
            //      `shl $2; add %edx,%eax; add 0x18(%ebp),%eax` = 5g+baseLevel），
            //      原写 baseLevel+g 使上界错位。
            //   条件用 `c <= 0` 复现 ORIG `cmpl $0x0,-0xc; jg <else>` 布局
            //   （`c == 0` 生 jne）。
            if (c <= 0)
            {
                // [b28d-solo] P167/P192：ORIG 0x832cffe 用 pair<unsigned,unsigned>
                //   直构（MISSING `pair<unsigned,unsigned>::pair<unsigned&&,
                //   unsigned&&>`），make_pair 落 EXTRA。
                //   真语义修正③：ORIG ctor 0x4/0x8(%esp) = -0x28/-0x2c ⇒
                //   pair = (+1, +5)（原写 (+5, +1) 使 range.first/second 颠倒）；
                //   4.1 右→左求值 ⇒ 先算 second(+5) 存 -0x2c、后算 first(+1) 存 -0x28。
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)c,
                    std::pair<unsigned int, unsigned int>(
                        baseLevel + g * 5u + 1u, baseLevel + g * 5u + 5u));
                if (found != 0)
                {
                    return found;
                }
            }
            else if (c > 1)
            {
                found = _FindItemAll_BindSphere(refs, select,
                    (ENUM_OUTPUTITEMS_RARITY)c,
                    std::pair<unsigned int, unsigned int>(baseLevel + g,
                                                          baseLevel + g * 5u));
                if (found != 0)
                {
                    return found;
                }
            }
        }
    }
    return found;
}