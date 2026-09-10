void PrintBackTrace() {
    void* frames[100];
    // ORIG DWARF（line 455-457）：buffer、stack_strings、nptrs 声明序为
    // stack_strings 在前、nptrs 在后（§40 栈槽反序分配 → nptrs@-0x10、
    // stack_strings@-0x14）。写成 count 在前会产生槽位互换 DIFF。
    char** symbols;
    int count;
    count = backtrace(frames, 100);
    symbols = backtrace_symbols(frames, count);
    // 原始：if (== NULL) return; 早退形态（§3.3 共享出口 jmp+nop），
    // 循环变量名 index（ORIG DWARF decl_line 464）。
    if (symbols == NULL) {
        return;
    }
    for (int index = 0; index < count; index++) {
        // 原始：先取 symbols[index] 到局部变量（触发 ebx 保存，ctor 调用前取值）
        register char* s = symbols[index];
        // 行号实参以各服务 ORIG 二进制实测为准：
        //   coserver: 0x186=390（DWARF decl_line 465=0x1d1 是伪 C 还原
        //   的猜测值，与 coserver 二进制不符，2026-08-11 修正）
        //   point/auction/guild: 0x1d1=465（对应服务 ORIG 实测）。
        // 四服务 ORIG 第四参实测均为 _ZZ14PrintBackTracevE12__FUNCTION__
        //   （__FUNCTION__ 符号：coserver 0x80bb834 / point 0x8154f34 /
        //   guild 0x811aa74 / manager 0x8159bc4），不得用字面量；
        //   statics/monitor 亦为 __FUNCTION__（0x1d1），
        //   见 2026-08-11 coserver/manager 第 2 轮记录。
