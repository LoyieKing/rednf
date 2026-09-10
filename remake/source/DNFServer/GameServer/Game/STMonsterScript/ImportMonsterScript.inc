bool ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag)
{
    if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path))
        return 0;
    script->Clear();

    getDirectoryPath(script->m_field708, "%s%s",
                     g_szMonsterScriptBaseDirectory, path);
    script->m_aiInfo.m_dir = script->m_field708;
    script->m_throwAttacks.clear();
    script->m_counterAttacks.clear();
    script->m_jumpAttacks.clear();
    script->m_equipment.clear();
    script->m_attackReadies.clear();

    std::string tag;
    std::string token;
    bool ok = false;
    int moduleId = -1;
    int subId = -1;
    bool nameSeen = false;
    while (ScanType(tag, true))                // [b28d-solo] 去 scanTag wrapper（p167 EXTRA）
    {
        moduleId = -1;
        subId = -1;
        const bool dispatch = processModuleTagSet(
            moduleId, subId, &tag, type, false);
        if (subId != -1)
            script->m_field1c9c = subId;
        if (!dispatch)
            continue;

        nameSeen = false;
        int monsterTagId = -1;
        for (;;)
        {
            static std::map<const char*, int, less_TCHAR> __lookup;
            static SpinLock __init_lock;
            static bool __inited = false;
            if (!__inited)
            {
                __init_lock.enter();
                if (__inited)
                {
                    __init_lock.leave();
                    continue;
                }
                monsterTagId = 0;
            }
            else
            {
                std::map<const char*, int, less_TCHAR>::const_iterator __it =
                    __lookup.find(tag.c_str());
                if (__it != __lookup.end())
                    monsterTagId = __it->second;
            }
        switch (monsterTagId)
        {
case 0:
case 0xa14:  // "[name]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[name]", 2580));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_name);  // +0x4
            script->m_altName = script->m_name;
            nameSeen = true;
            break;
        }

case 0xa1a:  // "[width]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[width]", 2586));
            /* FALLTHRU */
        }
        else
        {
            script->m_width = ScanInt(&ok);  // +0xc
            script->m_height = ScanInt(&ok);  // +0x10
            break;
        }

case 0xa1e:  // "[floating height]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[floating height]", 2590));
            /* FALLTHRU */
        }
        else
        {
            script->m_floatingHeight = ScanInt(&ok);  // +0x14
            break;
        }

case 0xa21:  // "[layer]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[layer]", 2593));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);  // ORIG 08a13905：ScanStr→-0x1a10(token)→getDrawLayer→+0x18
            script->m_layer = getDrawLayer(token.c_str());  // +0x18
            break;
        }

case 0xa25:  // "[category]" → ScanTypeOrStr + getObjectCategory → m_field01c
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[category]", 2597));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_field01c.clear();
            //  ORIG 08a13994..08a13ac8：block 级 string -0x1a28 + isType -0x1a29；
            //  ScanTypeOrStr 失败 → return 0；isType 时 **!=** "[/category]" 即 return 0；
            //  getObjectCategory(std::string 值参拷贝临时 -0x1994)；push_back((int)cat 临时
            //  物化 -0x1990 → OOi)。
            std::string catstr;                     // ORIG -0x1a28
            bool isType = false;                    // ORIG -0x1a29
            while (true)
            {
                if (!ScanTypeOrStr(isType, catstr, true))
                    return 0;
                if (!isType)
                {
                    int category = getObjectCategory(std::string(catstr));
                    if (category == 0x2c)
                        continue;
                    script->m_field01c.push_back((int)category);
                    continue;
                }
                if (catstr != "[/category]")
                    return 0;
                break;
            }
        }
            break;
        }

case 0xa3c:  // "[ambient sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ambient sound]", 2620));
            /* FALLTHRU */
        }
        else
        {
            // [b29-SweepSTMonsterScript2] ORIG 0x8a13b44..0x8a13b7e：`ScanStr(&token)`
            // 失败即 `mov $0,%ebx; jmp 0xfabf`（return false），成功才
            // `script->m_ambientSound = token`（`_ZNSsaSERKSs` @0x8a13b79，目标 +0x6ac）。
            if (!ScanStr(&token))
                return 0;
            script->m_ambientSound = token;  // +0x6ac
            break;
        }

case 0xa41:  // "[attack sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack sound]", 2625));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13be0..0x8a13c1a：同上（目标 +0x6a8，operator= @0x8a13c15）。
            if (!ScanStr(&token))
                return 0;
            script->m_attackSound = token;  // +0x6a8
            break;
        }

case 0xa46:  // "[appear sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[appear sound]", 2630));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13c7c..0x8a13cb6：同上（目标 +0x6a4，operator= @0x8a13cb1）。
            if (!ScanStr(&token))
                return 0;
            script->m_appearSound = token;  // +0x6a4
            break;
        }

case 0xa4b:  // "[damage sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage sound]", 2635));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13d18..0x8a13d52：同上（目标 +0x69c，operator= @0x8a13d4d）。
            if (!ScanStr(&token))
                return 0;
            script->m_damageSound = token;  // +0x69c
            break;
        }

case 0xa50:  // "[die sound]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[die sound]", 2640));
            /* FALLTHRU */
        }
        else
        {
            // ORIG 0x8a13db4..0x8a13dee：同上（目标 +0x6a0，operator= @0x8a13de9）。
            if (!ScanStr(&token))
                return 0;
            script->m_dieSound = token;  // +0x6a0
            break;
        }

case 0xa55:  // "[ability category]"（C @LAB_08a13df3 权威：clear+0x7c → while ScanStr 行 →
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ability category]", 2645));
            /* FALLTHRU */
        }
        else
        {
            //   14 类名匹配 PARAMETER_CATEGORY_NAME 表 → 元素 push_back 到 m_parameterCategories）
            {
                static const char* const s_catName[14] = {
                    "[ATTACK_SPEED]", "[MOVE_SPEED]", "[PHYSICAL_ATTACK]",
                    "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
                    "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
                    "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
                    "[HP MAX]", "[HIT_RECOVERY]", "[STUCK]", "[HP MAX BOSS]"
                };
                script->m_parameterCategories.clear();
                STMonsterParameterCategory item;
                while (ScanStr(&token))
                {
                    for (int i = 0; i <= 13; ++i)
                    {
                        if (token == s_catName[i])
                        {
                            item.m_field0 = i;
                            ScanStr(&token);
                            if (token == "+")
                                item.m_field4 = 0;
                            else if (token == "*")
                                item.m_field4 = 1;
                            item.m_field8 = ScanInt(&ok);
                            break;
                        }
                    }
                    script->m_parameterCategories.push_back(item);
                }
            }
            break;
        }

case 0xa70:  // "[ability table]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ability table]", 2672));
            /* FALLTHRU */
        }
        else
        {
            script->m_abilityTable = ScanInt(&ok);  // +0x3bc
            break;
        }

case 0xa73:  // "[summon skill status up rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon skill status up rate]", 2675));
            /* FALLTHRU */
        }
        else
        {
            for (int i = 0; i < 10; ++i)  // C for(i<10) ScanFloat→+0x3c4+4i（m_field3c4）
                script->m_field3c4[i] = ScanFloat(&ok);
            break;
        }

case 0xa77:  // "[level]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[level]", 2679));
            /* FALLTHRU */
        }
        else
        {
            script->m_levelByDiff[0] = ScanInt(&ok);  // +0x3ec
            script->m_levelByDiff[1] = ScanInt(&ok);  // +0x3f0
            break;
        }

case 0xa7b:  // "[fire resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fire resistance]", 2683));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[0][0] = ScanInt(&ok);  // +0x3f4
            script->m_resistTable[0][1] = ScanInt(&ok);  // +0x3f8
            break;
        }

case 0xa7f:  // "[water resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[water resistance]", 2687));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[1][0] = ScanInt(&ok);  // +0x3fc
            script->m_resistTable[1][1] = ScanInt(&ok);  // +0x400
            break;
        }

case 0xa83:  // "[dark resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dark resistance]", 2691));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[2][0] = ScanInt(&ok);  // +0x404
            script->m_resistTable[2][1] = ScanInt(&ok);  // +0x408
            break;
        }

case 0xa87:  // "[light resistance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[light resistance]", 2695));
            /* FALLTHRU */
        }
        else
        {
            script->m_resistTable[3][0] = ScanInt(&ok);  // +0x40c
            script->m_resistTable[3][1] = ScanInt(&ok);  // +0x410
            break;
        }

case 0xa8b:  // "[HP regen speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[HP regen speed]", 2699));
            /* FALLTHRU */
        }
        else
        {
            script->m_hpRegenSpeed[0] = ScanInt(&ok);  // +0x49c
            script->m_hpRegenSpeed[1] = ScanInt(&ok);  // +0x4a0
            break;
        }

case 0xa8f:  // "[MP regen speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[MP regen speed]", 2703));
            /* FALLTHRU */
        }
        else
        {
            script->m_mpRegenSpeed[0] = ScanInt(&ok);  // +0x4a4
            script->m_mpRegenSpeed[1] = ScanInt(&ok);  // +0x4a8
            break;
        }

case 0xa93:  // "[move speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[move speed]", 2707));
            /* FALLTHRU */
        }
        else
        {
            script->m_moveSpeed[0] = ScanInt(&ok);  // +0x4ac
            script->m_moveSpeed[1] = ScanInt(&ok);  // +0x4b0
            break;
        }

case 0xa97:  // "[attack speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack speed]", 2711));
            /* FALLTHRU */
        }
        else
        {
            script->m_attackSpeed[0] = ScanInt(&ok);  // +0x4b4
            script->m_attackSpeed[1] = ScanInt(&ok);  // +0x4b8
            break;
        }

case 0xaa3:  // "[cast speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[cast speed]", 2723));
            /* FALLTHRU */
        }
        else
        {
            script->m_castSpeed[0] = ScanInt(&ok);  // +0x4bc
            script->m_castSpeed[1] = ScanInt(&ok);  // +0x4c0
            break;
        }

case 0xaa7:  // "[hit recovery]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hit recovery]", 2727));
            /* FALLTHRU */
        }
        else
        {
            script->m_hitRecovery[0] = ScanInt(&ok);  // +0x4c4
            script->m_hitRecovery[1] = ScanInt(&ok);  // +0x4c8
            break;
        }

case 0xaab:  // "[jump power]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump power]", 2731));
            /* FALLTHRU */
        }
        else
        {
            script->m_jumpPower[0] = ScanInt(&ok);  // +0x4cc
            script->m_jumpPower[1] = ScanInt(&ok);  // +0x4d0
            break;
        }

case 0xaaf:  // "[jump speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump speed]", 2735));
            /* FALLTHRU */
        }
        else
        {
            script->m_jumpSpeed[0] = ScanInt(&ok);  // +0x4d4
            script->m_jumpSpeed[1] = ScanInt(&ok);  // +0x4d8
            break;
        }

case 0xab4:  // "[normal dash speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[normal dash speed]", 2740));
            /* FALLTHRU */
        }
        else
        {
            script->m_normalDashSpeed[0] = ScanInt(&ok);  // +0x4dc
            script->m_normalDashSpeed[1] = ScanInt(&ok);  // +0x4e0
            break;
        }

case 0xab9:  // "[slant dash speed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[slant dash speed]", 2745));
            /* FALLTHRU */
        }
        else
        {
            script->m_slantDashSpeed[0] = ScanInt(&ok);  // +0x4e4
            script->m_slantDashSpeed[1] = ScanInt(&ok);  // +0x4e8
            break;
        }

