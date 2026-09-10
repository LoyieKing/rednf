void CBreakAwayPreventSystem::giveBreakAwayDungeonClearReward(CUser* user)
{
    // b28d-D66 终态 50=50 AE（ndiff 37→0，仅重定位显示差）：四处修正
    // ①P0/P198①：删 order/accCera 具名 int 局部 —— ORIG `call getBreakAwayRewardOrder;
    //   cmp $0x1,%eax; sete %al; test %al,%al; je` 直用 %eax；accCera 内联为 selectReward
    //   第 2 实参（`call getBreakAwayAccureCera; mov 0xc(%ebp),%edx; mov %edx,0xc(%esp);
    //   mov %eax,0x8(%esp)`）。
    // ②P198①：`movb $0x0,-0x9` 函数头预清 + 两臂各自 `mov %al,-0x9; cmpb $0x0,-0x9; je`
    //   ⇒ 具名 ok 两句式（声明预清 + 分支赋值）。
    // ③ok 声明 `unsigned char`（非 bool）：bool 局部 `if (!ok)` 产 X 族 `movzbl; xor $1;
    //   test; jne`（54 vs 50），uchar 才复现 ORIG 的 `cmpb $0x0,-0x9; je` 直判。
    // ④P223：ORIG 两臂各有一个空 then 占位岛（0831d201 nop;jmp / 0831d204 nop）⇒
    //   否定 goto + 尾空标签 `if (ok == 0) goto LskipN; give(); LskipN:;` 复现。
    unsigned char ok = 0;
    if (user->getBreakAwayRewardOrder() == 1)
    {
        ok = selectReward(1, user->getBreakAwayAccureCera(), user);
        // P223：ORIG 两臂各有一个 `je <nop 岛>` 中继（0831d201 nop;jmp / 0831d204 nop）
        // ⇒ 用否定 goto + 尾空标签复现。
        if (ok == 0) goto Lskip1;
        Inter_LoadEtc::giveBreakAwayReward(user);   // ORIG 0x084d8e68
    Lskip1:
        ;
    }
    else
    {
        ok = selectReward(3, user->getBreakAwayAccureCera(), user);
        if (ok == 0) goto Lskip2;
        Inter_LoadEtc::giveBreakAwayReward(user);
    Lskip2:
        ;
    }
