// [b28d-solo-CUserOnlyOrig IDENTICAL] ORIG 0x082300d0：调用点/形态已对齐（本波修法见函数内注释）。
bool CUser::IsState(ch_state state) const
{
    // ORIG 0x082300d0：`mov 0x8cfc4(%eax),%eax; mov %eax,%edx; mov 0xc(%ebp),%eax;
    // cmp %eax,%edx; sete %al; pop %ebp; ret`（无栈帧）。
    // [b28d-solo-CUserOnlyOrig P0/P254] 原写法 `int cur=m_chState; int v=(int)state;`
    // 引入双槽（-0x8/-0x4）+ sub $0x10 帧；ORIG 无槽 ⇒ 原码是 enum 字段直比表达式。
    // 探针 /tmp/isstate_probe.cpp f1（enum 字段 × enum 形参）逐条复现 ORIG（4.1/4.4
    // 均保留 eax→edx 中转）；f3/f4 具名局部形产槽帧（劣）。
    return m_chState == state;
}