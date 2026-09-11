// b28d-solo-CEventManager: [N 封顶] P292 档① 已修——ORIG 两处调用点均
// `call; test %al,%al; je`（无 setne）⇒ 返回类型改 bool（TriggerEventStart/End
// 同步归位）。本体 ORIG 0x08115bf4：78 vs 79（round5：82→79，残 1 条）。P222
// 正向包裹 `if (channel <= 0x10) { … } return 0;` 已复现 ORIG 的 `ja <共享尾 mov$0>`
// 方向（旧 `if (channel >= 0x11) return 0;` 生反极 `jbe` + 独立 mov$0 块）。
// [round5] 本轮试形与实测（每形均 check_tu_routed 复算）：
//  ①纯 return 版（4 个独立 `mov $0` 块）= 82；②全 goto 尾标签 = 85（4.4 给每个
//    case 组各生 `nop;jmp` 岛：段1 4 岛 vs ORIG 1 岛）；③全 break+goto = 75
//    （case 直接跳、0 岛）；④**混合**（段1 break→1 岛，段2/段3 goto→3 岛/1 岛）
//    = **79**，岛数与 ORIG 的 1+3+1 逐段全等 ⇒ 取此形。
// 残余 1 条 = channel 寄存器角色（N7）：ORIG 的 register 值留 %eax
// （`cmp $0x10,%eax` / `mov %eax,%ecx`），4.4 把 register 变量落 %ebx ⇒ 多
// `mov %eax,%ebx`+`mov %ebx,%eax` 两条，而掩码区段1 少 `mov %eax,%edx` 复制、
// 段3 多 `mov %ebx,%eax` 恢复（净 +1）。已试并回退：去 register（槽化）、
// 三档类型旋钮（P248）、`==`/`!` 极性、goto/break 形。块体物理序 A/C/B（ORIG）
// vs A/B/C（我方）同属该族。
// [round5 C05 范式复检] 按 Main 转发 C05② 适用边界（常量比较 / 形参 / 跨调用返回值
// 三者均不适用）逐条核：本件两侧皆为**立即数寄存器比较**（`cmp $0x10,%eax` vs
// `cmp $0x10,%ebx`），无内存直比可换，既不属 C05② 命中形（需两个具名局部比较），
// 也非 C05①/③/④（无尾 fail 块、我方岛数已对齐、无 setne 段）⇒ 归 N7 寄存器角色族。
// P56 直分支形 + P133 独立守卫已应用。
// b28d-solo 双后端实测：4.4.7 DIFF / 4.4.6 后端 DIFF（后端不敏感）。
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
}