case 0xabf:  // "[weight]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[weight]", 2751));
            /* FALLTHRU */
        }
        else
        {
            script->m_weight[0] = ScanInt(&ok);  // +0x4ec
            script->m_weight[1] = ScanInt(&ok);  // +0x4f0
            break;
        }

case 0xac3:  // "[superarmor on attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[superarmor on attack]", 2755));
            /* FALLTHRU */
        }
        else
        {
            script->m_superarmorOnAttack = ScanInt(&ok) != 0;  // +0x5a5
            script->m_field5a8 = ScanInt(&ok);  // +0x5a8
            break;
        }

case 0xac7:  // "[sight]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[sight]", 2759));
            /* FALLTHRU */
        }
        else
        {
            script->m_sight = ScanInt(&ok);  // +0x504
            break;
        }

case 0xaca:  // "[come in action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[come in action]", 2762));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_comeInAction);  // +0x530
            script->m_field538 = ScanInt(&ok);  // +0x538
            script->m_field53c = ScanInt(&ok);  // +0x53c
            break;
        }

case 0xacf:  // "[come in action ex]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[come in action ex]", 2767));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_comeInAction);  // +0x530
            ScanStr(&script->m_comeInActionEx);  // +0x534
            script->m_field538 = ScanInt(&ok);  // +0x538
            script->m_field53c = ScanInt(&ok);  // +0x53c
            break;
        }

case 0xad6:  // "[diving sight]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[diving sight]", 2774));
            /* FALLTHRU */
        }
        else
        {
            ScanInt(&ok);  // 局部用途 @8a14bf7
            ScanInt(&ok);  // 局部用途 @8a14c09
            break;
        }

case 0xada:  // "[targeting time term]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting time term]", 2778));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingTimeTerm = ScanInt(&ok);  // +0x574
            break;
        }

case 0xadd:  // "[targeting on attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting on attack]", 2781));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingOnAttack = ScanInt(&ok) != 0;  // +0x580
            break;
        }

case 0xae0:  // "[targeting on damage]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting on damage]", 2784));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingOnDamage = ScanInt(&ok) != 0;  // +0x581
            break;
        }

case 0xae3:  // "[targeting high level]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting high level]", 2787));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingLevel = 0x2;  // +0x584 [literal]
        break;
            break;
        }

case 0xae7:  // "[targeting low level]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low level]", 2791));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingLevel = 0x1;  // +0x584 [literal]
        break;
            break;
        }

case 0xaeb:  // "[targeting high HP]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting high HP]", 2795));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingHp = 0x2;  // +0x588 [literal]
        break;
            break;
        }

case 0xaef:  // "[targeting low HP]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low HP]", 2799));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(&ok) != 0)
                script->m_targetingHp = 0x1;  // +0x588 [literal]
        break;
            break;
        }

case 0xaf3:  // "[targeting nearest]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting nearest]", 2803));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingNearest = ScanInt(&ok) != 0;  // +0x590
        break;
            break;
        }

case 0xaf6:  // "[targeting attacker]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting attacker]", 2806));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingAttacker = ScanInt(&ok) != 0;  // +0x591
        break;
            break;
        }

case 0xaf9:  // "[targeting from direction]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting from direction]", 2809));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingFromDirection = ScanInt(&ok) != 0;  // +0x592
        break;
            break;
        }

case 0xafc:  // "[targeting low physical attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low physical attack]", 2812));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowPhysAttack = ScanInt(&ok) != 0;
            break;
        }

case 0xaff:  // "[targeting low physical defense]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low physical defense]", 2815));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowPhysDefense = ScanInt(&ok) != 0;
            break;
        }

case 0xb02:  // "[targeting low magical attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low magical attack]", 2818));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowMagAttack = ScanInt(&ok) != 0;
            break;
        }

case 0xb05:  // "[targeting low magical defense]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting low magical defense]", 2821));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingLowMagDefense = ScanInt(&ok) != 0;
            break;
        }

case 0xb08:  // "[targeting downed]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting downed]", 2824));
            /* FALLTHRU */
        }
        else
        {
            script->m_targetingDowned = ScanInt(&ok) != 0;  // +0x5a4
        break;
            break;
        }

case 0xb0b:  // "[warlike]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warlike]", 2827));
            /* FALLTHRU */
        }
        else
        {
            script->m_warlike = ScanInt(&ok);  // +0x578
            break;
        }

case 0xb12:  // "[attack delay]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack delay]", 2834));
            /* FALLTHRU */
        }
        else
        {
            script->m_attackDelay = ScanInt(&ok);  // +0x57c
            break;
        }

case 0xb15:  // "[stuckbonus on damage]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[stuckbonus on damage]", 2837));
            /* FALLTHRU */
        }
        else
        {
            script->m_stuckBonusOnDamage[0][0] = ScanInt(&ok);  // +0x4f4
            script->m_stuckBonusOnDamage[0][1] = ScanInt(&ok);  // +0x4f8
            script->m_stuckBonusOnDamage[1][0] = ScanInt(&ok);  // +0x4fc
            script->m_stuckBonusOnDamage[1][1] = ScanInt(&ok);  // +0x500
            break;
        }

case 0xb1b:  // "[item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[item]", 2843));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[0].clear();  // vector<pair<int,int>> @+0x28
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[0].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xb25:  // "[common champion drop item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[common champion drop item]", 2853));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[1].clear();  // vector<pair<int,int>> @+0x34
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[1].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xb31:  // "[hell monster]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hell monster]", 2865));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag3a8 = ScanInt(&ok) != 0;  // +0x3a8
            break;
        }

case 0xb36:  // "[boss drop item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[boss drop item]", 2870));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[3].clear();  // vector<pair<int,int>> @+0x4c
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[3].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xb40:  // "[specific gen rate item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[specific gen rate item]", 2880));
            /* FALLTHRU */
        }
        else
        {
        script->m_field08c.clear();  // vector<pair<int,int>> @+0x8c
        script->m_field088 = ScanInt(&ok);
        if (!ok)
            return 0;
        while (true) {
            int v1 = ScanInt(&ok);
            if (!ok) break;
            ok = false;
            int v2 = ScanInt(&ok);
            if (!ok) break;
            script->m_field08c.push_back(std::pair<int, int>(v1, v2));
        }
            break;
        }

case 0xbb5:  // "[common champion elemental property]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[common champion elemental property]", 2997));
            /* FALLTHRU */
        }
        else
        {
        script->m_championElements.clear();
        while (ScanStr(&token)) {
            for (int i = 0; i < 5; ++i) {
                if (token == getElementName(static_cast<ENUM_ELEMENT>(i))) {
                    script->m_championElements.push_back(i);
                    break;
                }
            }
        }
            break;
        }

case 0xbc8:  // "[super champion drop item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[super champion drop item]", 3016));
            /* FALLTHRU */
        }
        else
        {
            script->m_field028[2].clear();  // vector<pair<int,int>> @+0x40
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field028[2].push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xbab:  // "[death tower item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[death tower item]", 2987));
            /* FALLTHRU */
        }
        else
        {
            script->m_field058.clear();  // vector<pair<int,int>> @+0x58
            while (true) {
                int v1 = ScanInt(&ok);
                if (!ok) break;
                script->m_field058.push_back(std::pair<int, int>(v1, ScanInt(static_cast<bool*>(0))));
            }
            break;
        }

case 0xbd3:  // "[keep distance]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep distance]", 3027));
            /* FALLTHRU */
        }
        else
        {
            script->m_keepDistance = ScanInt(&ok);  // +0x6e0
            break;
        }

case 0xbd6:  // "[is on the object]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[is on the object]", 3030));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag374 = ScanInt(&ok) != 0;  // +0x374
            script->m_field378 = ScanInt(&ok);  // +0x378
            break;
        }

case 0xbdc:  // "[out of targetting]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[out of targetting]", 3036));
            /* FALLTHRU */
        }
        else
        {
            script->m_isOnTheObject = ScanInt(&ok) != 0;  // +0x6e4
            break;
        }

case 0xbe0:  // "[attack kind]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack kind]", 3040));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_attackMethods.clear();
            for (;;)
            {
                STAttackMethod method;
                float value = ScanFloat(&ok);
                if (!ok)
                    break;
                method.m_field08 = value;      // [b28d-solo] 同型 float 直赋（原 memcpy 为 EXTRA）
                value = ScanFloat(static_cast<bool*>(0));
                method.m_field0c = value;
                value = ScanFloat(static_cast<bool*>(0));
                method.m_field10 = value;
                method.m_field00 = ScanInt(static_cast<bool*>(0)) != 0;
                method.m_field18 = ScanInt(static_cast<bool*>(0));
                method.m_field14 = ScanInt(static_cast<bool*>(0));
                method.m_field20 = ScanInt(static_cast<bool*>(0));
                method.m_field1c = ScanInt(static_cast<bool*>(0));
                method.m_field28 = ScanInt(static_cast<bool*>(0));
                method.m_field24 = ScanInt(static_cast<bool*>(0));
                method.m_field30 = ScanInt(static_cast<bool*>(0));
                method.m_field2c = ScanInt(static_cast<bool*>(0));
                script->m_attackMethods.push_back(method);
            }
        }
            break;
        }

case 0xbfa:  // "[die effect]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[die effect]", 3066));
            /* FALLTHRU */
        }
        else
        {
            script->m_dieEffect[0] = ScanInt(&ok);  // +0x6fc
            script->m_dieEffect[1] = ScanInt(&ok);  // +0x700
            script->m_dieEffect[2] = ScanInt(&ok);  // +0x704
            break;
        }

case 0xbff:  // "[face image]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[face image]", 3071));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_faceImage);  // +0x5ac
            script->m_field5b0 = ScanInt(&ok);  // +0x5b0
            break;
        }

case 0xc03:  // "[waiting motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[waiting motion]", 3075));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_waitingMotion = token;  // +0x5b4
            break;
        }

case 0xc0f:  // "[move motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[move motion]", 3087));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_moveMotion = token;  // +0x5b8
            break;
        }

case 0xc1b:  // "[sit motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[sit motion]", 3099));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_sitMotion = token;  // +0x5c8
            break;
        }

case 0xc27:  // "[damage motion 1]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage motion 1]", 3111));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_damageMotion[0] = token;  // +0x5cc
            break;
        }

case 0xc33:  // "[damage motion 2]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage motion 2]", 3123));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_damageMotion[1] = token;  // +0x5d0
            break;
        }

case 0xc3f:  // "[down motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[down motion]", 3135));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_downMotion = token;  // +0x5d4
            break;
        }

case 0xc4b:  // "[overturn motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overturn motion]", 3147));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_overturnMotion = token;  // +0x5d8
            break;
        }

case 0xc57:  // "[jump motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump motion]", 3159));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_jumpMotion = token;  // +0x5dc
            break;
        }

case 0xc63:  // "[jumpattack motion]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jumpattack motion]", 3171));
            /* FALLTHRU */
        }
        else
        {
            if (!ScanStr(&token) || !isValidExtension(token, ".ani"))
                return 0;
            script->m_jumpAttackMotion = token;  // +0x5e0
            break;
        }

case 0xc6f:  // "[attack motion]" → .ani 字符串列表
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack motion]", 3183));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_attackMotions.clear();
            bool isType = false;
            bool ended = false;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (isType)
                {
                    if (token == "[/attack motion]")
                    {
                        ended = true;
                        break;
                    }
                    return 0;
                }
                if (!isValidExtension(token, ".ani"))
                    return 0;
                script->m_attackMotions.push_back(token);
            }
            if (!ended)
                return 0;
        }
            break;
        }

