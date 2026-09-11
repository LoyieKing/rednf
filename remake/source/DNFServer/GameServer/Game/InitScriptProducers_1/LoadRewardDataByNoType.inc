// b28d-solo-InitScriptProducers_1: AE（修：删 result 局部改直接 return + `goto
// failZero` 尾标签，复现 ORIG 失败块落循环回跳之后 `nop; mov $0,%ebx; jmp`；槽组
// -0xc/-0x10/-0x11/-0x1c/-0x28 与帧 0x30 全对齐）。
// ORIG T 0x08917c99。endTag 形参在 ORIG 函数体内零引用（段终止靠 ScanInt 断流，
// 由调用方主循环消费闭合 tag），保留签名并显式标注。
int LoadRewardDataByNoType(stStepRewardStackableItemScript& script, std::string endTag)
{
    (void)endTag;

    int step;                                    // ORIG -0xc
    std::string deadTok; (void)deadTok;          // ORIG -0x10 默认 ctor 死串（prologue 首 ctor）
    bool ok = false;                             // ORIG local_15
    StackableItemData data;                      // ORIG local_20 函数级 ctor
    std::vector<StackableItemData> vec;          // ORIG local_2c 函数级 vector（每轮 clear）

    for (;;)
    {
        step = 0;                                // ORIG prologue-of-body $0 写
        data.clear();                            // ORIG StackableItemData::clear
        vec.clear();                             // ORIG vector::clear
        step = ScanInt(&ok);
        if (ok != true)
            goto failZero;                       // ORIG 失败块 0x08917d5b 落在循环回跳之后
        data.m_field0 = ScanInt(&ok);
        if (ok != true)
            return 0x3662;
        data.m_field4 = ScanInt(&ok);
        if (ok != true)
            return 0x3666;
        script.set_data(step, data);
    }
failZero:
    return 0;
}
