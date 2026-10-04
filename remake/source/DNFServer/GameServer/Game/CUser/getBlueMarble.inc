// b28d-lane5 AE: 本轮核验通过（orig/our 归一逐条相等）| ORIG 0x086552e6 | _ZN5CUser13getBlueMarbleEv
void* CUser::getBlueMarble()
{
    // 2026-09-10 gA（A 组）负结论 N（21 vs 22 唯一差）：ORIG 0x0865531c `js` 目标的
    //   失败块为 `mov $0x0,%eax`（无 nop），我方在同址多一条**布局 nop**
    //   （`jmp <epi>` 后、`mov $0x0,%eax` 前）——gcc `-falign-jumps` 产物，源码层
    //   不可控（recover_stamina 呈镜像：ORIG 有 nop 而我方无）⇒ §6.1 对齐豁免。
    // ORIG 0x8??：无 idx 槽——`movzwl mem,%eax; test %ax,%ax; js`（ushort 零装载
    // + js 符号判、body fall-through）+ 二次装载 `movswl %ax,%ebx`。关键：short→int
    // 的拓宽必须**作为普通实参直传**（隐式转换），4.4 才会在 %ax 上直接出 2 操作数
    // movswl 并入 callee-saved %ebx（跨 G_CGameManaging 调用存活）；若写成
    // `register int idx asm("ebx")` 中间变量则 4.4 退化成 cwtl+mov（probe s3 实证）。
    // 尾 `call;jmp merge` + `mov $0` 尾岛（goto 钉块序，技巧 23）。
    if (m_blueMarbleIndex < 0)
        goto RET0;
    return G_CGameManager()->getBlueMarble(m_blueMarbleIndex);
RET0:
    return 0;
}