case 0xc89:  // "[etc motion]" → .ani 字符串列表
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc motion]", 3209));
            /* FALLTHRU */
        }
        else
        {
        {
            script->m_etcMotions.clear();
            bool isType = false;
            bool ended = false;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (isType)
                {
                    if (token == "[/etc motion]")
                    {
                        ended = true;
                        break;
                    }
                    return 0;
                }
                if (!isValidExtension(token, ".ani"))
                    return 0;
                script->m_etcMotions.push_back(token);
            }
            if (!ended)
                return 0;
        }
            break;
        }

case 0xca3:  // "[waiting action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[waiting action]", 3235));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_waitingAction);  // +0x5fc
            break;
        }

case 0xca6:  // "[move action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[move action]", 3238));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_moveAction);  // +0x600
            break;
        }

case 0xca9:  // "[sit action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[sit action]", 3241));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_sitAction);  // +0x610
            break;
        }

case 0xcac:  // "[damage action 1]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage action 1]", 3244));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_damageAction[0]);  // +0x614
            break;
        }

case 0xcaf:  // "[damage action 2]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage action 2]", 3247));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_damageAction[1]);  // +0x618
            break;
        }

case 0xcb2:  // "[down action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[down action]", 3250));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_downAction);  // +0x61c
            break;
        }

case 0xcb5:  // "[overturn action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overturn action]", 3253));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_overturnAction);  // +0x620
            break;
        }

case 0xcb8:  // "[jump action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump action]", 3256));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_jumpAction);  // +0x624
            break;
        }

case 0xcbb:  // "[jumpattack action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jumpattack action]", 3259));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_jumpAttackAction);  // +0x628
            break;
        }

case 0xcbe:  // "[basic action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[basic action]", 3262));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_basicAction);  // +0x698
            break;
        }

case 0xcc1:  // "[attack action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack action]", 3265));
            /* FALLTHRU */
        }
        else
        {
        script->m_attackActions.clear();
        while (ScanStr(&token))
            script->m_attackActions.push_back(token);
            break;
        }

case 0xcc2:  // "[action list]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[action list]", 3266));
            /* FALLTHRU */
        }
        else
        {
        // ORIG 与 [attack action] 共用 +0x62c。
        script->m_attackActions.clear();
        while (ScanStr(&token))
            script->m_attackActions.push_back(token);
            break;
        }

case 0xcc8:  // "[etc action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc action]", 3272));
            /* FALLTHRU */
        }
        else
        {
        script->m_etcActions.clear();
        while (ScanStr(&token))
            script->m_etcActions.push_back(token);
            break;
        }

case 0xccd:  // "[last action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[last action]", 3277));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_lastAction);  // +0x644
            break;
        }

case 0xcd0:  // "[jumpattack info]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jumpattack info]", 3280));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_jumpAttackInfo);  // +0x6b4
            break;
        }

case 0xcd3:  // "[attack info]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack info]", 3283));
            /* FALLTHRU */
        }
        else
        {
        script->m_attackInfos.clear();
        while (ScanStr(&token))
            script->m_attackInfos.push_back(token);
            break;
        }

case 0xcd9:  // "[etc attack info]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc attack info]", 3289));
            /* FALLTHRU */
        }
        else
        {
        script->m_etcAttackInfos.clear();
        while (ScanStr(&token))
            script->m_etcAttackInfos.push_back(token);
            break;
        }

case 0xcdf:  // "[int data]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[int data]", 3295));
            /* FALLTHRU */
        }
        else
        {
        script->m_intData.clear();
        for (;;)
        {
            int value = ScanInt(&ok);
            if (!ok)
                break;
            script->m_intData.push_back(value);
        }
            break;
        }

case 0xcea:  // "[speech on situation]" → 111 个情境字符串桶
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[speech on situation]", 3306));
            /* FALLTHRU */
        }
        else
        {
        {
            for (int i = 0; i < 0x6f; ++i)
                script->m_field70c[i].clear();

            while (ScanStr(&token))
            {
                ScanStr(&token);
                const int index = ScanInt(static_cast<bool*>(0));
                int bucket = 0;
                int subId1;
                while (true)
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (__inited)
                    {
                        std::map<const char*, int, less_TCHAR>::const_iterator __it =
                            __lookup.find(token.c_str());
                        if (__it != __lookup.end())
                            subId1 = __it->second;
                        else
                            subId1 = -1;
                    }
                    else
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        subId1 = 0;
                    }
                    switch (subId1)
                    {
                    case 0:
                    case 3321:  // [on attack] → 当前行号桶（ORIG 08a17fca：-0xf8=-0x100）
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on attack]", 3321));
                            goto sub1_3324;
                        }
                        bucket = index;
                        break;
                    sub1_3324:
                    case 3324:  // [on damage]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on damage]", 3324));
                            goto sub1_3327;
                        }
                        bucket = 0x65;
                        break;
                    sub1_3327:
                    case 3327:  // [on die]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on die]", 3327));
                            goto sub1_3330;
                        }
                        bucket = 0x66;
                        break;
                    sub1_3330:
                    case 3330:  // [on revenge]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on revenge]", 3330));
                            goto sub1_3333;
                        }
                        bucket = 0x67;
                        break;
                    sub1_3333:
                    case 3333:  // [on etc]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on etc]", 3333));
                            goto sub1_3336;
                        }
                        bucket = 0x68;
                        break;
                    sub1_3336:
                    case 3336:  // [on appear]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on appear]", 3336));
                            goto sub1_3339;
                        }
                        bucket = 0x69;
                        break;
                    sub1_3339:
                    case 3339:  // [on disappear]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on disappear]", 3339));
                            goto sub1_3342;
                        }
                        bucket = 0x6a;
                        break;
                    sub1_3342:
                    case 3342:  // [on go to help]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on go to help]", 3342));
                            goto sub1_3345;
                        }
                        bucket = 0x6b;
                        break;
                    sub1_3345:
                    case 3345:  // [on stand]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on stand]", 3345));
                            goto sub1_3348;
                        }
                        bucket = 0x6c;
                        break;
                    sub1_3348:
                    case 3348:  // [on village]
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on village]", 3348));
                            goto end1;
                        }
                        bucket = 0x6e;
                        break;
                    default:
                        break;
                    }
                    end1:;
                if (!__inited)
                {
                    __init_lock.leave();
                    __inited = true;
                    continue;
                }
                break;
                }
                    // ORIG 08a183c6：m_field70c[bucket]（stride 0xc）.push_back(local_str)
                    script->m_field70c[bucket].push_back(token);
                }
            }
            break;
        }

case 0xd1d:  // "[fix move direction]"（ORIG 08a18438：ScanInt 非 0 → +0xc50 置 1）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fix move direction]", 3357));
            /* FALLTHRU */
        }
        else
        {
            if (ScanInt(static_cast<bool*>(0)) != 0)
                script->m_flagc50 = true;
            break;
        }

case 0xd22:  // "[throw attack]"（收集父 case；ORIG C LAB_08a184c3：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[throw attack]", 3362));
            /* FALLTHRU */
        }
        else
        {
            //  STThrowAttack 局部 + while(ScanStr 子标签行) __lookup_1(14 键 passive 字段表)
            //  分派 → 收尾 push_back m_throwAttacks +0x9c；字段槽映射 local_1d24+t 直证）
            //  ORIG 08a184c3..08a19076：增量链式建表（case 0 哨兵 → 每 leaf
            //  if(!__inited_1){insert 本键; jmp 下一 leaf} 链 → 尾 leave+置位重入）。
            //  与 STMapScript [background animation] 同族范式。
            {
                STThrowAttack elem;          // C1（ORIG -0x1d24）
                std::string fname;           // -0x1a88（filename 循环暂存）
                int subId2;                  // ORIG -0xec
                int i;                       // ORIG -0xe8（各 resize/读循环共用）
                while (true)
                {
                    static std::map<const char*, int, less_TCHAR> __lookup_1;
                    static SpinLock __init_lock_1;
                    static bool __inited_1 = false;
                    if (__inited_1)
                    {
                        std::map<const char*, int, less_TCHAR>::const_iterator __it =
                            __lookup_1.find(token.c_str());
                        if (__it != __lookup_1.end())
                            subId2 = __it->second;
                        else
                            subId2 = -1;
                    }
                    else
                    {
                        __init_lock_1.enter();
                        if (__inited_1)
                        {
                            __init_lock_1.leave();
                            continue;                        // ORIG 08a18725 nop;jmp 08a18544
                        }
                        subId2 = 0;                          // ORIG 08a18730 movl $0,-0xec
                    }
                    switch (subId2)
                    {
                    case 0:
                    case 3370:       // [passive object filename]：count(=m_field50) 条
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object filename]", 3370));
                            goto sub2_3377;                  // ORIG 08a18866 jmp 08a188b9
                        }
                        for (i = 0; i < elem.m_field50; ++i)
                        {
                            ScanStr(&fname);
                            elem.m_field00.push_back(fname);
                        }
                        break;
                    sub2_3377:
                    case 3377:       // [passive object number] → +0x50
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object number]", 3377));
                            goto sub2_3380;                  // ORIG 08a18914 jmp 08a1892d
                        }
                        elem.m_field50 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3380:
                    case 3380:       // [passive object index] → +0xc
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object index]", 3380));
                            goto sub2_3383;
                        }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3383:
                    case 3383:       // [power] → +0x10
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[power]", 3383));
                            goto sub2_3386;
                        }
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3386:
                    case 3386:       // [throw frame] → +0x14
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[throw frame]", 3386));
                            goto sub2_3389;
                        }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3389:
                    case 3389:       // [start x cood] → resize +0x18 后逐项 ScanInt
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object start x cood]", 3389));
                            goto sub2_3396;
                        }
                        elem.m_field18.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field18[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3396:
                    case 3396:       // [start y cood] → +0x24
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object start y cood]", 3396));
                            goto sub2_3403;
                        }
                        elem.m_field24.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field24[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3403:
                    case 3403:       // [start z cood] → +0x30
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object start z cood]", 3403));
                            goto sub2_3410;
                        }
                        elem.m_field30.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field30[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3410:
                    case 3410:       // [direction] → resize +0x58（ORIG avStack_1ccc 直证）
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[direction]", 3410));
                            goto sub2_3417;
                        }
                        elem.m_field58.resize(elem.m_field50);
                        for (i = 0; i < elem.m_field50; ++i)
                            elem.m_field58[i] = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3417:
                    case 3417:       // [attack number] → +0x3c
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack number]", 3417));
                            goto sub2_3420;
                        }
                        elem.m_field3c = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3420:
                    case 3420:       // [attack index] → +0x44
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3420));
                            goto sub2_3423;
                        }
                        elem.m_field44 = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3423:
                    case 3423:       // [throw sound] → ScanStr 直读 +0x40
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[throw sound]", 3423));
                            goto sub2_3426;
                        }
                        ScanStr(&elem.m_field40);
                        break;
                    sub2_3426:
                    case 3426:       // [object type] → +0x4c
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[object type]", 3426));
                            goto sub2_3429;
                        }
                        elem.m_field4c = ScanInt(static_cast<bool*>(0));
                        break;
                    sub2_3429:
                    case 3429:       // [start kind] → +0x54
                        if (!__inited_1)
                        {
                            __lookup_1.insert(std::map<const char*, int, less_TCHAR>::value_type("[start kind]", 3429));
                            goto sub2_end;                   // ORIG 末键 insert 直落尾
                        }
                        elem.m_field54 = ScanInt(static_cast<bool*>(0));
                        break;
                    default:
                        break;
                    }
                sub2_end:
                    if (!__inited_1)
                    {
                        __init_lock_1.leave();               // ORIG 08a1902a..31
                        __inited_1 = true;                   // ORIG 08a19036
                        continue;                            // ORIG 08a1903d jmp 08a18544
                    }
                    if (!ScanStr(&token))                    // ORIG 08a19043..53
                        break;
                }
                script->m_throwAttacks.push_back(elem);      // +0x9c ORIG 08a1906f
            }
            break;
        }

