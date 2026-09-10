bool IsDamageActiveStatus(int status)
// [b28d-solo-OnlyOrigBatch14 判词·N 封顶（P209），本波翻正位测试]
// 本波：位测试改 P231 纯 register（`register int bit = 1 << status;`，非 asm
// 绑定、P0 合规）⇒ 4.4 免槽并复现 ORIG 0x089bd04c 段
// `mov $0x1,%edx; mov %edx,%ebx; shl %cl,%ebx; mov %ebx,%eax; and $0xac4,%eax`
// 的寄存器形（27→25 insn，旧判词「无源形可同时满足 shl 无槽」作废）。
// 唯一残差 = 边界比较：ORIG 0x089bd046 `cmpl $0xb,0x8(%ebp); ja`（无符号内存
// 直比），4.4 对 unsigned 参与比较恒先载寄存器（`mov 0x8(%ebp),%eax;
// cmp $0xb,%eax`，P209）；写 signed 会翻 `jg` 并改变负数语义（真 bug 风险）
// ⇒ N 封顶。p167 CLEAR（0 calls 双侧）。
{
    bool ret = false;
    if ((unsigned int)status <= 0xb)
    {
        // bit 具名局部阻断 4.4 位折叠（N6：直写恒折 (0xac4>>s)&1 的
        // sar+and$1 反演形）；P231 纯 register（非 asm 绑定，P0 合规）
        // 令 4.4 免槽、复现 ORIG `shl %cl,%ebx; mov %ebx,%eax; and $0xac4`
        // 的寄存器形。残差：边界比较 mov+cmp vs ORIG cmpl 槽直比（P209）。
        register int bit = 1 << status;
        if ((bit & 0xac4) != 0)
            ret = true;
        else
            ret = false;
    }
    return ret;
