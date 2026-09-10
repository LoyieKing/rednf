bool CCreature::IsEvolutionCreature(int creatureId)
{
    // round1 R19：ORIG 第二守卫 `cmp 0xc(%ebp),%eax; sete %al; test %al,%al; je`
    // （寄存器物化、无槽）⇒ 按 P225 推广拆嵌套 if 复现 setcc 物化；具名 bool 局部
    // 在 4.4 恒落栈槽（`mov %al,-0x9; cmpb $0,-0x9`，帧 0x28 vs ORIG 0x18），
    // `&&` 合并则被 4.4 折成直跳。
    if (m_script != 0)
    {
        if (m_script->GetEvolutionCreatureId() == creatureId)
            return 1;
    }
    return 0;