case 0xd6e:  // "[equipment]"（收集；ORIG C LAB_08a190cc：clear +0x390 → ScanInt 数量 →
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[equipment]", 3438));
            /* FALLTHRU */
        }
        else
        {
            //  count 次 ScanStr push m_items；末尾一次 ScanStr 丢弃）
            {
                script->m_equipment.clear();
                std::string item;
                const int count = ScanInt(static_cast<bool*>(0));
                for (int i = 0; i < count; ++i)
                {
                    ScanStr(&item);
                    script->m_equipment.m_items.push_back(item);
                }
                std::string tail;        // ORIG -0x1a94：读后即弃
                ScanStr(&tail);
            }
            break;
        }

case 0xd7b:  // "[ready action]"（收集父 case；ORIG C LAB_08a1921f：STMosterAttackReady 局部
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ready action]", 3451));
            /* FALLTHRU */
        }
        else
        {
            //  + while(ScanStr) 子标签 operator== 直判（无子表）→ push +0x39c）
            {
                STMosterAttackReady elem;
                std::string tmp;                         // -0x1ab4
                while (ScanStr(&token))
                {
                    if (token == "[motion index]")       // ORIG +0xc
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                    else if (token == "[effect ani file & layer index]")
                    {
                        const int n = ScanInt(static_cast<bool*>(0));
                        elem.m_field10.reserve(n);       // 先 string 向量（C 顺序）
                        elem.m_field00.reserve(n);
                        for (int i = 0; i < n; ++i)
                        {
                            ScanStr(&tmp);
                            elem.m_field10.push_back(tmp);
                            const int v = ScanInt(static_cast<bool*>(0));
                            elem.m_field00.push_back(v);
                        }
                    }
                }
                script->m_attackReadies.push_back(elem);   // +0x39c
            }
            break;
        }

case 0xd96:  // "[counter attack]"（收集父 case；ORIG asm 08a19436 / C 直译：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[counter attack]", 3478));
            /* FALLTHRU */
        }
        else
        {
            //  块首 ScanInt 元素数 → reserve → for(count){ while(ScanStr) 内 __lookup_3(4 键)
            //  字段分派 } → push +0xa8）
            {
                STCounterAttack elem;                    // -0x1acc
                const int count = ScanInt(static_cast<bool*>(0));
                script->m_counterAttacks.reserve(count);
                for (int k = 0; k < count; ++k)
                {
                    while (ScanStr(&token))
                    {
                        int subId3;
                        while (true)
                        {
                            static std::map<const char*, int, less_TCHAR> __lookup;
                            static SpinLock __init_lock;
                            static bool __inited = false;
                            if (__inited)
                            {
                                std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                    __lookup.find(token.c_str());
                                if (__it != __lookup.end())
                                    subId3 = __it->second;
                                else
                                    subId3 = -1;
                            }
                            else
                            {
                                __init_lock.enter();
                                if (__inited)
                                {
                                    __init_lock.leave();
                                    continue;
                                }
                                subId3 = 0;
                            }
                            switch (subId3)
                            {
                            case 0:
                            case 3489:  // [probability] → +0x00
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[probability]", 3489));
                                    goto sub3_3492;
                                }
                            elem.m_field00 = ScanInt(static_cast<bool*>(0));
                            break;
                            sub3_3492:
                            case 3492:  // [attack index] → +0x04
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3492));
                                    goto sub3_3495;
                                }
                            elem.m_field04 = ScanInt(static_cast<bool*>(0));
                            break;
                            sub3_3495:
                            case 3495:  // [damage type] → 三串比较映射 0/1/2 → +0x08
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage type]", 3495));
                                    goto sub3_3511;
                                }
                        {
                            std::string kind;            // -0x1ad4（块内构造/析构）
                            ScanStr(&kind);
                            if (kind == "[approach attack]")
                                elem.m_field08 = 0;
                            else if (kind == "[range attack]")
                                elem.m_field08 = 1;
                            else if (kind == "[all attack]")
                                elem.m_field08 = 2;
                        }
                            break;
                            sub3_3511:
                            case 3511:  // [need hp] → +0x10
                                if (!__inited)
                                {
                                    __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[need hp]", 3511));
                                    goto end3;
                                }
                            elem.m_field10 = ScanInt(static_cast<bool*>(0));
                            break;
                            default:
                                break;
                            }
                            end3:;
                        if (!__inited)
                        {
                            __init_lock.leave();
                            __inited = true;
                            continue;
                        }
                        break;
                        }
                    }
                    script->m_counterAttacks.push_back(elem);   // +0xa8
                }
            }
            break;
        }

case 0xdc0:  // "[jump attack]"（收集父 case；ORIG asm 08a19a00：STJumpAttack C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump attack]", 3520));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr 子标签行) __lookup_4(11 键) 字段分派 → push +0xb4）
            {
                STJumpAttack elem;                       // -0x1d24
                while (ScanStr(&token))
                {
                    int subId4;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId4 = __it->second;
                            else
                                subId4 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId4 = 0;
                        }
                        switch (subId4)
                        {
                        case 0:
                        case 3526:  // [attack index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3526));
                                goto sub4_3529;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3529:
                        case 3529:  // [start frame index] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[start frame index]", 3529));
                                goto sub4_3532;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3532:
                        case 3532:  // [end frame index] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[end frame index]", 3532));
                                goto sub4_3535;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3535:
                        case 3535:  // [jump height] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump height]", 3535));
                                goto sub4_3538;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3538:
                        case 3538:  // [jump speed] → +0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump speed]", 3538));
                                goto sub4_3541;
                            }
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3541:
                        case 3541:  // [on collision custom animation index] → +0x0c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on collision custom animation index]", 3541));
                                goto sub4_3544;
                            }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3544:
                        case 3544:  // [ememy distance rate] → +0x10（ScanFloat，P79 float 域）
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ememy distance rate]", 3544));
                                goto sub4_3547;
                            }
                        elem.m_field10 = ScanFloat(static_cast<bool*>(0));
                        break;
                        sub4_3547:
                        case 3547:  // [limited x cood] → +0x24
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limited x cood]", 3547));
                                goto sub4_3550;
                            }
                        elem.m_field24 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3550:
                        case 3550:  // [limited y cood] → +0x28
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limited y cood]", 3550));
                                goto sub4_3553;
                            }
                        elem.m_field28 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3553:
                        case 3553:  // [jump type] → +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump type]", 3553));
                                goto sub4_3556;
                            }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub4_3556:
                        case 3556:  // [jump direction] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[jump direction]", 3556));
                                goto end4;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end4:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_jumpAttacks.push_back(elem);   // +0xb4
            }
            break;
        }

case 0xdec:  // "[summon]"（收集父 case；ORIG asm 08a1a262：STSummon C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon]", 3564));
            /* FALLTHRU */
        }
        else
        {
            //  STSummonInfo 复用槽(-0x1ae8) + while(ScanStr) __lookup_5(9 键) 分派 → push +0xc0）
            {
                STSummon elem;                           // -0x1d24
                STSummonInfo info;                       // -0x1ae8（每轮四 int 覆写后 push）
                while (ScanStr(&token))
                {
                    int subId5;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId5 = __it->second;
                            else
                                subId5 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId5 = 0;
                        }
                        switch (subId5)
                        {
                        case 0:
                        case 3572:  // [function index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3572));
                                goto sub5_3575;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3575:
                        case 3575:  // [attack index] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3575));
                                goto sub5_3578;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3578:
                        case 3578:  // [pos random] → +0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[pos random]", 3578));
                                goto sub5_3581;
                            }
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3581:
                        case 3581:  // [monster kind] → +0x0c 数量 + infos reserve/loop
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[monster kind]", 3581));
                                goto sub5_3593;
                            }
                    {
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        elem.m_infos.reserve(elem.m_field0c);
                        for (int i = 0; i < elem.m_field0c; ++i)
                        {
                            info.m_field00 = ScanInt(static_cast<bool*>(0));
                            info.m_field04 = ScanInt(static_cast<bool*>(0));
                            info.m_field08 = ScanInt(static_cast<bool*>(0));
                            info.m_field0c = ScanInt(static_cast<bool*>(0));
                            elem.m_infos.push_back(info);
                        }
                    }
                        break;
                        sub5_3593:
                        case 3593:  // [summon term] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon term]", 3593));
                                goto sub5_3596;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3596:
                        case 3596:  // [probability] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[probability]", 3596));
                                goto sub5_3599;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3599:
                        case 3599:  // [enterance effect] → +0x24
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[enterance effect]", 3599));
                                goto sub5_3602;
                            }
                        elem.m_field24 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3602:
                        case 3602:  // [max number] → +0x28
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[max number]", 3602));
                                goto sub5_3605;
                            }
                        elem.m_field28 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub5_3605:
                        case 3605:  // [max frequency] → +0x2c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[max frequency]", 3605));
                                goto end5;
                            }
                        elem.m_field2c = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end5:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_summons.push_back(elem);   // +0xc0
            }
            break;
        }

case 0xe1d:  // "[dash attack]"（收集父 case；ORIG asm 08a1aaa6：STDash C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash attack]", 3613));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_6(8 键) 分派 → push +0xcc）
            {
                STDash elem;                             // -0x1d24
                while (ScanStr(&token))
                {
                    int subId6;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId6 = __it->second;
                            else
                                subId6 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId6 = 0;
                        }
                        switch (subId6)
                        {
                        case 0:
                        case 3620:  // [function index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3620));
                                goto sub6_3623;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3623:
                        case 3623:  // [attack index] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3623));
                                goto sub6_3626;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3626:
                        case 3626:  // [shakeable] → 连续三 int +0x08/+0x0c/+0x10
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[shakeable]", 3626));
                                goto sub6_3631;
                            }
                    {
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                    }
                        break;
                        sub6_3631:
                        case 3631:  // [first speed] → +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[first speed]", 3631));
                                goto sub6_3634;
                            }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3634:
                        case 3634:  // [last speed] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[last speed]", 3634));
                                goto sub6_3637;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3637:
                        case 3637:  // [dash time] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash time]", 3637));
                                goto sub6_3640;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3640:
                        case 3640:  // [dash ready time] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash ready time]", 3640));
                                goto sub6_3643;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub6_3643:
                        case 3643:  // [animation first index] → +0x24
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[animation first index]", 3643));
                                goto end6;
                            }
                        elem.m_field24 = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end6:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_dashes.push_back(elem);   // +0xcc
            }
            break;
        }

