// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 SlangFilter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "SlangFilter.h"

#include <cstdio>
#include <cstring>
#include <strstream>

#include "SlangFilter/SlangFilter.inc"
    : __slang_filter__FlexLexer(0, 0)
{
}

#include "SlangFilter/dtor_SlangFilter.inc"
{
}

#include "SlangFilter/LexerOutput.inc"
{
    (void)size;
    std::fprintf(stdout, buf);
}

#include "SlangFilter/LexerError.inc"
{
    std::fprintf(stderr, msg);
}

#include "SlangFilter/is_clean.inc"
{
    // [b28d-59 判词·N18a+N7] ORIG 0x08094ef2：`mov %ebx,%eax; mov %eax,-0x24(%ebp)`
    // 两段（4.4 恒折单步，N18a 反向不可造）；虚调用 `mov (%eax),%ecx; call *%ecx`
    // vs 4.4 恒 %eax（N7 间接 call 目标寄存器）。语义等价，封顶。
    // [P2-S4 2026-09-11 复核·N 维持] 逐条核完 ORIG 108 条：全函数仅两处差、合计 2 条，
    //   且**都是我方少**（gap=+2，我方无「ORIG 有我方无」的可造段）：
    //   ① 0x08094f69 `mov %ebx,%eax; mov %eax,-0x24(%ebp)`（this_00 = new 结果的两步
    //      store；4.1 经 %eax 中转后落 -0x24）vs 我方 0x08094f69 处单条 `mov %ebx,-0x24(%ebp)`；
    //   ② 0x08094f83 `mov -0x24(%ebp),%edx` 在 `movl $0x0,0x8(%esp)` **之后**，我方在其之前
    //      （同一条指令的调度位次差，非指令数差）。
    //   r 槽/帧（push %ebx,%esi,%edi; sub $0x3c）与两分支标签布局已全等。不可造，维持 N。
    int iVar3;
    size_t sVar2;
    std::istrstream* this_00;
    char* local_24;
    char* local_20;

    if (param_1 == 0)
        return 0;
    sVar2 = std::strlen(param_1);
    if (sVar2 == 0)
        return 0;

    this_00 = new std::istrstream(param_1, (int)sVar2);
    switch_streams(this_00, 0);
    local_24 = (char*)param_1;
    do
    {
        iVar3 = yylex();
        if (iVar3 == 1)
        {
            local_20 = (char*)param_1;
            local_24 = (char*)std::strstr(local_24, YYText());
            if (local_24 == 0)
                goto next_tok;
            for (; local_20 <= local_24; local_20 = local_20 + 1)
            {
                if (local_20 == local_24)
                    return 0;
            }
            local_24 = local_20;
        }
    next_tok:
        ;
    } while (iVar3 != 2);
    if (this_00 != 0)
        (*(void (**)(std::istrstream*))(*(int*)this_00 + 4))(this_00);
    return 1;
}
