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
    register unsigned int state = (unsigned int)user->get_state();
    if (state > 0xc)
    {
        return 0x13;                  // ORIG ja → +0x3a 共享 0x13 块
    }
    // [Lane5] 试形（33→32 insn，ndiff 17→14）：bit 去具名 + `(1u<<state)&0x15e8`
    // 单表达式（ORIG 无 bit 槽、`mov $1,%edx` 直出）。残余=N6（4.4 折 shr/mask）+
    // N22（`mov %eax,%edx` 中转）+ 条件极性（jbe vs ja）+ push/pop esi。
    if (((1u << state) & 0x15e8) == 0)
    {
        return 0x13;
    }
    return 0;