case 0xe43:  // "[hold attack]"（收集父 case；ORIG asm 08a1b1af：STHold C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hold attack]", 3651));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_7(6 键) 分派 → push +0xd8）
            {
                STHold elem;                             // -0x1d24
                while (ScanStr(&token))
                {
                    int subId7;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId7 = __it->second;
                            else
                                subId7 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId7 = 0;
                        }
                        switch (subId7)
                        {
                        case 0:
                        case 3658:  // [function index] → +0x00；==1 时连读 +0x04/+0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3658));
                                goto sub7_3666;
                            }
                    {
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        if (elem.m_field00 == 1)
                        {
                            elem.m_field04 = ScanInt(static_cast<bool*>(0));
                            elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        }
                    }
                        break;
                        sub7_3666:
                        case 3666:  // [attack index] → +0x0c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3666));
                                goto sub7_3669;
                            }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3669:
                        case 3669:  // [offset] → +0x10 与 +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[offset]", 3669));
                                goto sub7_3673;
                            }
                        // ORIG 0x8a1b5e9 body：ScanInt→-0x1d10、ScanInt→-0x1d0c（STHold C1 基座 -0x1d20）
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3673:
                        case 3673:  // [animation index] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[animation index]", 3673));
                                goto sub7_3676;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3676:
                        case 3676:  // [min hold] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[min hold]", 3676));
                                goto sub7_3679;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub7_3679:
                        case 3679:  // [max hold] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[max hold]", 3679));
                                goto end7;
                            }
                        elem.m_field20 = ScanInt(static_cast<bool*>(0));
                        break;
                        default:
                            break;
                        }
                        end7:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_holds.push_back(elem);   // +0xd8
            }
            break;
        }

case 0xe67:  // "[warp attack]"（收集父 case；ORIG asm 08a1b7c6：STWarp C1(-0x1d24) +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warp attack]", 3687));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_8(9 键) 分派 → push +0xe4；
            //  [category of target]：null→-2 / all→-1 / 其余 getCategoryToEnumValue）
            {
                STWarp elem;                             // -0x1d24
                while (ScanStr(&token))
                {
                    int subId8;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId8 = __it->second;
                            else
                                subId8 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId8 = 0;
                        }
                        switch (subId8)
                        {
                        case 0:
                        case 3694:  // [function index] → +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[function index]", 3694));
                                goto sub8_3697;
                            }
                        elem.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3697:
                        case 3697:  // [attack index] → +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack index]", 3697));
                                goto sub8_3700;
                            }
                        elem.m_field04 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3700:
                        case 3700:  // [warp style] → +0x08
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warp style]", 3700));
                                goto sub8_3703;
                            }
                        elem.m_field08 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3703:
                        case 3703:  // [after attack] → +0x0c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[after attack]", 3703));
                                goto sub8_3706;
                            }
                        elem.m_field0c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3706:
                        case 3706:  // [after appear time] → +0x10
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[after appear time]", 3706));
                                goto sub8_3709;
                            }
                        elem.m_field10 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3709:
                        case 3709:  // [cool time] → +0x14
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[cool time]", 3709));
                                goto sub8_3712;
                            }
                        elem.m_field14 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3712:
                        case 3712:  // [target offset] → +0x18
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[target offset]", 3712));
                                goto sub8_3715;
                            }
                        elem.m_field18 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3715:
                        case 3715:  // [passive object index] → +0x1c
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[passive object index]", 3715));
                                goto sub8_3718;
                            }
                        elem.m_field1c = ScanInt(static_cast<bool*>(0));
                        break;
                        sub8_3718:
                        case 3718:  // [category of target] → +0x20
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[category of target]", 3718));
                                goto end8;
                            }
                    {
                        std::string cat;         // -0x1afc（块内构造/析构）
                        ScanStr(&cat);
                        if (cat == "null")
                            elem.m_field20 = -2;
                        else if (cat == "all")
                            elem.m_field20 = -1;
                        else
                            elem.m_field20 = getCategoryToEnumValue(cat);
                    }
                        break;
                        default:
                            break;
                        }
                        end8:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                }
                script->m_warps.push_back(elem);   // +0xe4
            }
            break;
        }

case 0xe9e:  // "[event super on scr]"（收集父 case；ORIG asm 08a1bfcf：STEventSuper C1 +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[event super on scr]", 3742));
            /* FALLTHRU */
        }
        else
        {
            //  while(ScanStr) __lookup_9(8 键触发模式表) 分派 → push +0xfc）
            {
                STEventSuper elem;                       // -0x1d24（每元素复用槽）
                while (ScanStr(&token))
                {
                    int subId9;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token.c_str());
                            if (__it != __lookup.end())
                                subId9 = __it->second;
                            else
                                subId9 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId9 = 0;
                        }
                        switch (subId9)
                        {
                        case 0:
                        case 3749:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on time]", 3749));
                                goto sub9_3755;
                            }
                    {
                        elem.m_field00 = 0;
                        elem.m_field08 = ScanInt(&ok);   // +8
                        elem.m_field0c = ScanInt(&ok);   // +0xc
                        elem.m_field10 = ScanInt(&ok) != 0;   // +0x10 byte（CONCAT 低位）
                    }
                        break;
                        sub9_3755:
                        case 3755:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[on hp]", 3755));
                                goto sub9_3761;
                            }
                    {
                        elem.m_field00 = 1;
                        elem.m_field08 = ScanInt(&ok);
                        elem.m_field0c = ScanInt(&ok);
                        elem.m_field10 = ScanInt(&ok) != 0;
                    }
                        break;
                        sub9_3761:
                        case 3761:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[always]", 3761));
                                goto sub9_3767;
                            }
                    {
                        elem.m_field00 = 2;
                        elem.m_field08 = ScanInt(&ok);
                        elem.m_field10 = ScanInt(&ok) != 0;
                    }
                        break;
                        sub9_3767:
                        case 3767:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[counter]", 3767));
                                goto sub9_3773;
                            }
                    {
                        elem.m_field00 = 3;
                        elem.m_field08 = ScanInt(&ok);
                        elem.m_field0c = ScanInt(&ok);
                        elem.m_field10 = ScanInt(&ok) != 0;
                    }
                        break;
                        sub9_3773:
                        case 3773:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[breakable]", 3773));
                                goto sub9_3780;
                            }
                    {
                        elem.m_field04 = 0;
                        elem.m_field14 = ScanInt(&ok);   // +0x14（local_1d10）
                        elem.m_field18 = ScanInt(&ok);   // +0x18
                        elem.m_field1c = ScanInt(&ok);   // +0x1c
                        elem.m_field20 = ScanInt(&ok);   // +0x20
                    }
                        break;
                        sub9_3780:
                        case 3780:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[half]", 3780));
                                goto sub9_3787;
                            }
                    {
                        elem.m_field04 = 1;
                        elem.m_field14 = ScanInt(&ok);
                        elem.m_field18 = ScanInt(&ok);
                        elem.m_field1c = ScanInt(&ok);
                        elem.m_field20 = ScanInt(&ok);
                    }
                        break;
                        sub9_3787:
                        case 3787:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[unbreakable]", 3787));
                                goto sub9_3794;
                            }
                    {
                        elem.m_field04 = 2;
                        elem.m_field14 = ScanInt(&ok);
                        elem.m_field18 = ScanInt(&ok);
                        elem.m_field1c = ScanInt(&ok);
                        elem.m_field20 = ScanInt(&ok);
                    }
                        break;
                        sub9_3794:
                        case 3794:
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[with blast]", 3794));
                                goto end9;
                            }
                        elem.m_field24 = 1;   // +0x24 byte（CONCAT 置 1）
                        break;
                        default:
                            break;
                        }
                        end9:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                    // ORIG：未识别引导无字段分支，循环继续
                }
                script->m_eventSupers.push_back(elem);   // +0xfc
            }
            break;
        }

case 0xeda:  // "[atk super on scr]"（收集父 case；ORIG asm 08a1c823 / C 段：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[atk super on scr]", 3802));
            /* FALLTHRU */
        }
        else
        {
            //  每元素行 = <m_field04值> `[breakable|half|unbreakable]` 4×int → push +0x108）
            {
                STAtkSuper elem;   // C1（asm 08a1c883 每元素复用槽）
                while (true)
                {
                    int v = ScanInt(&ok);
                    if (!ok)
                        break;   // asm 08a1c8b1：首 ScanInt 失败退出收集
                    elem.m_field04 = v;   // 存 +4（asm mov -0x1b10）
                    if (ScanStr(&token))
                    {
                        if (token == "[breakable]")
                            elem.m_field00 = 0xd;
                        else if (token == "[half]")
                            elem.m_field00 = 0xe;
                        else if (token == "[unbreakable]")
                            elem.m_field00 = 0xf;
                    }
                    elem.m_field08 = ScanInt(&ok);
                    elem.m_field0c = ScanInt(&ok);
                    elem.m_field10 = ScanInt(&ok);
                    elem.m_field14 = ScanInt(&ok);
                    script->m_atkSupers.push_back(elem);   // +0x108
                }
            }
            break;
        }

case 0xefb:  // "[cooltime]"（收集父 case；ORIG asm 08a1c99f / C 5530：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[cooltime]", 3835));
            /* FALLTHRU */
        }
        else
        {
            //  ScanTypeOrInt 循环：数字→m_field04；"[Pattern]"→m_field00=ScanInt；
            //  "[/cooltime]" 结束 → push +0x114）
            {
                STCoolTime elem;   // C1
                for (;;)
                {
                    bool isType = false;
                    bool isInt = false;
                    int v = ScanTypeOrInt(isType, token, isInt, true);
                    if (!isInt)
                        break;   // 读尽/EOF
                    if (isType)
                    {
                        if (token == "[Pattern]")
                            elem.m_field00 = ScanInt(&ok);
                        else if (token == "[/cooltime]")
                            break;
                    }
                    else
                        elem.m_field04.push_back(v);
                }
                script->m_coolTimes.push_back(elem);   // +0x114
            }
            break;
        }

case 0xf23:  // "[spawn prob]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[spawn prob]", 3875));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc4c = ScanInt(&ok);  // +0xc4c
            break;
        }

case 0xf27:  // "[drop item offset]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[drop item offset]", 3879));
            /* FALLTHRU */
        }
        else
        {
            script->m_field36c = ScanInt(&ok);  // +0x36c
            script->m_field370 = ScanInt(&ok);  // +0x370
            break;
        }

case 0xf2b:  // "[is grabable]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[is grabable]", 3883));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag368 = ScanInt(&ok) != 0;  // +0x368
            break;
        }

case 0xf2e:  // "[ai pattern]"（ORIG 叶为空；importAIInfoScript 仅在 0xf3e 调 1 次）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ai pattern]", 3886));
            /* FALLTHRU */
        }
        else
        {
            break;
        }

case 0xf2f:  // "[think term]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[think term]", 3887));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf30:  // "[destination change term]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[destination change term]", 3888));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf31:  // "[targeting bonus]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[targeting bonus]", 3889));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf32:  // "[keep distance with target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep distance with target]", 3890));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf33:  // "[warlike]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[warlike]", 3891));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf34:  // "[vision]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[vision]", 3892));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf35:  // "[intelligence]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[intelligence]", 3893));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf36:  // "[keep range distance with target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target]", 3894));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf37:  // "[back vision]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[back vision]", 3895));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf38:  // "[no change target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[no change target]", 3896));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf39:  // duplicate unnumbered keep-range key in ORIG map
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target]", 3897));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3a:  // "[keep range distance with target 1]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 1]", 3898));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3b:  // "[keep range distance with target 2]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 2]", 3899));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3c:  // "[keep range distance with target 3]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 3]", 3900));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3d:  // "[keep range distance with target 4]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 4]", 3901));
            /* FALLTHRU */
        }
        else
        {
            break;
        }
case 0xf3e:  // "[keep range distance with target 5]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[keep range distance with target 5]", 3902));
            /* FALLTHRU */
        }
        else
        {
            importAIInfoScript(&script->m_aiInfo, &tag, true);
            break;
        }

case 0xf46:  // "[reflection object]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[reflection object]", 3910));
            /* FALLTHRU */
        }
        else
        {
        script->m_fieldc8c.clear();
        script->m_fieldc80.clear();
        for (;;)
        {
            int first = ScanInt(&ok);
            if (!ok)
                break;
            int second = ScanInt(static_cast<bool*>(0));
            script->m_fieldc8c.push_back(first);
            script->m_fieldc80.push_back(second);
        }
            break;
        }

