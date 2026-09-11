bool IsValidMultiplyResult(int a, int b)
{
    // [本波·DIFF→AE] ORIG 0x08213e40（34 insns，无栈帧）逐条即 4.1 对
    // `(long long)a * b > 0x7fffffff` 的展开：signed 64 位乘法（`mov %eax,%ebx;
    // sar $0x1f,%ebx` 符号修正 + `imul/mul` 高位）+ 64 位常量比较
    // （hi<0→假、hi>0→真、hi==0→`cmp $0x7fffffff` 无符号比较 lo），
    // 结果经 %ecx 汇聚（`mov $0x1,%ecx` … `mov $0x0,%ecx` … `mov %ecx,%eax`）
    // —— 正是 **三目** `cond ? 0 : 1` 的 4.1 形态（前置 mov $1 即 else 臂）。
    // 旧 b28d-41 判词 N（手写 Ghidra 式 `(a>>31)*b + (b>>31)*a + hi` +
    // 具名 A/B/P/acc 四局部）会多出整段二次展开与 5 个栈槽；按 P0 全部内联
    // 为单一表达式后 4.4 产物与 ORIG 逐条全等（无具名局部、无栈帧）。
    return ((long long)a * (long long)b > 0x7fffffff) ? 0 : 1;
}