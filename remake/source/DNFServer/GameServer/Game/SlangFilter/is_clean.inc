bool SlangFilter::is_clean(const char* param_1)
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