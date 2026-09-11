// ORIG 0x08232cae W _ZN15DisPatcher_Fish7processEP5CUserR8MSG_BASER9ParamBase
// objdump 实测：push %ebp; mov %esp,%ebp; mov $0x0,%eax; pop %ebp; ret
// —— 原始二进制即为空实现（无 process 报告，W 内联弱符号）。
int DisPatcher_Fish::process(CUser* /*user*/, MSG_BASE& /*msg*/, ParamBase& /*param*/) { return 0; }