case 0xf57:  // "[set damage check]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[set damage check]", 3927));
            /* FALLTHRU */
        }
        else
        {
            script->m_field380 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0xf5c:  // "[change direction by target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[change direction by target]", 3932));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag33c = ScanInt(&ok) != 0;  // +0x33c（asm movb %dl 实证）
            break;
        }

case 0xf62:  // "[is direction to move]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[is direction to move]", 3938));
            /* FALLTHRU */
        }
        else
        {
            script->m_field37c = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0xf67:  // "[absolute damage resistance rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[absolute damage resistance rate]", 3943));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc54 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0xf6c:  // "[hp regen rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[hp regen rate]", 3948));
            /* FALLTHRU */
        }
        else
        {
            script->m_field540 = ScanFloat(static_cast<bool*>(0));
            script->m_field544 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0xf72:  // "[absolute defense ignore rate]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[absolute defense ignore rate]", 3954));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc58 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0xf76:  // "[lie frame]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[lie frame]", 3958));
            /* FALLTHRU */
        }
        else
        {
        for (int i = 0; i < 5; ++i)
            script->m_fieldc5c[i] = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0xf7b:  // "[forced knockback]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[forced knockback]", 3963));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1cac = ScanInt(&ok);  // +0x1cac
            break;
        }

case 0xf80:  // "[No Combo Rate Monster]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[No Combo Rate Monster]", 3968));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1ca8 = true;
            break;
        }

case 0xf84:  // "[proc action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[proc action]", 3972));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_procAction);  // +0x648
            break;
        }

case 0xf87:  // "[monster title]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[monster title]", 3975));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_monsterTitle);  // +0x694
            break;
        }

case 0xf8a:  // "[destroy count]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[destroy count]", 3978));
            /* FALLTHRU */
        }
        else
        {
            script->m_field6f4 = ScanInt(&ok);  // +0x6f4
            break;
        }

case 0xf8d:  // "[destroy count damage]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[destroy count damage]", 3981));
            /* FALLTHRU */
        }
        else
        {
            script->m_field6f8 = ScanInt(&ok);  // +0x6f8
            break;
        }

case 0xf99:  // "[overhead gauge type]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overhead gauge type]", 3993));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc74 = ScanInt(&ok);  // +0xc74
            break;
        }

case 0xf9d:  // "[overhead gauge pos]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[overhead gauge pos]", 3997));
            /* FALLTHRU */
        }
        else
        {
            script->m_fieldc78 = ScanInt(&ok);  // +0xc78
            script->m_fieldc7c = ScanInt(&ok);  // +0xc7c
            break;
        }

case 0xfa2:  // "[flying mark]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[flying mark]", 4002));
            /* FALLTHRU */
        }
        else
        {
        {
            const int value = ScanInt(static_cast<bool*>(0));
            script->m_flagc70 = true;
            script->m_flagc71 = (value == 1);
        }
            break;
        }

case 0xfa7:  // "[moveback action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[moveback action]", 4007));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_movebackAction);  // +0x604
            break;
        }

case 0xfaa:  // "[dash action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dash action]", 4010));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_dashAction);  // +0x608
            break;
        }

case 0xfad:  // "[dashattack action]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dashattack action]", 4013));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_dashattackAction);  // +0x60c
            break;
        }

case 0xfb0:  // "[burning death path]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[burning death path]", 4016));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_field098);  // +0x98
            break;
        }

case 0xfb3:  // "[initial target]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[initial target]", 4019));
            /* FALLTHRU */
        }
        else
        {
        {
            STInitialTarget target;
            for (;;)
            {
                std::string sub;
                if (!ScanType(sub, true))
                    break;
                if (sub == "[/initial target]")
                    break;
                if (sub == "[initial target Permanent]")
                {
                    target.m_field0c =
                        (ScanInt(static_cast<bool*>(0)) == 1);
                }
                else
                {
                    target.m_field04 = sub;
                    target.m_field00 = ScanInt(static_cast<bool*>(0));
                }
            }
            script->m_initialTargets.push_back(target);
        }
            break;
        }

case 0xfde:  // "[level of difficulty]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[level of difficulty]", 4062));
            /* FALLTHRU */
        }
        else
        {
        {
            STDifficulty difficulty;
            bool isType = false;
            while (ScanTypeOrStr(isType, token, true))
            {
                if (!isType)
                    continue;
                if (token == "[/level of difficulty]")
                    break;
                // ORIG 08a1e041/084/0c7/107/147：五个 tag 各为**独立分支**
                // （每分支 2×ScanInt(0) 写 -0x1b5c/-0x1b58），汇合于 08a1e185 的
                // push_back；合并条件只发 2 次 ScanInt（p167 差集 −8 的根因）
                if (token == "[easy]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[medium]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[hard]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[ultimate]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                else if (token == "[hero]")
                {
                    difficulty.m_field00 = ScanInt(static_cast<bool*>(0));
                    difficulty.m_field04 = ScanInt(static_cast<bool*>(0));
                }
                script->m_difficulties.push_back(difficulty);
            }
        }
            break;
        }

case 0x1020:  // "[fire attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fire attack]", 4128));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[0][0] = ScanInt(&ok);  // +0x508
            script->m_paramTable[0][1] = ScanInt(&ok);  // +0x50c
            break;
        }

case 0x1024:  // "[water attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[water attack]", 4132));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[1][0] = ScanInt(&ok);  // +0x510
            script->m_paramTable[1][1] = ScanInt(&ok);  // +0x514
            break;
        }

case 0x1028:  // "[dark attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[dark attack]", 4136));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[2][0] = ScanInt(&ok);  // +0x518
            script->m_paramTable[2][1] = ScanInt(&ok);  // +0x51c
            break;
        }

case 0x102c:  // "[light attack]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[light attack]", 4140));
            /* FALLTHRU */
        }
        else
        {
            script->m_paramTable[3][0] = ScanInt(&ok);  // +0x520
            script->m_paramTable[3][1] = ScanInt(&ok);  // +0x524
            break;
        }

case 0x1030:  // "[rigidity]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[rigidity]", 4144));
            /* FALLTHRU */
        }
        else
        {
            script->m_field528[0] = ScanInt(&ok);  // +0x528
            script->m_field528[1] = ScanInt(&ok);  // +0x52c
            break;
        }

case 0x1035:  // "[drop avatar item]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[drop avatar item]", 4149));
            /* FALLTHRU */
        }
        else
        {
        {
            for (int i = 0; i < 4; ++i)
            {
                float rate = ScanFloat(&ok);
                if (!ok)
                    return 0;
                script->m_dropAvatarInfo.m_field08[i] = rate;   // [b28d-solo] 同型 float 直赋
            }
            for (int i = 4; i < 9; ++i)
            {
                float rate = ScanFloat(&ok);
                if (!ok)
                    return 0;
                script->m_dropAvatarInfo.m_field08[i] = rate;
            }
            script->m_dropAvatarInfo.m_field00 = ScanInt(&ok);
            if (!ok)
                return 0;
            script->m_dropAvatarInfo.m_field04 = ScanInt(&ok);
            if (!ok)
                return 0;
            for (;;)
            {
                stEnamyDropAvatarElement_t element;
                element.m_field0 = ScanInt(&ok);
                if (!ok)
                    break;
                element.m_field4 = ScanInt(&ok);
                if (!ok)
                    return 0;
                element.m_field8 = ScanInt(&ok);
                if (!ok)
                    return 0;
                script->m_dropAvatarInfo.m_field2c += element.m_field4;
                element.m_field4 = script->m_dropAvatarInfo.m_field2c;
                script->m_dropAvatarInfo.m_elements.push_back(element);
            }
        }
            break;
        }

case 0x1060:  // "[pattern]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[pattern]", 4192));
            /* FALLTHRU */
        }
        else
        {
        {
            std::vector<int> pattern;
            for (;;)
            {
                int value = ScanInt(&ok);
                if (!ok)
                    break;
                pattern.push_back(value);
            }
            if (!pattern.empty())
                script->m_patterns.push_back(pattern);
        }
            break;
        }

