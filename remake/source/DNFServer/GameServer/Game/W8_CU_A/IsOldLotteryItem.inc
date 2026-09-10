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
    if (itemIdx < 0x1d57u) {
        if (itemIdx < 0x1d4cu) {
            if (itemIdx < 0x1bc6u) {
                return false;
            }
            if ((itemIdx > 0x1bf5u) && ((itemIdx - 0x1c15u) > 4u)) {
                return false;
            }
        }
    } else if (itemIdx < 0x1e5du) {
        if ((itemIdx < 0x1e44u) && ((itemIdx - 0x1dcau) > 5u)) {
            return false;
        }
    } else if ((itemIdx - 0x1ebau) > 1u) {
        return false;
    }
    return true;
