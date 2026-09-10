void setBasicAbilityByLevel(const STMonsterScript* script, ActiveStaticInfo* info,
                            int level, int dungeonIdx, int roleType,
                            bool flagA, bool flagB)
{
    info->clear();                             // +0x19 ActiveStaticInfo::clear
    info->m_level = level;                     // +0x21

    const int diff = level - script->m_levelByDiff[0];        // -0x3c
    const int diffSpan = script->m_levelByDiff[1] -
                         script->m_levelByDiff[0];             // -0x38
    char framePad[0x8];                    // [b28d-solo] P232：ORIG 帧 0x74 的 0x10 空洞
    (void)framePad;
    const int tableIdx = getMonsterTableIndex(script, dungeonIdx,
                                              roleType, flagA, flagB);
    // ORIG 0x088b9685 `mov -0x34(%ebp),%eax; cmp $0x4,%eax; jne`（寄存器比较链）。
    // [D07 试形记录] 单 case switch 形可复现该链（P286 有效）但多 2 条（switch 出口
    // jmp + default 落点），实测 481/484 劣于 if 形 481/482 ⇒ 回退 if，封 P209/N18b。
    if (tableIdx == 4)                         // +0x85 难度表 4 用副本等级查行
        level = dungeonIdx;

    // +0x93..0x15c：g_monsterBaseParameters[tableIdx] 中首列==level 的首个命中；
    // 脚本自带基础表时改取脚本内同下标行（operator[] 无界，忠实 ORIG）。
    const MonsterBaseParameter* param = 0;     // -0x30
    for (std::vector<MonsterBaseParameter>::size_type i = 0;
         i < g_monsterBaseParameters[tableIdx].size(); ++i)
    {
        if (g_monsterBaseParameters[tableIdx][i].m_value[0] == level)
        {
            // ORIG 0x088b96d4..0x088b972a：命中后**先取全局表项**（operator[]×2
            // → -0x30），再由 `size()!=0` 条件覆盖为脚本表项；写成 if/else 会让
            // 4.4 把 size 检查放到 then 块首（ORIG 的 then 首段是全局表取址）。
            param = &g_monsterBaseParameters[tableIdx][i];
            if (script->m_baseParameters.size() != 0)           // +0x102
                param = &script->m_baseParameters[i];
            break;
        }
    }
    if (param == 0)
        return;                                // +0x5f9 未命中直接返回

    // 列映射按 ORIG 访问偏移实证：info+0c←v[2]、+18←v[3] … +28←v[6]
    info->m_hpBase = param->m_value[1];        // +0x169 [v1@+04]→info+04
    info->m_mpBase = param->m_value[2];        //         [v2@+08]→info+0c
    info->m_attack = param->m_value[3];        //         [v3@+0c]
    info->m_magicAttack = param->m_value[4];   //         [v4@+10]
    info->m_defense = param->m_value[5];       //         [v5@+14]
    info->m_magicDefense = param->m_value[6];  //         [v6@+18]
    info->m_statBase11 = param->m_value[7];    //         [v7@+1c]→info+158
    info->m_statBase12 = param->m_value[8];    //         [v8@+20]
    info->m_statBase13 = param->m_value[9];    //         [v9@+24]
    info->m_statBase14 = param->m_value[10];   //         [v10@+28]
    info->m_flag14 = script->m_flag3b8;        // +0x1ed movzbl/movb 直拷

    // +0x206：四属性抗性低/高对 → 元素攻击（GetLinearData(低, 高, 差, 跨度)）
    for (int i = 0; i <= 3; ++i)
    {
        info->m_elemAttack[i] = GetLinearDataForMonsterAbility(
            script->m_resistTable[i][0], script->m_resistTable[i][1], diff, diffSpan);
    }

    // +0x25c：17 组线性能力表对 → attr17Base
    for (int i = 0; i <= 16; ++i)
    {
        info->m_attr17Base[i] = GetLinearDataForMonsterAbility(
            script->m_linearAbilities[i][0], script->m_linearAbilities[i][1],
            diff, diffSpan);
    }

    // +0x2ac..0x39e：速度/恢复/跳跃/重量线性对 → statBase 槽
    info->m_statBase3 = GetLinearDataForMonsterAbility(
        script->m_moveSpeed[0], script->m_moveSpeed[1], diff, diffSpan);
    info->m_statBase4 = GetLinearDataForMonsterAbility(
        script->m_attackSpeed[0], script->m_attackSpeed[1], diff, diffSpan);
    info->m_statBase5 = GetLinearDataForMonsterAbility(
        script->m_castSpeed[0], script->m_castSpeed[1], diff, diffSpan);
    info->m_statBase6 = GetLinearDataForMonsterAbility(
        script->m_hitRecovery[0], script->m_hitRecovery[1], diff, diffSpan);
    info->m_statBase8 = GetLinearDataForMonsterAbility(
        script->m_jumpPower[0], script->m_jumpPower[1], diff, diffSpan);
    info->m_statBase9 = GetLinearDataForMonsterAbility(
        script->m_jumpSpeed[0], script->m_jumpSpeed[1], diff, diffSpan);
    info->m_statBase10 = GetLinearDataForMonsterAbility(
        script->m_weight[0], script->m_weight[1], diff, diffSpan);

    // +0x420：HP/MP 再生对 ÷1000.0f（_DAT_08e084a8）后按最大值折算（截断）
    const int hpRegen = GetLinearDataForMonsterAbility(
        script->m_hpRegenSpeed[0], script->m_hpRegenSpeed[1], diff, diffSpan);
    const int mpRegen = GetLinearDataForMonsterAbility(
        script->m_mpRegenSpeed[0], script->m_mpRegenSpeed[1], diff, diffSpan);
    const float hpFactor = static_cast<float>(hpRegen) / 1000.0f;
    const float mpFactor = static_cast<float>(mpRegen) / 1000.0f;
    info->m_statBase1 = static_cast<int>(
        static_cast<float>(info->getHpMax()) * hpFactor);
    info->m_statBase2 = static_cast<int>(
        static_cast<float>(info->getMpMax()) * mpFactor);

    // +0x30b：float 槽 info+0x1a8（m_statBase27，float 实证）+= param[11]/100.0f
    info->m_statBase27 += static_cast<float>(param->m_value[11]) / 100.0f;

    // +0x333：attr17Base[13] += trunc(param[12]/10.0)（double _DAT_08e084b0）
    info->m_attr17Base[13] += static_cast<int>(
        static_cast<double>(param->m_value[12]) / 10.0);

    // +0x367：paramTable 低/高对 → 元素防御；随后攻击/防御加 param 列
    for (int i = 0; i <= 3; ++i)
    {
        info->m_elemDefense[i] = GetLinearDataForMonsterAbility(
            script->m_paramTable[i][0], script->m_paramTable[i][1],
            diff, diffSpan);
        info->m_elemAttack[i] += param->m_value[13 + i];      // +0x3b7 [param+0x34+4i]
        info->m_elemDefense[i] += param->m_value[17 + i];     // +0x3df [param+0x44+4i]
    }

    info->m_statBase6 += param->m_value[21];   // +0x41c [+54]
    info->m_statBase7 = GetLinearDataForMonsterAbility(
        script->m_field528[0], script->m_field528[1], diff, diffSpan);
    info->m_statBase7 += param->m_value[22];   // +0x46b [+58]

    for (int i = 0; i <= 16; ++i)              // +0x485 i==13 跳过
    {
        if (i == 13)
            continue;
        info->m_attr17Base[i] += param->m_value[23];
    }
