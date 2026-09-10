void getDefenseRateDamager(float& outDefenseRate, float& outReduceS1,
                           STComputeDamageVariable* v, void* defender)
{
    // 0x08095816：lvl = getComputeDamageBasisLevelAttacker(v, false)
    int basisLevel = getComputeDamageBasisLevelAttacker(v, false);

    float dr = 0.0f;        // -0x24 防御减伤基数
    float s1 = 0.0f;        // -0x10
    float redConst = 0.0f;  // -0xc

    if (v->m_damageType == 0)            // 0x08095847：test 0x5 → 物理分支
    {
        dr = static_cast<float>(v->m_field093);                       // fildl 0x93
        dr = (static_cast<float>(v->m_field097) * kDefRateMul01 + 1.0f) * dr;  // 0x97
        redConst = g_physicalReduceRateConst_;                        // mov 0x93f5d18
        s1 = v->m_field09B * kDefRateMul01;                           // flds 0x9b
        if (v->m_objTypeByte == 9)                                    // 0x1b2
            s1 *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(8));
    }
    else if (v->m_damageType == 1)       // 0x080958d6：cmp $1 → 魔法分支
    {
        dr = static_cast<float>(v->m_field09F);                       // fildl 0x9f
        dr = (static_cast<float>(v->m_field0A3) * kDefRateMul01 + 1.0f) * dr;  // 0xa3
        redConst = g_magicalReduceRateConst_;                         // mov 0x93f5d30
        s1 = v->m_field0A7 * kDefRateMul01;                           // flds 0xa7
        if (v->m_objTypeByte == 9)
            s1 *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(0xA));
    }

    // 0x0809595b：dr *= getAbilityConvertRateSDC(defender, 1)
    dr *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(1));

    // 0x08095976：if (v->m_char1E5) redConst *= 2（fadd st(0),st(0)）
    if (v->m_char1E5 != 0)
        redConst += redConst;

    // 0x0809598c：dr = std::max(dr, 0.0f)
    // [C03 实测] ORIG 该 max 的第一实参是具名 0.0f 局部（-0x20）、第二为 dr（-0x24），
    //   但把 0.0f/1.0f 具名化后 4.4 全帧槽位反而上升 4（dr -0x20→-0x18，本件变差），
    //   已回退；残差属 N85①+槽组族（见下）。
    dr = std::max(dr, 0.0f);

    // 0x080959ae..0x080959de：out0 = dr / max(1.0f, (float)lvl * redConst + dr)
    // [C03 修·配方②（槽复用）] ORIG 把 denom 作为**临时**（0x080959c4 `fstps -0x18`）
    // 而非具名；我方原具名 denom 多开一个 4B 槽并占走 dr 的 -0x24 ⇒ 内联 denom 后
    // **diff 行数 150→58**（其余为 call/jmp 重定位伪差）。反例留痕：补具名 `one`
    // （1.0）后 diff 回到 86 且 dr 落 -0x20 ⇒ 已回退，维持内联形。
    outDefenseRate = dr / std::max(1.0f, static_cast<float>(basisLevel) * redConst + dr);

    // 0x080959e3：if (dr == 0.0f && lvl <= 9) out0 = dr / 400.0f
    //   （fnstsw+test 0x45+sete：非无序且相等，即 C++ dr == 0.0f）
    if (dr == 0.0f && basisLevel <= 9)
        outDefenseRate = dr / kDefRateDrFloorDiv;

    // 0x08095a0c：out1 = s1
    outReduceS1 = s1;
