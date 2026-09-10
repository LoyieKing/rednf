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

    // 0x08095b9d..0x08095bb7：if (crit * 100.0f > (float)roll)
    if (crit * kCritRollScale > static_cast<float>(roll))
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
