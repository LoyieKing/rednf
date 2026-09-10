bool filterEvent(int type)
{
    if (type == 2)
        return 0;
    // P231 纯 register 存储类提示（无 asm 绑定，P0 合规）：ORIG 0x08115bf4 中
    // channel/bit 全程无栈槽（cmp $0x10,%eax / shl %cl,%ebx），4.4 裸形恒槽化。
    register unsigned int channel = (unsigned int)G_GameWorld()->GetChannelType();
    if (channel <= 0x10)
    {
        register unsigned int bit = 1U << channel;
        if ((bit & 0x1a100U) != 0)
        {
            // P251/P286：ORIG 0x5d 段是**寄存器**比较决策树（`mov 0x8(%ebp),%eax;
            // cmp $0x39,%eax; jg …`，首键 0x39 = 8 键索引中点）⇒ 原码为
            // switch(type)；if 链在 4.4 恒退化为内存直比 `cmpl $imm,0x8(%ebp)`。
            // P325/P223：ORIG 各 case 命中后走 `nop; jmp +0xc6` 岛并共享函数尾
            // `mov $0x0,%eax`（+0xc6）⇒ break 到 switch 外再 goto 尾标签。
            switch (type)
            {
            case 0xb:
            case 0x36: case 0x37: case 0x38: case 0x39:
            case 0x3d: case 0x3e:
            case 0x5c:
                break;
            default:
                return 1;
            }
            goto L_false;
        }
        else if ((bit & 0x4000U) != 0)
        {
            switch (type)
            {
            case 0xb:
                goto L_false;
            case 0x39:
                goto L_false;
            case 0x3d: case 0x3e:
                goto L_false;
            default:
                return 1;
            }
        }
        else if ((bit & 0x600U) != 0)
        {
            switch (type)
            {
            case 9:
                goto L_false;
            default:
                return 1;
            }
        }
    }
L_false:
    return 0;
