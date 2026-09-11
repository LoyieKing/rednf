// [b28d-L28 P317 收口 2026-09-09] 三个 assault 结构体的 user-provided 空析构已删
// （ServerParameterScript.h，Main 批准）⇒ 4.4 不再判 non-trivial：
// p167 OUR calls 24 → 19 == ORIG 19 **CLEAN**（原 EXTRA `stAssaultLevelGhostPanaltyD1Ev`
// ×2 + `stAssaultChaosPointGhostPanaltyD1Ev` ×2 + `_Unwind_Resume` 全消，仅余
// C1/C2 同址别名 P93 伪差）；gap 25 → **0**（quick/objdump 实测 267=267）。
// 残余（判 N）：`push %edi` 位置/`sub $imm` vs `add $imm` 帧调整（N82 帧策略）+
// 一处 `mov -0x??(%ebp),%edx` 载入时刻（N7 寄存器角色）。
unsigned int ServerParameterScript::getAssaultGhostTime(int level,
                                                        int chaosPoint)
{
    // [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF（N85）→ 试=④x87 轴：ORIG 0x08a8cb08
    //   `fmuls -0xc(%ebp)` vs 我方 `flds -0xc(%ebp); fmulp %st,%st(1)`（4.4 恒 fld+fmulp）；
    //   按 TimerDispatcher 先例在 overlay 追加 `-march=i486 -mtune=generic` 复编 ⇒ 本件
    //   267/268、gap −1 **逐值不变**（同 TU 的 setDungeonOpen/CheckUnusable 亦不变）
    //   ⇒ x87 lowering 差非 -march 所致、属 4.4 版本族。②prett ③strall CLEAR。⇒ 维持 N85。
    // b28d-solo-ServerParameterScript: DIFF→修（真语义 bug + P243 + P50 复活）
    // ORIG 0x08a8c804 逐段：① time clamp 为 `cmp t0,time; jbe`/`cmp t1,time; jae`
    // 回写形（与源码同）；② chaos 段 `iterator cit;` **默认构造 C1Ev 0x08a8dd24 +
    // `cit = begin()` 赋值**（ORIG 0x08a8c9a5/0x8a8c9cb）⇒ 先声明后赋值（P50 复活）；
    // ③ rate 的 x87 序列 `flds r0; flds r1; flds r0; fsubrp … faddp`（faddp 前无
    // 新 flds）⇒ 原码 `r0 + (r1-r0)/(max-min)*(chaos-min)`（P243 左端写 r0）；
    // ④ **rate clamp 真 bug**：ORIG 首分支 `flds r0; flds rate; fxch; fucompp;
    // seta` ⇒ `rate < r0 → rate = r0`（下限 r0），次分支 `test $0x45; sete` ⇒
    // `rate > r1 → rate = r1`（上限 r1）。旧源码写成 `rate > r0 → rate = r0` 与
    // `rate < r1 → rate = r1`，区间反置（r0<r1 时恒被拉到 r1）⇒ 真语义 bug。
    // ⑤ x87 比较 fucompp/fnstsw/sahf vs 4.4 fucomip/fstp 属 N85（P230 TU 旗标）。
    if (m_ghostLevel4f0.empty())
    {
        return 0xb4;
    }
    std::vector<stAssaultLevelGhostPanalty>::iterator it =
        m_ghostLevel4f0.begin();
    stAssaultLevelGhostPanalty g;
    for (; it != m_ghostLevel4f0.end(); it++)
    {
        if (level >= it->m_min && level <= it->m_max)
        {
            g = *it;
            break;
        }
    }
    unsigned int time = 0;
    // [b28d-L4a 2026-09-11] ORIG 0x08a8c852 起求值序：先 (t1-t0)（0x08a8c85d
    // `sub`）再 (max-min)（0x08a8c874）→ divl → 再 (level-min)（0x08a8c88b）→ imul；
    // ORIG 全函数只用 esi/ebx（无 edi）。我方旧式左端写 (level-min) 使 4.4 先算它
    // 并额外引入 edi（多 push/pop edi + flds/fmulp 共 3 条）⇒ 按 ORIG 把除法子式
    // 写在乘法**左端**（乘法交换，语义等价）。
    time = g.m_t0 + ((g.m_t1 - g.m_t0) / (unsigned int)(g.m_max - g.m_min)) *
                        (level - g.m_min);
    if (time < g.m_t0)
    {
        time = g.m_t0;
    }
    else if (time > g.m_t1)
    {
        time = g.m_t1;
    }
    if (m_ghostChaos4fc.empty())
    {
        return time / 1000;
    }
    if (chaosPoint > 1000)
    {
        chaosPoint = 1000;
    }
    float rate = 1.0f;
    std::vector<stAssaultChaosPointGhostPanalty>::iterator cit;
    cit = m_ghostChaos4fc.begin();
    stAssaultChaosPointGhostPanalty cp;
    for (; cit != m_ghostChaos4fc.end(); cit++)
    {
        if (chaosPoint >= cit->m_min && chaosPoint <= cit->m_max)
        {
            cp = *cit;
            break;
        }
    }
    rate = cp.m_r0 + ((cp.m_r1 - cp.m_r0) / (float)(cp.m_max - cp.m_min)) *
                         (float)(chaosPoint - cp.m_min);
    if (rate < cp.m_r0)
    {
        rate = cp.m_r0;
    }
    if (rate > cp.m_r1)
    {
        rate = cp.m_r1;
    }
    time = (unsigned int)((long double)time * rate);
    return time / 1000;
}