// ============================================================================
// 5) _Z21getDefenseRateDamagerRfS_P23STComputeDamageVariablePv @0x08095810 (0x206)
//    逐指令转写。out0=防御减伤率、out1=力量换算 s1。
//    常量：0x8b12f54=0.01f、0x8b12f60=400.0f、0x8b12f64=100.0f；
//    全局：0x93f5d18=g_physicalReduceRateConst_、0x93f5d30=g_magicalReduceRateConst_
//          （ScriptDepsB.cpp 定义）。
// [b28d-lane15 判词·N（N85①/N82 槽组平移）] 残差为 float 中转槽 -0x24 vs -0x20 的
// 整体浅移 4（ORIG flds/fstps 往返占槽，4.4 mov 直搬省槽），指令序列/长度逐条一致
// （152 vs 151）；常量/全局引用行为 .o 重定位伪差。封顶。
// [C03CryptoInit68 复测·2026-09-10] 已试把 ORIG 的两个具名常量局部补回
// （`float zero=0.0f; dr=std::max(zero,dr);` 与 `float one=1.0f;`，含 max 实参序
// `max(denom,one)`）：dr 槽由 -0x20 变 **-0x18（更远）**，整帧上移 4 ⇒ 反劣，已回退。
// ⇒ 本件槽差不可由具名局部翻正，维持 N85①+N82 封顶。（另：`_ZSt3maxIfERKT_S2_S2_`
// 的 ORIG 形 `fucompp/fnstsw/test $0x45/sete` 需 TU 级 `-march=i486 -mtune=generic`，
// 已按五要素上报 Main；实测该旗标下本 TU ae 20→21、diff 7→6。）
// ============================================================================
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
        dr = (static_cast<float>(v->m_field097) * 0.01f + 1.0f) * dr;  // 0x97
        redConst = g_physicalReduceRateConst_;                        // mov 0x93f5d18
        s1 = v->m_field09B * 0.01f;                                   // flds 0x9b
        if (v->m_objTypeByte == 9)                                    // 0x1b2
            s1 *= getAbilityConvertRateSDC(defender, static_cast<ENUM_CONVERT_TABLE_TYPE>(8));
    }
    else if (v->m_damageType == 1)       // 0x080958d6：cmp $1 → 魔法分支
    {
        dr = static_cast<float>(v->m_field09F);                       // fildl 0x9f
        dr = (static_cast<float>(v->m_field0A3) * 0.01f + 1.0f) * dr;  // 0xa3
        redConst = g_magicalReduceRateConst_;                         // mov 0x93f5d30
        s1 = v->m_field0A7 * 0.01f;                                   // flds 0xa7
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
    // [g09] ORIG 实参槽证据（0x08095994/0x0809599b）：arg1=&dr(-0x24)、arg0=&zero(-0x20)
    //   ⇒ 源码是 max(0.0f, dr)（实参序与旧版相反）。实测：本件 DIFF 3 hunk → 1 hunk。
    dr = std::max(0.0f, dr);

    // 0x080959ae..0x080959de：out0 = dr / max(1.0f, (float)lvl * redConst + dr)
    // [C03 修·配方②（槽复用）] ORIG 把 denom 作为**临时**（0x080959c4 `fstps -0x18`）
    // 而非具名；我方原具名 denom 多开一个 4B 槽并占走 dr 的 -0x24 ⇒ 内联 denom 后
    // **diff 行数 150→58**（其余为 call/jmp 重定位伪差）。反例留痕：补具名 `one`
    // （1.0）后 diff 回到 86 且 dr 落 -0x20 ⇒ 已回退，维持内联形。
    // [g09] ORIG（0x080959c7/0x080959ce）：arg1=&one(-0x1c)、arg0=&denom(-0x18) ⇒ max(denom, 1.0f)。
    outDefenseRate = dr / std::max(static_cast<float>(basisLevel) * redConst + dr, 1.0f);

    // [R13fixX87] 0x080959e3：ORIG `flds dr; fldz; fucompp; fnstsw %ax; test $0x45,%ah;
    //   sete %al; test; je`（**无 fxch**）= 有序且 `0.0 > dr` ⇒ 源码是 **dr < 0.0f**（旧注释
    //   把它读成 `==` 是错的：`==` 在 4.4 产 `fldz; fxch; sahf; jne; jp` 相等判定）。
    //   语义：dr 已被上行 max(0.0f,dr) 夹到 >=0 ⇒ 条件不可观察（照抄 ORIG 形，P171）。
    if (dr < 0.0f && basisLevel <= 9)
        outDefenseRate = dr / 400.0f;

    // 0x08095a0c：out1 = s1
    outReduceS1 = s1;
}