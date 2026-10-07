// computeHpDamageSDC —— ORIG T _Z18computeHpDamageSDCP23STComputeDamageVariablePvS1_
//   @0x08095c32 size 0xac8（0x08095c32..0x080966f9）逐指令转写；gcc4.4.7 -m32 -O0 口径，C++98。
// FPU 语义已用 32 位汇编实测定案：fsubrp=st(1)-st(0)，fdivrp=st(1)/st(0)；x87 (int) 即 trunc。
// 返回 (int)trunc(dmg) 并写 v->m_expectedHp；第二参全程未用【asm】。
int computeHpDamageSDC(STComputeDamageVariable* v, void* /*unusedArg2*/, void* defender)
{
    unsigned int randState = (unsigned int)v->m_randSeed;      // mov 0x1d9 → -0x5c
    int convertArg0 = 0;                                       // -0x58，恒 0【asm：仅初始化与读取】
    int convertArg1 = 0;                                       // -0x54，恒 0【asm：仅初始化与读取】

    float dmg = getDamageInflict(v, defender);                 // 0x08095c56；fstps -0x60

    // 0x08095c6b: m_attackElem != 4 且元素有效 → *1.05f（0x08b12f68）
    // [Lane1Dispatchers 2026-09-10] m_attackElem 为 `unsigned char`（+0x1DD）；ORIG 该点
    //   `movzbl 0x1dd(%eax),%eax; movsbl %al,%eax` 两段 ⇒ 源码是 `(int)(char)…`
    //   （我方旧写 (int) 只出 movzbl）。
    if (v->m_attackElem != 4 && isValidAttackInfoElement(v, (int)(char)v->m_attackElem))
        dmg *= 1.05f;

    // 0x08095cb7: 防御方对象 0x11 且攻击元素无效 → 四元素取 max
    if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && !isValidAttackInfoElement(v, 4))
    {
        // [Lane1Dispatchers 2026-09-10 · 真语义 bug] Ghidra C 该点 `local_50 = -1.0;`
        //   + ORIG 0x08095d0d `mov $0xbf800000,%eax; mov %eax,-0x4c(%ebp)`（-1.0f）；
        //   旧码写 `1.0f`（0x3f800000）⇒ 当四元素计算值全 < 1.0 时 best 保持 1.0
        //   会把伤害抬到 1.0（真 bug）。比较方向 `if (x > best)` 与 ORIG
        //   `if (local_50 < local_48) local_50 = local_48` 等价，一并核对无误。
        float best = -1.0f;                                    // -0x4c
        for (int i = 0; i <= 3; ++i)                           // -0x48
        {
            if (v->m_elemEnable[i] == 0)
                continue;
            float x = dmg * ((g_commonElementBaseConst_
                              - (float)v->m_elemAbility[i] * v->m_f08F) * 0.01f);  // 0x08095d3a
            if (x > best)
                best = x;                                      // 0x08095d74
        }
        dmg = best;                                            // 0x08095d89
    }

    // 0x08095d8f: 随机基
    float randomBase = 0.0f;                                   // -0x50
    if (v->m_damageType == 0)
    {
        randomBase = g_physicalRandomBaseConst_;               // 0x08095da2
        int lvl = getComputeDamageBasisLevelAttacker(v, v->m_char004 != 0);  // -0x3c
        randomBase += (float)lvl * g_physicalRandomRateConst_;
    }
    else if (v->m_damageType == 1)
    {
        randomBase = g_magicalRandomBaseConst_;                // 0x08095deb
        int lvl = getComputeDamageBasisLevelAttacker(v, v->m_char004 != 0);  // -0x38
        randomBase += (float)lvl * g_magicalRandomRateConst_;
    }
    else if (v->m_damageType == 2)
    {
        randomBase = dmg * 0.07f;                              // 0x08095e34（0x08b12f6c）
    }
    if (v->m_objTypeByte == 0xE)                               // 0x08095e42
        randomBase = 0.0f;

    int r = (int)SDC_Rand(&randState);                         // 0x08095e5e
    int rMod = r % 1000;                                       // imul 0x10624dd3 有符号除法
    dmg += GetUniformVelocityFloatSDC(-randomBase, randomBase, rMod, 999);  // 0x08095ea3

    // 0x08095eb0: 防御率链
    if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && v->m_damageType != 2 && v->m_char00E == 0)
    {
        float drA = 0.0f;                                      // -0x64
        float drB = 0.0f;                                      // -0x68
        getDefenseRateDamager(drA, drB, v, 0);                 // 0x08095f23
        float m = (1.0f - drA) - v->m_f05F * drB;              // -0x34
        // [R13fixX87] 0x08095f3f/0x08095f60: ORIG 为**严格小于 clamp**（`fucompp; fnstsw %ax;
        // test $0x45,%ah; sete %al; test; je <skip>` ⇒ 不跳即赋值）：`g_max > m` / `0.0f > m`。
        // 旧写 `m == C`（等值自赋 = 空操作）⇒ 负值/超界不被夹取，与 ORIG 真语义分叉。
        if (m < g_maxTotalDefenseRate)
            m = g_maxTotalDefenseRate;
        if (m < 0.0f)
            m = 0.0f;
        dmg *= m;                                              // 0x08095f7b

        if (v->m_damageType == 0)
        {
            float s1 = v->m_f0AB;                              // 0x08095f8f；-0x30
            if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
                s1 *= getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_4);
        }
        else if (v->m_damageType == 1)
        {
            float s1 = v->m_f0AF;                              // 0x08095fe2；-0x2c
            if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
                s1 *= getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_6);
        }
    }

    // [bughunt13] ORIG 0x08095ef1 `je 0x8096022`：s2 段在防御率 if **之外**（旧码误嵌其内）。
    if (v->m_damageType == 0)                                  // 0x08096022
    {
        float s2 = v->m_f03F;                                  // 0x0809602d；-0x28
        if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
            s2 *= getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_3);
    }
    else if (v->m_damageType == 1)
    {
        float s2 = v->m_f043;                                  // 0x0809607e；-0x24
        if (v->m_objTypeByte != 4 && v->m_objTypeByte != 5)
            s2 *= getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_5);
        (void)v->m_char1EA;                                    // 0x080960be 读取未用【asm 事实】
    }

    if (v->m_damageType == 0)                                  // 0x080960ca 物理分支
    {
        float final1 = v->m_f00F[0];                           // 0x080960d9
        if (v->m_char004 != 0)
            final1 += (float)v->m_field063 * v->m_f0B3;
        final1 *= (v->m_f006 * 0.01f + 1.0f);                  // 0x0809610c
        final1 *= (v->m_f037 * 0.01f + 1.0f);                  // 0x08096126
        // [bughunt13] ORIG 0x0809614e..0x0809615b：`call; fmuls final1; flds dmg; faddp` 单表达式，
        //   乘积不落 float（旧码先 final1*=conv 落盘再加，多一次舍入）。
        dmg = getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_7) * final1 + dmg;  // 0x08096140

        if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && v->m_char00E == 0)
        {
            // [bughunt13] ORIG 0x080961a2..0x080961ca：三步各 fstps 落 float（逐步舍入）。
            float mdef = (float)v->m_physMDefBase;             // 0x08096196；-0x1c
            mdef = v->m_f05F * mdef;
            mdef = (float)v->m_physMDefAdd + mdef;
            dmg = dmg - mdef * getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_8);
        }
        dmg = dmg - dmg * v->m_physReduceRate;                 // 0x080961ef
    }
    else if (v->m_damageType == 1)                             // 0x08096211 魔法分支（对称）
    {
        float final1 = v->m_f00F[0];                           // 0x08096219
        if (v->m_char004 != 0)
            final1 += (float)v->m_field067 * v->m_f0B3;
        final1 *= (v->m_f006 * 0.01f + 1.0f);                  // 0x0809624c
        final1 *= (v->m_f03B * 0.01f + 1.0f);                  // 0x08096266
        // [bughunt13] ORIG 0x0809628e..0x0809629b：同上单表达式。
        dmg = getAbilityConvertRateSDC((void*)convertArg1, CONVERT_TABLE_9) * final1 + dmg;  // 0x08096280

        if (_SDC::isObjectType(v->m_defenderObjType, 0x11) && v->m_char00E == 0)
        {
            // [bughunt13] ORIG 0x080962e2..0x0809630a：同上逐步舍入。
            float mdef = (float)v->m_magicMDefBase;            // 0x080962d6；-0x14
            mdef = v->m_f05F * mdef;
            mdef = (float)v->m_magicMDefAdd + mdef;
            dmg = dmg - mdef * getAbilityConvertRateSDC((void*)convertArg0, CONVERT_TABLE_A);
        }
        dmg = dmg - dmg * v->m_magicReduceRate;                // 0x0809632f
    }

    if (dmg < 0.0f)                                            // [R13fixX87] 0x08096345：ORIG `0.0f > dmg` clamp
        dmg = 0.0f;

    v->m_isCritical = (unsigned char)checkIsCriticalSDC(v, &dmg, &randState);  // 0x08096360

    if (v->m_char1CF != 0)                                     // 0x08096384 暴击扣血段
    {
        // [bughunt13] ORIG 0x08096392 `mov $0x3f8ccccd,%eax; mov %eax,-0x10(%ebp)` ⇒ 局部常量 1.1f
        //   （旧码 0.05f 为误译）；`flds dmg; fmuls -0x10; flds dmg; fsubrp` ⇒ dmg*1.1f - dmg。
        float critMul = 1.1f;                            // -0x10
        v->m_critDamage = (int)(dmg * critMul - dmg);
        dmg = dmg * critMul;                                   // 0x080963c7
    }

    // [bughunt13] ORIG 0x080963d3/0x080963f2：先 `x*dmg` 再 `*0.01f`（结合序与 f[4..6] 不同）。
    dmg = v->m_f00F[3] * dmg * 0.01f;                          // 0x080963d0（0x1b = f[3]）
    dmg = (float)v->m_field1D5 * dmg * 0.01f;                  // 0x080963e6
    if (v->m_char0D3 != 0)                                     // 0x08096405 f[4]=0x1f
        dmg *= v->m_f00F[4] * 0.01f;
    if (v->m_char0D4 != 0)                                     // 0x08096429 f[5]=0x23
        dmg *= v->m_f00F[5] * 0.01f;
    // [bughunt13] ORIG 0x08096450 `movzbl 0xd5(%eax)` = m_char0D5（旧码读 0x1d5 低字节，偏移错）。
    if (v->m_char0D5 != 0)                                     // 0x0809644d f[6]=0x27
        dmg *= v->m_f00F[6] * 0.01f;
    dmg *= v->m_f06B;                                          // 0x08096471

    if (dmg < 1.0f)                                            // [R13fixX87] 0x0809647e：ORIG `1.0f > dmg` clamp
        dmg = 1.0f;

    if (_SDC::isObjectType(v->m_field1C6, 0x111))              // 0x0809649a 暴击矩阵 26 项
    {
        const int row = v->m_isCritical ? 1 : 0;
        for (int i = 0; i <= 0x19; ++i)                        // i ≤ 25
            dmg *= v->m_critMatrix[row][i];
    }

    if (_SDC::isObjectType(v->m_attackerObjType, 0x11) && v->m_f077 != 1.0f)  // 0x08096506（NaN 计入不等）
        dmg *= v->m_f077;                                      // 0x08096546
    if (_SDC::isObjectType(v->m_attackerObjType, 0x11) && v->m_field07B != 0)
        dmg += (float)v->m_field07B;                           // 0x0809658b
    dmg *= v->m_f1A6;                                          // 0x0809659f

    if (v->m_char1DF != 0 && _SDC::isObjectType(v->m_attackerObjType, 0x111))  // 0x080965b0
    {
        // [bughunt13] ORIG 0x080965eb `flds dmg; flds 0.25; fmulp; fistpl -0x70`：dmg*0.25 **不回写** dmg，
        //   仅截断入 n；随后 `fildl n; flds dmg; faddp` ⇒ dmg = dmg + (int)(dmg*0.25f)（×1.25）。
        //   旧码 `dmg *= 0.25f; dmg += n` ⇒ ≈×0.5，真 bug。
        int n = (int)(dmg * 0.25f);                            // trunc；-0x70（0x08b12f70）
        dmg = (float)n + dmg;                                  // 0x0809660c
    }
    if (v->m_char1E0 != 0)
        dmg += (float)v->m_field1E1;                           // 0x0809662b
    if (v->m_char1EF != 0 && _SDC::isObjectType(v->m_attackerObjType, 0x111))
        dmg *= v->m_f1F0;                                      // 0x0809667d
    dmg += dmg * v->m_f1F8 / 100.0f;                           // 0x0809668e（fdivrp 下/上实测）
    dmg += v->m_f1F4;                                          // 0x080966ac

    v->m_expectedHp = (int)dmg;                                // 0x080966bd trunc 写入
    return (int)dmg;                                           // 0x080966e2 trunc 返回
}