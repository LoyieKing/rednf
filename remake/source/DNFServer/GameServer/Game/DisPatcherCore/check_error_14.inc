// [b28d-solo-DisPatcherCore] 复核 DIFF：正极性形实测 32→31 insn（ORIG 28）；
// 尾段已与 ORIG 同形（`test; jne <mov $0>` + fall `mov $0x13`），残余 3 条：
// ① 4.4 把 register state 落 callee-saved %esi（ORIG 留 %eax：call 返回值直判+
// `mov %eax,%ecx` 复用）⇒ push/pop esi + mov/cmp/mov 共 4 条；② ORIG `mov $1,%edx;
// mov %edx,%ebx` 立即数物化 2 条 vs 4.4 `mov $1,%ebx` 1 条；③ ja 落点块布局。
// 两句式/goto 形均劣 ⇒ N7+N18a 封顶。
int DisPatcher_RepairEquip::check_error(CUser* user, MSG_BASE& /*msg*/)

{
    // [b28c-30] N96 封顶：原 asm("eax")/asm("edx") 绑定已按 P0 清除。
    // ORIG 0x081c61bb `call get_state; cmp $0xc,%eax; ja` 无槽直判 +
    // `mov $0x1,%edx; mov %edx,%ebx` 物化；清绑后纯 register 用 %esi/%ebx（35 insn）、
    // 普通局部落槽（34 insn）均不可复现（4.4 恒 callee-saved/槽化）。
    if (user == 0)
    {
        return 0xffffffff;
    }
    // ORIG 0x81c61b6..：get_state 结果无槽（`cmp $0xc,%eax; ja L13` 直判）；
    // `mov $0x1,%edx; mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx; mov %ebx,%eax;
    // and $0x15e8,%eax; test; jne` —— 1 物化到 %edx（register asm 绑定）、移位累加器
    // 落 %ebx（callee-saved 已 push）；ja 与 !bit 共享 L13 fall 块。
    // ORIG 081c61bb `cmp $0xc,%eax; ja`→共享 0x13 块、+0xd4 `test; jne`→ret0 块。
    // 4.4 需 goto 形才能把「mov $0x13; jmp」排在 jne 之后（直落共享块），
    // OK 岛由 ja 落点承接（技23/17）。残留 mov %eax,%edx = N22 变体天花板。
    // [b28d-29] P0 清绑 + P231 纯 register（无 asm 绑定，合规）：ORIG 该值无槽
    // （`cmp $0xc,%eax` 直判后 `mov %eax,%ecx` 复用），字面量 1 无具名局部
    // （`mov $0x1,%edx` 直出）。
    // [M8BigTURest 2026-09-11 03:41 复核修复] 上一版误删初始化 ⇒ state 未定义值、
    // 且 `call _ZN5CUser9get_stateEv` 整条消失（p167 会 MISSING）。恢复 ORIG
    // 0x081c61bb `call get_state` 的赋值（P0：值无槽、直判可用 register 承载）。
    register unsigned int state = (unsigned int)user->get_state();
    // [DPC_CheckErr] P324/块序轴：ORIG 0x081c61be `ja 0x081c61d6` 与 0x081c61d4
    // `jne 0x081c61dd` 两跳的落点关系 ⇒ **错误块共享**（0x081c61d6 为
    // `mov $0x13,%eax; jmp 0x081c61e3`，0x081c61dd 才是 `mov $0x0,%eax`）
    // ⇒ 原码是**单条 if（两条件 `||`）**；我方原两条独立 if 令 4.4 各生成一个
    // 重复错误块（各 2 条），且第一个块用 `jbe` 反极性落正常路径。
    // [b28f-M8DPA] 破 N6（4.4 折 shr/mask）：`(unsigned long)` 掩码 cast 使
    // fold 模式失效 ⇒ 恢复 ORIG 0x081c61c0..0x081c61db 的 shl 序列
    // （`mov $0x1,%edx; mov %edx,%ebx; mov %eax,%ecx; shl %cl,%ebx;
    //  mov %ebx,%eax; and $0x15e8,%eax`）与 err-fallthrough/ok-jne 块序
    // （ORIG 0x081c61d6 err 块、0x081c61dd ok 块+nop）。实测 30 insn（原 30，
    // 但去掉了 shr 折叠与两处布尔化）。残余 N22：具名 register state 的 home
    // 被 4.4 定为 %ebx ⇒ 多 `mov %eax,%ebx` + `mov %ebx,%eax` 2 条（ORIG 该值
    // 直留 %eax，0x081c61bb `cmp $0xc,%eax`/0x081c61c7 `mov %eax,%ecx`）；
    // 试过赋值表达式/逗号/声明分离/声明块内/const register 均仍落 %ebx，
    // 属 4.4 -O0 register home 选择天花板。
    if (state > 0xc || ((1u << state) & (unsigned long)0x15e8u) == 0)
    {
        return 0x13;
    }
    return 0;
}
