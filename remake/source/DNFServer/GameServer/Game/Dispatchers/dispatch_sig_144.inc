int Dispatcher_SecurityCardAuthCancel::dispatch_sig(CUser* user, PacketBuf& packet)
{
    // ORIG 0x0820a42c
    // ORIG cmp $0x1,get_state; setle → state<=1 走断言
    if (user->get_state() <= 1)
    {
        return LineFunc(0xa62d,
                        __PRETTY_FUNCTION__,
                        0, 0);
    }

    if (user->getSecurityCard()->isActivate())
    {
        // [实测] P225 `while (getRequestState() != 1) { goto LOUT; }` 形不产 ORIG 的
        // `setne;test;je <正常路径>`（4.4 仍 jne 直跳）且令 LOUT 前多出 3 条 jmp
        // （ndiff 4→9）⇒ 回退裸 if；该 setne 落点方向按 N（setcc-fold 跨版本）封顶。
        if (user->getSecurityCard()->getRequestState() != 1)
        {
            goto LOUT;
        }
        if (process(user) == 0)   // ORIG call process(CUser*) @0x0820a580
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xaf, 0x7d);
        }
        goto LOUT;
    }

    // ORIG 0x0820a42c 块序：`call isActivate; test %al,%al; je <LineFunc 块>`——
    // 未激活块落在**函数尾部共享出口之前**（激活路径 fall-through）。4.4 对否定早返
    // 恒把 LineFunc 前置 ⇒ 改正向包裹 + 尾部 return（P59/P222 布局钥匙）。
    if (user->getPad()->isActivate())
    {
        if (user->getPad()->getRequestState() != 1)
        {
            goto LOUT;
        }
        if (process(user) == 0)   // ORIG call process(CUser*) @0x0820a580
        {
            user->SendCmdErrorPacket((ENUM_CMDPACKET)0xaf, 0x8b);
        }
        goto LOUT;
    }
    // ORIG 0x0820a42c 尾：`movl $0xa650,(%esp); call LineFunc; leave; ret`
    // —— **直接返回 LineFunc 的 %eax**（不经共享 mov $0 出口）⇒ 必须保留 return 形
    // （改成语句 + 尾 return 0 会把非 0 断言码吞掉，属可观察行为变化）。
    return LineFunc(0xa650,
                    __PRETTY_FUNCTION__,
                    0, 0);
    // ORIG 0x0820a42c 尾：四处 `return 0` 全部 `jmp` 到**唯一**共享出口
    // `mov $0x0,%eax; leave; ret`（P325）；4.4 对独立 return 0 恒就地物化 ⇒ goto 汇聚。
LOUT:
    return 0;