case 0x106d:  // "[fix direction]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fix direction]", 4205));
            /* FALLTHRU */
        }
        else
        {
        {
            bool isString = false;
            bool isInt = false;
            std::string direction;
            const int value = ScanStrOrInt(isString, direction, isInt, true);
            if (!isString)
            {
                script->m_flagcd4 = (value != 0);
            }
            else
            {
                script->m_flagcd4 = true;
                int fixDirKind = -1;
                for (;;)
                {
                    static std::map<const char*, int, less_TCHAR> __lookup;
                    static SpinLock __init_lock;
                    static bool __inited = false;
                    if (!__inited)
                    {
                        __init_lock.enter();
                        if (__inited)
                        {
                            __init_lock.leave();
                            continue;
                        }
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[left]", 4219));
                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[right]", 4222));
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    std::map<const char*, int, less_TCHAR>::const_iterator __it = __lookup.find(direction.c_str());
                    if (__it != __lookup.end())
                        fixDirKind = __it->second;
                    break;
                }
                // 好 .o f349/f366：left→+0xcd8=0；right→+0xcd8=1；无 else 分支
                if (fixDirKind == 4219)
                    script->m_fieldcd8 = 0;
                else if (fixDirKind == 4222)
                    script->m_fieldcd8 = 1;
            }
        }
            break;
        }

case 0x108b:  // "[ridableskill definition]"（收集父 case；ORIG asm 08a1ebdb：
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ridableskill definition]", 4235));
            /* FALLTHRU */
        }
        else
        {
            //  STRidableSkill sk(-0x1d24) + do{ do{ScanTypeOrStr}while(!isStr)
            //    __lookup_10(4 键) 分派；[skill]→string+__lookup_11(6 键 type 表) }while(!closed)
            //    push m_ridableSkills(+0x6f4)）
            {
                std::string token2;                    // local_1b90
                int closed = 0;                        // local_78
                STRidableSkill sk;                     // -0x1d24
                do
                {
                    bool isStr = false;
                    while (true)
                    {
                        if (!ScanTypeOrStr(isStr, token2, true))
                            // [b29-SweepSTMonsterScript2 真 bug] ORIG 0x8a1ec89
                            // `jne 8a1f765` 跳到「析构 sk/token2 + jmp case 尾」，
                            // **不 push**（push 在循环体内正常出口 0x8a1ef41，
                            // 随后 `movb $0x1,-0x6d`=closed=1 退出）。旧实现
                            // `goto rid_def_exit` 落在 push 之前 ⇒ 读失败也 push
                            // 半成品 sk（多一条 ridable skill）。
                            goto rid_def_fail;         // 读失败 → 丢弃（不 push）
                        if (isStr)
                            break;
                    }
                    bool doneLine = false;             // local_71
                    int subId10;
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                        if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(token2.c_str());
                            if (__it != __lookup.end())
                                subId10 = __it->second;
                            else
                                subId10 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;
                            }
                            subId10 = 0;
                        }
                        switch (subId10)
                        {
                        case 0:
                        case 4254:  // [/ridableskill definition]
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/ridableskill definition]", 4254));
                                goto sub10_4260;
                            }
                        doneLine = true;
                        break;
                        sub10_4260:
                        case 4260:  // [pattern] → sk +0x00
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[pattern]", 4260));
                                goto sub10_4263;
                            }
                        sk.m_field00 = ScanInt(static_cast<bool*>(0));
                        break;
                        sub10_4263:
                        case 4263:  // [image path] → sk +0x04
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[image path]", 4263));
                                goto sub10_4266;
                            }
                        ScanStr(&sk.m_name);
                        break;
                        sub10_4266:
                        case 4266:  // [skill]：8 组（2×int + type 词）
                            if (!__inited)
                            {
                                __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[skill]", 4266));
                                goto end10;
                            }
                    {
                        for (int i = 0; i < 8; ++i)
                        {
                            sk.m_field08[i] = ScanInt(static_cast<bool*>(0));
                            sk.m_fielda8[i] = ScanInt(static_cast<bool*>(0));
                            std::string ty;            // local_1b9c：clear 后读
                            ty.clear();
                            ScanStr(&ty);
                            sk.m_field88[i] = 6;
                            int subId11;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(ty.c_str());
                                    if (__it != __lookup.end())
                                        subId11 = __it->second;
                                    else
                                        subId11 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId11 = 0;
                                }
                                switch (subId11)
                                {
                                case 0:
                                case 4282:                                    // normal
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("normal", 4282));
                                        goto sub11_4287;
                                    }
                                    sk.m_field88[i] = 0;
                                    break;
                                sub11_4287:
                                case 4287:                                    // defaultcancel
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("defaultcancel", 4287));
                                        goto sub11_4292;
                                    }
                                    sk.m_field88[i] = 1;
                                    break;
                                sub11_4292:
                                case 4292:                                    // movable
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("movable", 4292));
                                        goto sub11_4297;
                                    }
                                    sk.m_field88[i] = 2;
                                    break;
                                sub11_4297:
                                case 4297:                                    // charge
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("charge", 4297));
                                        goto sub11_4302;
                                    }
                                    sk.m_field88[i] = 3;
                                    break;
                                sub11_4302:
                                case 4302:                                    // evasion
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("evasion", 4302));
                                        goto sub11_4307;
                                    }
                                    sk.m_field88[i] = 4;
                                    break;
                                sub11_4307:
                                case 4307:                                    // super
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("super", 4307));
                                        goto end11;
                                    }
                                    sk.m_field88[i] = 5;
                                    break;
                                default:
                                    break;
                                }
                            end11:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        // ORIG 0x8a1f5fc 公共尾：按刚写入的类型值附加读取——
                        //  v∈{0,1,2,4,5}(1<<v & 0x37)：n=ScanInt，随后 n 个 ScanInt
                        //    逐个 push 进 sk.m_levels[i]（+0x28+i*12 vector<int>）；
                        //  v==3(charge)：m_fieldc8[i]/m_fielde8[i] 各一 ScanInt；
                        //  v==6（未识别/默认）不读。
                        {
                            const int v = sk.m_field88[i];
                            if (v <= 5)
                            {
                                const unsigned bit = 1u << v;
                                if ((bit & 0x37u) != 0)
                                {
                                    const int n = ScanInt(static_cast<bool*>(0));
                                    for (int j = 0; j < n; ++j)
                                        sk.m_levels[i].push_back(ScanInt(static_cast<bool*>(0)));
                                }
                                else if ((bit & 8u) != 0)
                                {
                                    sk.m_fieldc8[i] = ScanInt(static_cast<bool*>(0));
                                    sk.m_fielde8[i] = ScanInt(static_cast<bool*>(0));
                                }
                            }
                        }
                        }
                    }
                        break;
                        default:
                            break;
                        }
                        end10:;
                    if (!__inited)
                    {
                        __init_lock.leave();
                        __inited = true;
                        continue;
                    }
                    break;
                    }
                    if (doneLine)
                        closed = 1;
                } while (closed == 0);
                script->m_ridableSkills.push_back(sk);
            rid_def_fail:;   // 读失败落点：跳过 push（ORIG 0x8a1f765 同）
            }
            break;
        }

case 0x1146:  // "[limit slowdown atkspeed]"（ORIG 08a1f7bc：ScanFloat → m_fieldcc0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limit slowdown atkspeed]", 4422));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1c94 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x1149:  // "[limit slowdown movespeed]"（ScanFloat → m_fieldcc4）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[limit slowdown movespeed]", 4425));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1c98 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x114c:  // "[ridableskill explanation]"（ORIG 08a1f8aa：STDifficulty 未用局部 +
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[ridableskill explanation]", 4428));
            /* FALLTHRU */
        }
        else
        {
            //  STRidableSkillExplain exp + ScanTypeOrStr 串词分派 → push +0xce8）
            {
                STDifficulty unused;                   // local_1ba8：ORIG 构造不使用
                (void)unused;
                std::string word;                      // local_1bac
                STRidableSkillExplain exp;             // -0x1bc0
                while (true)
                {
                    bool isStr = false;
                    if (!ScanTypeOrStr(isStr, word, true))
                        break;
                    if (!isStr)
                        continue;
                    if (word == "[/ridableskill explanation]")
                        break;
                    if (word == "[title]")
                        ScanStr(&exp.m_field00);
                    else if (word == "[subtitle]")
                        ScanStr(&exp.m_field04);
                    else if (word == "[category]")
                        ScanStr(&exp.m_field08);
                    else if (word == "[detail]")
                        ScanStr(&exp.m_field0c);
                    else if (word == "[pattern]")
                        exp.m_field10 = ScanInt(static_cast<bool*>(0));
                }
                script->m_ridableSkillExplains.push_back(exp);
            }
            break;
        }

case 0x1177:  // "[physical attack besed on diff]"（ORIG 08a1faf4：200×5 float 表 +0xcf4，
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[physical attack besed on diff]", 4471));
            /* FALLTHRU */
        }
        else
        {
            //  diff-1 定行、ScanFloat 失败或表满置 ended）
            {
                std::string unused;                    // local_1bc8：ORIG 构造不使用
                (void)unused;
                bool ended = false;
                for (int i = 0; i < 200 && !ended; ++i)
                {
                    const int diff = ScanInt(&ok);
                    if (!ok)
                    {
                        ended = true;
                        break;
                    }
                    for (int j = 0; j < 5; ++j)
                    {
                        const float v = ScanFloat(&ok);
                        script->m_difficultyScales[diff - 1][j] = v;
                        if (!ok)
                        {
                            ended = true;
                            break;
                        }
                    }
                }
            }
            break;
        }

case 0x1194:  // "[non targeted]"（ScanInt 非 0 → m_field56c byte 1/0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[non targeted]", 4500));
            /* FALLTHRU */
        }
        else
        {
            script->m_field56c = (ScanInt(static_cast<bool*>(0)) != 0);
            break;
        }

case 0x1197:  // "[target appendage]"（ScanStr 直读 string +0x570）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[target appendage]", 4503));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_field570);
            break;
        }

case 0x119a:  // "[damage time balance]"（4×ScanInt(&ok) push vector +0x554）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[damage time balance]", 4506));
            /* FALLTHRU */
        }
        else
        {
            for (int i = 0; i <= 3; ++i)          // ORIG 08a1fd87 cmp $3,setle
                script->m_field554.push_back(ScanInt(static_cast<bool*>(0)));
            break;
        }

case 0x11a0:  // "[stun time balance]"（4× push +0x560）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[stun time balance]", 4512));
            /* FALLTHRU */
        }
        else
        {
            for (int i = 0; i <= 3; ++i)          // ORIG 同族 i<=3 形
                script->m_field560.push_back(ScanInt(static_cast<bool*>(0)));
            break;
        }

case 0x11a7:  // "[add gravity in aerial]"（ORIG 08a1fe7c：bool +0x548 + float×2 +0x54c/550）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[add gravity in aerial]", 4519));
            /* FALLTHRU */
        }
        else
        {
            script->m_field548 = (ScanInt(static_cast<bool*>(0)) != 0);
            script->m_field54c = ScanFloat(static_cast<bool*>(0));
            script->m_field550 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0x11ae:  // "[summon skill status up type]"（ScanInt(0) → m_field3c0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[summon skill status up type]", 4526));
            /* FALLTHRU */
        }
        else
        {
            script->m_field3c0 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x11b1:  // "[attack down prob]"（ScanFloat(&ok) 循环写 m_attackMethods[i].+4）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[attack down prob]", 4529));
            /* FALLTHRU */
        }
        else
        {
            {
                int i = 0;
                while (true)
                {
                    const float v = ScanFloat(&ok);
                    if (!ok)
                        break;
                    script->m_attackMethods[i].m_field04 = v;
                    ++i;
                }
            }
            break;
        }

case 0x11c6:  // "[strong]"（ScanInt(0)→m_field1ca0；ScanFloat(0)→m_field1ca4）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[strong]", 4550));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1ca0 = ScanInt(static_cast<bool*>(0));
            script->m_field1ca4 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0x11ca:  // "[weak]"（取负写同槽 + float）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[weak]", 4554));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1ca0 = -ScanInt(static_cast<bool*>(0));
            script->m_field1ca4 = ScanFloat(static_cast<bool*>(0));
            break;
        }

case 0x11cf:  // "[explain]"（ScanStr 直读 string +0x6d0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[explain]", 4559));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&script->m_explain);
            break;
        }

case 0x11d4:  // "[consume mp]"（ORIG 08a201dc：ScanInt(&ok) 循环 push vector +0x120）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[consume mp]", 4564));
            /* FALLTHRU */
        }
        else
        {
            {
                int v = 0;
                while ((v = ScanInt(&ok), ok == true))
                    script->m_field120.push_back(v);
            }
            break;
        }

case 0x11e6:  // "[base table]"（ORIG 08a2027e：ScanStr 路径 → clear +0x12c →
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[base table]", 4582));
            /* FALLTHRU */
        }
        else
        {
            //  ImportMonsterBaseTable(script,c_str)；成功且 size()==0 → return 0）
            {
                std::string path2;                     // local_1bd8
                if (ScanStr(&path2) == true)
                {
                    script->m_baseParameters.clear();  // +0x12c
                    if (ImportMonsterBaseTable(script, path2.c_str()))
                    {
                        if (script->m_baseParameters.size() == 0)
                            return 0;
                    }
                    else
                        return 0;
                }
            }
            break;
        }

case 0x11ff:  // "[catch item]"（ORIG 08a203c8：+0x64 三字段暂存后按 CatchItemInfo
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[catch item]", 4607));
            /* FALLTHRU */
        }
        else
        {
            //  push 至 +0x70 vector；随后一次 ScanType 消耗尾词）
            {
                CatchItemInfo ci;
                ci.m_field0 = ScanInt(static_cast<bool*>(0));
                ci.m_field4 = ScanInt(static_cast<bool*>(0));
                ci.m_field8 = ScanFloat(static_cast<bool*>(0));
                std::string tail;                      // local_1bdc：ScanType 消耗
                ScanType(tail, true);
                script->m_catchItems.push_back(ci);
            }
            break;
        }

case 0x1213:  // "[fixed height]"（ScanInt(0) → m_field1cb0）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fixed height]", 4627));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1cb0 = ScanInt(static_cast<bool*>(0));
            break;
        }

case 0x1218:  // "[etc sound]"（ScanStr 成功则 m_field6b0 = 词）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[etc sound]", 4632));
            /* FALLTHRU */
        }
        else
        {
            {
                std::string snd;                       // local_1be0
                if (ScanStr(&snd))
                    script->m_etcSound = snd;
            }
            break;
        }

case 0x1220:  // "[can beat index]"（ORIG 08a20609：ScanTypeOrInt 循环 push +0x1cb4；
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[can beat index]", 4640));
            /* FALLTHRU */
        }
        else
        {
            //  非 int 词：仅 "[/can beat index]" 继续，否则整体 return 0）
            {
                bool isInt = false;
                bool isStr = false;
                std::string word;                      // local_1be8
                bool ended = false;
                while (true)
                {
                    const int v = ScanTypeOrInt(isStr, word, isInt, true);
                    if (!isInt)
                        break;
                    if (isStr)
                    {
                        if (word == "[/can beat index]")
                            ended = true;
                        break;
                    }
                    script->m_field1cb4.push_back(v);
                }
                if (!ended)
                    return 0;
            }
            break;
        }

