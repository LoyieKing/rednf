// ORIG _ZN13user_creature9CCreature19IsEvolutionCreatureEi T 0x83388ec。
// [N96 清绑 2026-09-09] 原 `register bool eq asm("al")` 全域禁用（P0②/N96/P248 尾条），
// 还原 P168a② 具名 bool 局部自然形（`bool eq = (call()==arg); if (eq) return 1;`）。
// 4.4 发 `sete %al; mov %al,-N; cmpb $0,-N; je`（栈槽物化），ORIG 为
// `sete %al; test %al,%al; je`（寄存器物化）⇒ 自然形不可达，残差 = N96+N18a 封顶。
// 读数：清绑前 our_insns=20（TU 口径 AE）；清绑后 our_insns=22 / orig=20（TU 口径 DIFF）。
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
}