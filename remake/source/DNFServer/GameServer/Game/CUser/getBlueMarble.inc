void* CUser::getBlueMarble()
{
    // ORIG 0x8??：无 idx 槽——`movzwl mem,%eax; test %ax,%ax; js`（ushort 零装载
    // + js 符号判、body fall-through）+ 二次装载 `movswl %ax,%ebx`。关键：short→int
    // 的拓宽必须**作为普通实参直传**（隐式转换），4.4 才会在 %ax 上直接出 2 操作数
    // movswl 并入 callee-saved %ebx（跨 G_CGameManaging 调用存活）；若写成
    // `register int idx asm("ebx")` 中间变量则 4.4 退化成 cwtl+mov（probe s3 实证）。
    // 尾 `call;jmp merge` + `mov $0` 尾岛（goto 钉块序，技巧 23）。
    if (m_field8d00c < 0)
        goto RET0;
    return G_CGameManager()->getBlueMarble(m_field8d00c);
RET0:
    return 0;