case 0x1239:  // "[mounting action]"（ScanStr→token，赋 +0x64c）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[mounting action]", 4665));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);
            script->m_mountingAction = token;
            break;
        }

case 0x123f:  // "[mounted action]"（赋 +0x650）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[mounted action]", 4671));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);
            script->m_mountedAction = token;
            break;
        }

case 0x1245:  // "[unmounting action]"（赋 +0x654）
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[unmounting action]", 4677));
            /* FALLTHRU */
        }
        else
        {
            ScanStr(&token);
            script->m_unmountingAction = token;
            break;
        }

case 0x124b:  // "[control passive object]"（ORIG 08a208ff：do{ScanType→__lookup_12
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[control passive object]", 4683));
            /* FALLTHRU */
        }
        else
        {
            //  11 键；[option]→ScanStr+__lookup_13；[transparency]→词循环 __lookup_14/15；
            //  [fix direction]→ScanStr+__lookup_16；[/control passive object]→退出}
            //  while(ScanType 成功)）
            //  ORIG 08a2095f..08a22096：增量链式建表（case 0 哨兵 + 每 leaf
            //  if(!__inited_12){insert 本键; jmp 下 leaf}；建轮 continue 跳 guard
            //  08a209a9 而非 loop head 08a2096f）。
            {
                std::string tag2;                      // local_1bf0
                bool ended = false;                    // local_1bf5
                do
                {
                    ended = ScanType(tag2, true);
                    if (ended != true)
                        return 0;
                    int subId12;                       // ORIG -0x30
                    while (true)
                    {
                        static std::map<const char*, int, less_TCHAR> __lookup;
                        static SpinLock __init_lock;
                        static bool __inited = false;
                            if (__inited)
                        {
                            std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                __lookup.find(tag2.c_str());
                        if (__it != __lookup.end())
                            subId12 = __it->second;
                            else
                                subId12 = -1;
                        }
                        else
                        {
                            __init_lock.enter();
                            if (__inited)
                            {
                                __init_lock.leave();
                                continue;                // ORIG 08a20b8d jmp 08a209a9
                            }
                            subId12 = 0;                // ORIG 08a20b92
                        }
                        {
                    switch (subId12)
                    {
                    case 0:
                    case 4696:  // [index] → +0x658
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[index]", 4696));
                            goto sub12_4704;             // ORIG 08a20c86 链
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field658 = v;
                    }
                        break;
                    sub12_4704:
                    case 4704:  // [x pos] → +0x65c
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[x pos]", 4704));
                            goto sub12_4712;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field65c = v;
                    }
                        break;
                    sub12_4712:
                    case 4712:  // [y pos] → +0x660
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[y pos]", 4712));
                            goto sub12_4720;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field660 = v;
                    }
                        break;
                    sub12_4720:
                    case 4720:  // [width] → +0x664
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[width]", 4720));
                            goto sub12_4728;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field664 = v;
                    }
                        break;
                    sub12_4728:
                    case 4728:  // [height] → +0x668
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[height]", 4728));
                            goto sub12_4736;
                        }
                    {
                        const int v = ScanInt(&ok);
                        if (!ok) return 0;
                        script->m_field668 = v;
                    }
                        break;
                    sub12_4736:
                    case 4736:  // [velocity x] → float +0x66c
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[velocity x]", 4736));
                            goto sub12_4744;
                        }
                    {
                        const float f = ScanFloat(&ok);
                        if (!ok) return 0;
                        script->m_field66c = f;
                    }
                        break;
                    sub12_4744:
                    case 4744:  // [velocity y] → float +0x670
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[velocity y]", 4744));
                            goto sub12_4752;
                        }
                    {
                        const float f = ScanFloat(&ok);
                        if (!ok) return 0;
                        script->m_field670 = f;
                    }
                        break;
                    sub12_4752:
                    case 4752:  // [option] → ScanStr + convert y to z
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[option]", 4752));
                            goto sub12_4771;
                        }
                    {
                        std::string opt;               // local_1c00
                        bool okflag = false;
                        if (ScanStr(&opt))
                        {
                            int subId13;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(opt.c_str());
                                    if (__it != __lookup.end())
                                        subId13 = __it->second;
                                    else
                                        subId13 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId13 = 0;
                                }
                                switch (subId13)
                                {
                                case 0:
                                case 4760:      // convert y to z → +0x674 = 1
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("convert y to z", 4760));
                                        goto end13;
                                    }
                                    script->m_field674 = 1;
                                    okflag = true;
                                    break;
                                default:
                                    break;
                                }
                            end13:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        }
                        if (!okflag)
                            return 0;
                    }
                        break;
                    sub12_4771:
                    case 4771:  // [transparency] → 词循环
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[transparency]", 4771));
                            goto sub12_4823;
                        }
                    {
                        std::string word;              // local_1c08
                        bool done = false;
                        while (ScanType(word, true))
                        {
                            if (word == "[/transparency]")
                            {
                                done = true;
                                break;
                            }
                            int subId14;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(word.c_str());
                                    if (__it != __lookup.end())
                                        subId14 = __it->second;
                                    else
                                        subId14 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId14 = 0;
                                }
                                switch (subId14)
                                {
                                case 0:
                                case 4784:      // [master] → int +0x678
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[master]", 4784));
                                        goto sub14_4792;
                                    }
                                    {
                                        int mv;                    // ORIG -0x1bf0（ScanInt(int*) 直写）
                                        if (!ScanInt(&mv))
                                            return 0;
                                        script->m_field678 = mv;
                                    }
                                    break;
                                sub14_4792:
                                case 4792:      // [others] → int +0x67c
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[others]", 4792));
                                        goto sub14_4800;
                                    }
                                    {
                                        int mv;                    // ORIG -0x1bf0（ScanInt(int*) 直写）
                                        if (!ScanInt(&mv))
                                            return 0;
                                        script->m_field67c = mv;
                                    }
                                    break;
                                sub14_4800:
                                case 4800:      // [option] → ScanStr + inherit → +0x680
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[option]", 4800));
                                        goto end14;
                                    }
                                    {
                                        std::string opt2;      // local_1c10
                                        bool okflag = false;
                                        if (ScanStr(&opt2))
                                        {
                                            int subId15;
                                            while (true)
                                            {
                                                static std::map<const char*, int, less_TCHAR> __lookup;
                                                static SpinLock __init_lock;
                                                static bool __inited = false;
                                                if (__inited)
                                                {
                                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                                        __lookup.find(opt2.c_str());
                                                    if (__it != __lookup.end())
                                                        subId15 = __it->second;
                                                    else
                                                        subId15 = -1;
                                                }
                                                else
                                                {
                                                    __init_lock.enter();
                                                    if (__inited)
                                                    {
                                                        __init_lock.leave();
                                                        continue;
                                                    }
                                                    subId15 = 0;
                                                }
                                                switch (subId15)
                                                {
                                                case 0:
                                                case 4807:
                                                    if (!__inited)
                                                    {
                                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("inherit", 4807));
                                                        goto end15;
                                                    }
                                                    script->m_field680 = 1;
                                                    okflag = true;
                                                    break;
                                                default:
                                                    break;
                                                }
                                            end15:;
                                                if (!__inited)
                                                {
                                                    __init_lock.leave();
                                                    __inited = true;
                                                    continue;
                                                }
                                                break;
                                            }
                                        }
                                        if (!okflag)
                                            return 0;
                                    }
                                    break;
                                default:
                                    return 0;
                                }
                            end14:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        }
                        if (!done)
                            return 0;
                    }
                        break;
                    sub12_4823:
                    case 4823:  // [fix direction] → ScanStr + left/right
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[fix direction]", 4823));
                            goto sub12_4844;
                        }
                    {
                        std::string dir;               // local_1c18
                        bool okflag = false;
                        if (ScanStr(&dir))
                        {
                            int subId16;
                            while (true)
                            {
                                static std::map<const char*, int, less_TCHAR> __lookup;
                                static SpinLock __init_lock;
                                static bool __inited = false;
                                if (__inited)
                                {
                                    std::map<const char*, int, less_TCHAR>::const_iterator __it =
                                        __lookup.find(dir.c_str());
                                    if (__it != __lookup.end())
                                        subId16 = __it->second;
                                    else
                                        subId16 = -1;
                                }
                                else
                                {
                                    __init_lock.enter();
                                    if (__inited)
                                    {
                                        __init_lock.leave();
                                        continue;
                                    }
                                    subId16 = 0;
                                }
                                switch (subId16)
                                {
                                case 0:
                                case 4832:      // [left] → +0x684=0
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[left]", 4832));
                                        goto sub16_4836;
                                    }
                                    script->m_field684 = 0;
                                    okflag = true;
                                    break;
                                sub16_4836:
                                case 4836:      // [right] → +0x684=1
                                    if (!__inited)
                                    {
                                        __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[right]", 4836));
                                        goto end16;
                                    }
                                    script->m_field684 = 1;
                                    okflag = true;
                                    break;
                                default:
                                    break;
                                }
                            end16:;
                                if (!__inited)
                                {
                                    __init_lock.leave();
                                    __inited = true;
                                    continue;
                                }
                                break;
                            }
                        }
                        if (!okflag)
                            return 0;
                    }
                        break;
                    sub12_4844:
                    case 4844:  // [/control passive object] → 结束本 case
                        if (!__inited)
                        {
                            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[/control passive object]", 4844));
                            goto sub12_end;            // ORIG 08a2204e jmp 08a22057
                        }
                        ended = false;
                        break;
                    default:
                        break;
                    }
                    // ORIG：未命中键（含 -1）无动作，继续外层 do-while 读下一词
                        sub12_end:;
                        }
                        if (!__inited)
                        {
                            __init_lock.leave();       // ORIG 08a22065
                            __inited = true;           // ORIG 08a22071
                            continue;                  // ORIG 08a22078 jmp 08a209a9
                        }
                        break;
                    }
                } while (ended == true);
            }
            break;
        }

case 0x12f7:  // "[life max]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[life max]", 4855));
            /* FALLTHRU */
        }
        else
        {
            script->m_flag3b8 = ScanInt(&ok) != 0;
            break;
        }

case 0x12fb:  // "[start immune time]"
        if (!__inited)
        {
            __lookup.insert(std::map<const char*, int, less_TCHAR>::value_type("[start immune time]", 4859));
            /* FALLTHRU */
        }
        else
        {
            script->m_field1cc0 = ScanInt(&ok);
            if (!ok)
                return 0;
            break;
        }

            default:
                // ORIG fallback for the 17 ACTIVESTATUS_TAG entries:
                // each tag writes the low/high difficulty pair in
                // m_linearAbilities[0..16].
                for (int i = 0; i < 17; ++i)
                {
                    if (tag == kMonsterActiveStatusTags[i])
                    {
                        script->m_linearAbilities[i][0] =
                            ScanInt(static_cast<bool*>(0));
                        script->m_linearAbilities[i][1] =
                            ScanInt(static_cast<bool*>(0));
                        break;
                    }
                }
                break;
        }
        if (!__inited)
        {
            __init_lock.leave();
            __inited = true;
            continue;
        }
        break;
    }
        if (flag && nameSeen)
            break;
    }
    script->m_index = reinterpret_cast<STScriptFileList*>(
        &g_stMonsterScriptFileList)->FindIndexByFullScan(path);
    return 1;
