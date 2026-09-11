// ============================================================================
// calDeadUserDungeonPenalty —— ORIG T 0x085ad9b4。
// slot<0 → 0；memberCount(m_battleData.m_field68c=CParty+0x9b8)>1 时
// rate = float(记录[slot])/float(count-1)，ORIG 语义：rate < 0.3f
// （.rodata 0x08cbbbe8 = 0.3f）→ 归零（fucompp 后 ST0=0.3/ST1=rate，test $0x45 判 C0|C2|C3
// 全 0 ⇔ 0.3f>rate；Ghidra 同证 `local_c < _DAT_08cbbbe8`）。
// 死亡计数表基址 this+0x10+(slot+0xe0)*4（ORIG 寻址实证），位于 m_battleData
// （+0x32c）内，超建模段按裸偏移访问。
// ============================================================================
// [b28d-solo-CParty_KillChain N85+N94] ORIG 0x085ad9b4：本波已把槽布局修到与 ORIG 全等
// （rate -0x8 / memberCount -0x4 / fildl 临时 -0x14 / 返回槽 -0x18，帧 $0x18；修法 =
// P0 去 deadCnt 具名局部 + memberCount 前置到 slot 判定之前）。残余两条结构性差：
//   ① `rate < 0.3f` 的 x87 比较 lowering（[M1Free76to90 2026-09-11] 语义轴真修：原码
//      写作 `== 0.3f` 是误读，实为 `<`，已改；语义修正后 4.4 产 `fucomip; fstp; seta %al;
//      test; je`——语义等价但比 ORIG 4.1 的 `fucompp; fnstsw %ax; test $0x45,%ah; sete;
//      test; je` 少 1 条，属跨版本 lowering，N85 封顶）。
//   ② 死亡计数表寻址：ORIG `add $0xe0,%edx; mov 0x10(%eax,%edx,4),%eax`（disp 保留），
//      4.4 恒折 `add $0xe0,%edx; add $0x4,%edx; mov (%eax,%edx,4),%eax`（N94/N4：7 形探针
//      ——成员数组视图/int* 加法/char* 加法/具名指针——全部折叠，源形不可择）。
// P167 CLEAR（本函数无 call 点）。语义已正确（本次真修）；残余仅 N85/N94 结构性差，封顶。
float CParty::calDeadUserDungeonPenalty(int slot)
{
    float rate = 0.0f;
    // P0：ORIG 0x085ad9c2 在 slot 判定前即读入 memberCount 并占槽 -0x4。
    const int memberCount = m_battleData.m_field68c;
    if (slot < 0)
        return rate;

    if (memberCount > 1)
    {
        // P0/P183：ORIG 无 deadCnt 具名槽（0x085ad9eb 直接 fildl 临时槽）——
        // 表达式内联进 float 转换，禁具名 int 局部。
        rate = static_cast<float>(
                   reinterpret_cast<const KcDeadCountView*>(this)
                       ->m_deadCounts[slot + 0xe0])
             / static_cast<float>(memberCount - 1);
        // [M1Free76to90 2026-09-11] 语义轴真修：ORIG x87 序列 `flds rate; flds 0.3f;
        // fucompp; fnstsw %ax; test $0x45,%ah; sete %al; test; je`：ST0=0.3 vs ST1=rate，
        // and(0x45)=C0|C2|C3=0 ⇔ ST0>ST1 ⇔ 0.3f>rate ⇒ al=1（不跳）⇒ 归零。
        // 即 `rate < 0.3f → 0`（Ghidra 同证：`local_c < _DAT_08cbbbe8`）。
        // 历史判词「rate == 0.3f」为误读（0x085ada16 的 `sete` 是 ZF 结果非相等语义）。
        if (rate < 0.3f)
            rate = 0.0f;
    }
    return rate;
}