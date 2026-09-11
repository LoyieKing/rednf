// ===========================================================================
// 1) CUser::IsOldLotteryItem(unsigned int) @0x08673342 —— 报告全文直转
// ===========================================================================
bool CUser::IsOldLotteryItem(unsigned int itemIdx)
{
    // b28d-solo-W8_CU_A：N 封顶（N7/N18a 寄存器化 + 跨版本块布局；原 b28d-35
    //   P85/P209 判词同族）。ORIG 0x08673342：形参一次载入 %eax 后全程
    //   `cmp $imm,%eax` 复用，三处失败路径 tail-merge 到单一 mov $0 尾块
    //   (0x59)、成功汇 0x52。4.4.7 恒「形参槽内存直比 + 失败块就地散布」。
    //   本轮五形实测（探针 /tmp/probe_ol.cpp）：①当前嵌套 if 37 条；②register
    //   局部 37 条（值落 %ebx+push/pop，ORIG 为 %eax 无 push）；③显式 early
    //   return 36 条；④goto 尾标签 45 条（4.4 在 goto 岛补 nop+jmp）——四形
    //   mnemonic 序列均与 ORIG 31 条不符，语义/边界全等 ⇒ 封顶。
    // [round2-L30 复核·N18b（Main 反误判规则 2）] widthscan `cmp_l orig=0 our=6`：
    //   ORIG 实为**两段寄存器比较**（`mov 0xc(%ebp),%eax; cmp $0x1d56,%eax; ja`，
    //   形参一次载入 %eax 全程复用；我方 4.4 退化为 `cmpl $imm,0xc(%ebp)` 槽直比）
    //   ⇒ 判 N18b，非类型宽度问题（不改类型）。
    // [E04 2026-09-10 实验·回退] 失败出口合并为单一尾标签（P325 变体）：实测
    //   36 条（较原 37 少 1）但生成 `nop; jmp` 中继岛、形参仍未入寄存器 ⇒ 仍 DIFF
    //   且远离 ORIG 31 条，已回退。ORIG 的 tail-merge 共享出口（0x52/0x59）+
    //   形参一次载入 %eax 属 4.1 前端产物，4.4 不可复现 ⇒ N18b 维持。
    // [E11·形态⑤ 成功] 6 段区间并集（见下 return）实测 our 37→**31**（orig 31，
    //   gap −6→0）：单一 return 使 4.4 也汇出 `mov $1; jmp end / mov $0` 双尾块，
    //   与 ORIG 条数全等。残余形态：ORIG 为 `mov 0xc(%ebp),%eax` 一次载入 + 边界
    //   cmp/jcc + 3× `sub imm; cmp` 复用；4.4 恒 `cmpl $imm,0xc(%ebp)` 槽直比
    //   （N18b，槽直比不可消除）⇒ 判词 N18b 保留，读数为 gap 0。
    // [P8S3 2026-09-11 真 switch 落地·推翻 N18b 封顶] 真 switch（P261）实证：
    //   ORIG 0x08673342 的 range-check 树 = GCC 对 `case A ... B:` 的生成
    //   （`cmp $0x1d56,%eax; ja` / `sub $0x1c15,%eax; cmp $0x4,%eax; ja` +
    //   共享 0x8673394 `mov $1` / 0x867339b `mov $0` 尾块）。改真 switch 后
    //   件级 verdict=AE（31/31，gap 0）、TU diff 5→4 / ae 71→72。
    switch (itemIdx)
    {
    case 0x1bc6u ... 0x1bf5u:
    case 0x1c15u ... 0x1c19u:
    case 0x1d4cu ... 0x1d56u:
    case 0x1dcau ... 0x1dcfu:
    case 0x1e44u ... 0x1e5cu:
    case 0x1ebau ... 0x1ebbu:
        return true;
    default:
        return false;
    }
}