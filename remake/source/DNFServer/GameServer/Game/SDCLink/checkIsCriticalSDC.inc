// ============================================================================
// 6) _Z18checkIsCriticalSDCP23STComputeDamageVariablePfPj @0x08095a16 (0x21c)
//    暴击判定：累计暴击率 → SDC_Rand()%10000 对比 → 命中写 *dmg 倍率与 m_isCritical。
// [b28d-lane15 判词·N（N85① + 常量装载形）] ORIG `push %ebx; sub $0x34`，kCritRollScale
// 常量地址先 `mov $HEX,%eax; mov %eax,-0x10` 入槽再 `flds -0x10`；4.4 直接
// `flds 0x30` 立即数寻址、帧 0x28、无 push ebx ⇒ float 常量/中转槽整体平移。
// 语义等价，封顶。
// [b28d-L28 重核·修 10 条，gap 9→-1] 判据①(jcc 目标下标) + p167 复核后按 P244/P243/
// P232 翻正两处**真缺口**（非 N85）：
//  ① ORIG 0x08095a2d `mov $0x40400000,%eax; mov %eax,-0x10(%ebp)`（3.0f 常量→槽二跳，
//     P244）⇒ 原码为具名 float 常量局部 `baseCrit`（P0：有槽往返 ⇒ 有具名局部），
//     加法按 ORIG `flds 成员; flds 槽; faddp` 序写 `v->m_f00F[1] + baseCrit`（P243）。
//  ② ORIG 0x08095b6a 的 rem 为**有符号**序列（`imul %edx`(0x68db8bad) + `sar $0xc` +
//     `sar $0x1f` 修正 + `imul $0x2710`）⇒ 源码须先转 int 再对 int 常量取余；
//     旧 `SDC_Rand(...) % 10000u` 产无符号 `mul/shr` 序列 —— 除形态差外，负值语义
//     亦不同（真 bug 修复）。
// 残余（不可翻）：③ `flds 槽 + faddp` 被 4.4 折 `fadds 槽`（N85③ 内存操作数折叠）；
// ④ 我方 rem 展开多 1 对 store/load（N87 固有）；⑤ ORIG 同槽 `fildl`+`flds` 双视图
// （局部 union 视角，P260 不适用于自动局部）。判 N（N85③+N87）。
// ============================================================================
bool checkIsCriticalSDC(STComputeDamageVariable* v, float* dmg, unsigned int* randState)
{
    if (dmg == 0)                                   // 0x08095a1d：cmpl $0,0xc(%ebp)
        return false;

    // [b28d-L28 重核] ORIG 0x08095a2d `mov $0x40400000,%eax; mov %eax,-0x10(%ebp)`（常量
    // →槽二跳，P244/P232）后 `flds 0x13(%ebx); flds -0x10(%ebp); faddp`（成员先 fld，
    // P243）⇒ 原码是具名 float 常量局部 + 成员在左的加法。
    float baseCrit = 3.0f;
    float crit = v->m_f00F[1] + baseCrit;

    if (v->m_char1B6 != 0)                          // 0x1b6
    {
        if (_SDC::isObjectType(v->m_attackerObjType, 0x81))
            crit += v->m_f05B;                      // flds 0x5b
    }
    else if (_SDC::isObjectType(v->m_attackerObjType, 0x11))
    {
        if (v->m_damageType == 0)
            crit += v->m_f04B;                      // flds 0x4b
        else if (v->m_damageType == 1)
            crit += v->m_f04F;                      // flds 0x4f
    }

    if (v->m_char1DE != 0)                          // 0x1de
    {
        crit += v->m_f00F[2];                       // flds 0x17
        if (_SDC::isObjectType(v->m_attackerObjType, 0x11))
        {
            if (v->m_damageType == 0)
                crit += v->m_f053;                  // flds 0x53
            else if (v->m_damageType == 1)
                crit += v->m_f057;                  // flds 0x57
        }
    }

    if (_SDC::isObjectType(v->m_defenderObjType, 0x11))   // 0x1c2
        crit -= v->m_field0CF;                      // flds 0xcf

    // 0x08095b6a：roll = SDC_Rand(randState) % 10000（有符号 rem，0x68db8bad/0x2710 序列）
    // [b28d-L28 重核] ORIG 0x08095b6a 出**有符号** rem 序列（`imul %edx`(0x68db8bad)
    // + `sar $0xc` + `sar $0x1f` 修正 + `imul $0x2710`），我方旧 `% 10000u` 走无符号
    // `mul/shr` 序列 ⇒ 原码是先转 int 再对 int 常量取余。
    // [C03 试验·F2c 轴 A（const 限定符）] 去 const 后 our_insns 171→**172**、
    // 帧 0x34→0x24（反劣）⇒ 已回滚；本件保留 const（4.4 未对其做立即数折叠）。
    const int roll = static_cast<int>(SDC_Rand(randState)) % 10000;

    // 0x08095b9d..0x08095bb7：ORIG 形为 `fildl roll; flds crit; flds 100.0f; fmulp;
    // fucompp; fnstsw %ax; test $0x45,%ah; sete`（**无 fxch**）；写成 `crit*100 > roll`
    // 时 4.4 先算左操作数 ⇒ `flds; flds; fmulp; fildl; fxch; sahf; seta`（多 fxch 且
    // flags 通道不同）。[R13fixX87] 操作数互换 + 极性反转（`roll < crit*100`）即产 ORIG 形；
    // 有序域等价，NaN 域两侧同为 `sete` 判定（无序 ⇒ 假 ⇒ 非暴击）⇒ 可观察行为一致。
    if (static_cast<float>(roll) < crit * 100.0f)
    {
        float critMul = g_criticalDamageDefaultRate_;     // mov 0x936e4ac（=1.5f 初值）
        if (_SDC::isObjectType(v->m_attackerObjType, 0x11))
            critMul *= v->m_f06F;                   // flds 0x6f
        critMul += v->m_field073;                   // flds 0x73
        *dmg *= critMul;
        v->m_isCritical = 1;                        // movb $1,0x1ce
    }
    else
    {
        v->m_isCritical = 0;                        // movb $0,0x1ce
    }
    return v->m_isCritical != 0;
}