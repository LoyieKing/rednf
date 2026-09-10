bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra)
{
    static char saveFileName[0x100];                      // getScriptFileFullPath 缓冲
    getScriptFileFullPath(saveFileName, 0x100,
                          g_szMonsterScriptBaseDirectory, path);   // +0x2c
    FILE* file = fopen(saveFileName, "wb");               // +0x3a "wb"
    if (file == 0)
        return false;

    STMonsterScript defaultScript;                        // +0x5c 局部默认对象（Clear 全默认值）

    // ---- [name] +0x110：m_name != "" 时经 getTagByName(7,..) 导出 ----
    if (script->m_name != "")
    {
        const wchar_t* tag = 0;
        if (!script->m_altName.empty())
            tag = (const wchar_t*)script->m_altName.c_str();
        fprintf(file, "[name] %s\n",
                toTString(g_scriptStringManager_.getTagByName(
                    7, (const wchar_t*)script->m_name.c_str(),
                    tag, false)));
    }
    // ---- [face image] +0x5ac 非空 ----
    if (!script->m_faceImage.empty())
        fprintf(file, "[face image] `%s` %d\n\n",
                script->m_faceImage.c_str(), script->m_field5b0);
    // ---- [die effect]（默认对象比较）----
    if (script->m_dieEffect[0] != defaultScript.m_dieEffect[0] ||
        script->m_dieEffect[1] != defaultScript.m_dieEffect[1] ||
        script->m_dieEffect[2] != defaultScript.m_dieEffect[2])
        fprintf(file, "[die effect] %d %d %d\n",
                script->m_dieEffect[0], script->m_dieEffect[1],
                script->m_dieEffect[2]);
    // ---- [width] ----
    if (script->m_width != defaultScript.m_width ||
        script->m_height != defaultScript.m_height)
        fprintf(file, "[width] %d %d\n", script->m_width, script->m_height);
    // ---- [floating height] ----
    if (script->m_floatingHeight != defaultScript.m_floatingHeight)
        fprintf(file, "[floating height] %d\n", script->m_floatingHeight);
    // ---- [layer]（getDrawLayerName）----
    if (script->m_layer != defaultScript.m_layer)
        fprintf(file, "[layer] `%s`\n",
                getDrawLayerName((ENUM_DRAWLAYER)script->m_layer));
    fputc('\n', file);

    // ---- [category] +0x1c vector<int> 非空 ----
    if (!script->m_field01c.empty())
    {
        fwrite("[category]\n", 1, 0xb, file);
        for (unsigned int i = 0; i < script->m_field01c.size(); ++i)
        {
            ENUM_CATEGORY cat = (ENUM_CATEGORY)script->m_field01c[i];
            std::string str(getObjectCategoryString(cat));
            fprintf(file, "`%s`\n", str.c_str());
        }
        fwrite("[/category]\n\n", 1, 0xd, file);
    }
    // ---- 六声音段：ambient/attack/appear/damage/die 无条件，etc 非空 ----
    fprintf(file, "[ambient sound] `%s`\n", script->m_ambientSound.c_str());
    fprintf(file, "[attack sound] `%s`\n", script->m_attackSound.c_str());
    fprintf(file, "[appear sound] `%s`\n", script->m_appearSound.c_str());
    fprintf(file, "[damage sound] `%s`\n", script->m_damageSound.c_str());
    fprintf(file, "[die sound] `%s`\n", script->m_dieSound.c_str());
    if (!script->m_etcSound.empty())
        fprintf(file, "[etc sound] `%s`\n", script->m_etcSound.c_str());
    // ---- [ability category] +0x7c size != 0；内循环匹配 0x93f5640 表 14 类 ----
    if (script->m_parameterCategories.size() != 0)
    {
        static const char* const s_abilityCategoryName[14] = {
            "[ATTACK_SPEED]", "[MOVE_SPEED]", "[PHYSICAL_ATTACK]",
            "[PHYSICAL_DEFENSE]", "[MAGICAL_ATTACK]", "[MAGICAL_DEFENSE]",
            "[EQUIPMENT_PHYSICAL_ATTACK]", "[EQUIPMENT_PHYSICAL_DEFENSE]",
            "[EQUIPMENT_MAGICAL_ATTACK]", "[EQUIPMENT_MAGICAL_DEFENSE]",
            "[HP MAX]", "[HIT_RECOVERY]", "[STUCK]", "[HP MAX BOSS]"
        };
        fwrite("[ability category]\n", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_parameterCategories.size(); ++i)
        {
            const STMonsterParameterCategory* cat =
                &script->m_parameterCategories[i];
            for (int j = 0; j <= 13; ++j)
            {
                if (cat->m_field0 == j)
                {
                    fprintf(file, "`%s` `%s` %d\n",
                            s_abilityCategoryName[j],
                            cat->m_field4 ? "*" : "+",
                            cat->m_field8);
                    break;
                }
            }
        }
        fwrite("[/ability category]\n", 1, 0x14, file);
    }
    // ---- [ability table] ----
    if (script->m_abilityTable != defaultScript.m_abilityTable)
        fprintf(file, "[ability table] %d\n", script->m_abilityTable);
    // ---- [summon skill status up rate]：0..9 float 非零则输出（"%.2f " 累积）----
    {
        char rateBuf[0x1000];
        strncpy(rateBuf, "", 0x1000);
        bool anyRate = false;
        for (int i = 0; i <= 9; ++i)
        {
            if (script->m_field3c4[i] != 0.0f)
                anyRate = true;
            char tmp[0x40];
            snprintf(tmp, 0x40, "%.2f ", (double)script->m_field3c4[i]);
            strncat(rateBuf, tmp, 0x40);
        }
        if (anyRate)
            fprintf(file, "[summon skill status up rate]\n%s\n", rateBuf);
    }
    // ---- [level] 无条件 ----
    fprintf(file, "[level] %d %d\n",
            script->m_levelByDiff[0], script->m_levelByDiff[1]);
    // ---- 四系 resistance（任一非零）----
    // ORIG 08a22c9f 实证：条件两项均读 [k][0]（原源码笔误，忠实复现）
    if (script->m_resistTable[0][0] != 0 || script->m_resistTable[0][0] != 0)
        fprintf(file, "[fire resistance] %d %d\n",
                script->m_resistTable[0][0], script->m_resistTable[0][1]);
    if (script->m_resistTable[1][0] != 0 || script->m_resistTable[1][0] != 0)
        fprintf(file, "[water resistance] %d %d\n",
                script->m_resistTable[1][0], script->m_resistTable[1][1]);
    if (script->m_resistTable[2][0] != 0 || script->m_resistTable[2][0] != 0)
        fprintf(file, "[dark resistance] %d %d\n",
                script->m_resistTable[2][0], script->m_resistTable[2][1]);
    if (script->m_resistTable[3][0] != 0 || script->m_resistTable[3][0] != 0)
        fprintf(file, "[light resistance] %d %d\n",
                script->m_resistTable[3][0], script->m_resistTable[3][1]);
    // ---- 线性 ability 17 项（任一非零；名称表 0x93f5480）----
    {
        static const char* const s_linearAbilityName[17] = {
            "[slow resistance]", "[freeze resistance]", "[poison resistance]",
            "[stun resistance]", "[curse resistance]", "[blind resistance]",
            "[lightning resistance]", "[stone resistance]", "[sleep resistance]",
            "[burn resistance]", "[weapon break resistance]", "[bleeding resistance]",
            "[piercing resistance]", "[stuck resistance]", "[confuse resistance]",
            "[hold resistance]", "[armor break resistance]"
        };
        for (int i = 0; i <= 16; ++i)
        {
            if (script->m_linearAbilities[i][0] ||
                script->m_linearAbilities[i][1])
                fprintf(file, "%s %d %d\n", s_linearAbilityName[i],
                        script->m_linearAbilities[i][0],
                        script->m_linearAbilities[i][1]);
        }
    }
    // ---- [HP regen speed]..[jump power]：任一非零 ----
    if (script->m_hpRegenSpeed[0] || script->m_hpRegenSpeed[1])
        fprintf(file, "[HP regen speed] %d %d\n",
                script->m_hpRegenSpeed[0], script->m_hpRegenSpeed[1]);
    if (script->m_mpRegenSpeed[0] || script->m_mpRegenSpeed[1])
        fprintf(file, "[MP regen speed] %d %d\n",
                script->m_mpRegenSpeed[0], script->m_mpRegenSpeed[1]);
    if (script->m_moveSpeed[0] || script->m_moveSpeed[1])
        fprintf(file, "[move speed] %d %d\n",
                script->m_moveSpeed[0], script->m_moveSpeed[1]);
    if (script->m_attackSpeed[0] || script->m_attackSpeed[1])
        fprintf(file, "[attack speed] %d %d\n",
                script->m_attackSpeed[0], script->m_attackSpeed[1]);
    if (script->m_castSpeed[0] || script->m_castSpeed[1])
        fprintf(file, "[cast speed] %d %d\n",
                script->m_castSpeed[0], script->m_castSpeed[1]);
    if (script->m_hitRecovery[0] || script->m_hitRecovery[1])
        fprintf(file, "[hit recovery] %d %d\n",
                script->m_hitRecovery[0], script->m_hitRecovery[1]);
    if (script->m_jumpPower[0] || script->m_jumpPower[1])
        fprintf(file, "[jump power] %d %d\n",
                script->m_jumpPower[0], script->m_jumpPower[1]);
    // ---- [jump speed]：默认 100 比较 ----
    if (script->m_jumpSpeed[0] != 100 || script->m_jumpSpeed[1] != 100)
        fprintf(file, "[jump speed] %d %d\n",
                script->m_jumpSpeed[0], script->m_jumpSpeed[1]);
    // ---- [normal dash speed]/[slant dash speed]/[weight]（默认对象比较）----
    if (script->m_normalDashSpeed[0] != defaultScript.m_normalDashSpeed[0] ||
        script->m_normalDashSpeed[1] != defaultScript.m_normalDashSpeed[1])
        fprintf(file, "[normal dash speed] %d %d\n",
                script->m_normalDashSpeed[0], script->m_normalDashSpeed[1]);
    if (script->m_slantDashSpeed[0] != defaultScript.m_slantDashSpeed[0] ||
        script->m_slantDashSpeed[1] != defaultScript.m_slantDashSpeed[1])
        // ORIG 08a2312f 实证：条件判 slant，**打印却是 normalDash 两字段**（原笔误忠实）
        fprintf(file, "[slant dash speed] %d %d\n",
                script->m_normalDashSpeed[0], script->m_normalDashSpeed[1]);
    if (script->m_weight[0] != defaultScript.m_weight[0] ||
        script->m_weight[1] != defaultScript.m_weight[1])
        fprintf(file, "[weight] %d %d\n",
                script->m_weight[0], script->m_weight[1]);
    fputc('\n', file);
    // ---- [superarmor on attack] ----
    if (script->m_superarmorOnAttack != defaultScript.m_superarmorOnAttack)
        fprintf(file, "[superarmor on attack] %d %d\n",
                (int)script->m_superarmorOnAttack, script->m_field5a8);
    // ---- [sight] ----
    if (script->m_sight != defaultScript.m_sight)
        fprintf(file, "[sight] %d\n", script->m_sight);
    // ---- [come in action ex]/[come in action] +0x530 非空 ----
    if (!script->m_comeInAction.empty())
    {
        if (!script->m_comeInActionEx.empty())
            fprintf(file, "[come in action ex] `%s` `%s` %d %d\n",
                    script->m_comeInAction.c_str(),
                    script->m_comeInActionEx.c_str(),
                    script->m_field538, script->m_field53c);
        else
            fprintf(file, "[come in action] `%s` %d %d\n",
                    script->m_comeInAction.c_str(),
                    script->m_field538, script->m_field53c);
    }
    // ---- targeting 系列 ----
    if (script->m_targetingTimeTerm != defaultScript.m_targetingTimeTerm)
        fprintf(file, "[targeting time term] %d\n",
                script->m_targetingTimeTerm);
    if (script->m_targetingOnAttack != defaultScript.m_targetingOnAttack)
        fprintf(file, "[targeting on attack] %d\n",
                script->m_targetingOnAttack ? 1 : 0);
    if (script->m_targetingOnDamage != defaultScript.m_targetingOnDamage)
        fprintf(file, "[targeting on damage] %d\n",
                script->m_targetingOnDamage ? 1 : 0);
    if (script->m_targetingLevel == 2)
        fwrite("[targeting high level] 1\n", 1, 0x19, file);
    else if (script->m_targetingLevel == 1)
        fwrite("[targeting low level] 1\n", 1, 0x18, file);
    if (script->m_targetingHp == 2)
        fwrite("[targeting high HP] 1\n", 1, 0x16, file);
    else if (script->m_targetingHp == 1)
        fwrite("[targeting low HP] 1\n", 1, 0x15, file);
    if (script->m_targetingNearest != defaultScript.m_targetingNearest)
        fprintf(file, "[targeting nearest] %d\n",
                script->m_targetingNearest ? 1 : 0);
    if (script->m_targetingAttacker != defaultScript.m_targetingAttacker)
        fprintf(file, "[targeting attacker] %d\n",
                script->m_targetingAttacker ? 1 : 0);
    if (script->m_targetingFromDirection !=
        defaultScript.m_targetingFromDirection)
        fprintf(file, "[targeting from direction] %d\n",
                script->m_targetingFromDirection ? 1 : 0);
    if (script->m_targetingLowPhysAttack !=
        defaultScript.m_targetingLowPhysAttack)
        fprintf(file, "[targeting low physical attack] %d\n",
                script->m_targetingLowPhysAttack ? 1 : 0);
    if (script->m_targetingLowPhysDefense !=
        defaultScript.m_targetingLowPhysDefense)
        fprintf(file, "[targeting low physical defense] %d\n",
                script->m_targetingLowPhysDefense ? 1 : 0);
    if (script->m_targetingLowMagAttack !=
        defaultScript.m_targetingLowMagAttack)
        fprintf(file, "[targeting low magical attack] %d\n",
                script->m_targetingLowMagAttack ? 1 : 0);
    if (script->m_targetingLowMagDefense !=
        defaultScript.m_targetingLowMagDefense)
        fprintf(file, "[targeting low magical defense] %d\n",
                script->m_targetingLowMagDefense ? 1 : 0);
    if (script->m_targetingDowned != defaultScript.m_targetingDowned)
        fprintf(file, "[targeting downed] %d\n",
                script->m_targetingDowned ? 1 : 0);
    if (script->m_warlike != defaultScript.m_warlike)
        fprintf(file, "[warlike] %d\n", script->m_warlike);
    if (script->m_attackDelay != defaultScript.m_attackDelay)
        fprintf(file, "[attack delay] %d\n", script->m_attackDelay);
    // ---- [stuckbonus on damage] 无条件 ----
    fprintf(file, "[stuckbonus on damage] %d %d %d %d\n",
            script->m_stuckBonusOnDamage[0][0], script->m_stuckBonusOnDamage[0][1],
            script->m_stuckBonusOnDamage[1][0], script->m_stuckBonusOnDamage[1][1]);
    fputc('\n', file);
    // ---- [item] 四段（pair<int,int> 循环）+ champion element + super 第二段 ----
    if (!script->m_field028[0].empty())
    {
        fwrite("[item]\n", 1, 7, file);
        for (unsigned int i = 0; i < script->m_field028[0].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[0][i].first,
                    script->m_field028[0][i].second);
        fwrite("[/item]\n\n", 1, 9, file);
    }
    if (!script->m_field028[1].empty())
    {
        fwrite("[common champion drop item]\n", 1, 0x1c, file);
        for (unsigned int i = 0; i < script->m_field028[1].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[1][i].first,
                    script->m_field028[1][i].second);
        fwrite("[/common champion drop item]\n\n", 1, 0x1e, file);
    }
    if (script->m_field028[2].size() != 0)
    {
        fwrite("[super champion drop item]\n", 1, 0x1b, file);
        for (unsigned int i = 0; i < script->m_field028[2].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[2][i].first,
                    script->m_field028[2][i].second);
        fwrite("[/super champion drop item]\n\n", 1, 0x1d, file);
    }
    if (!script->m_field058.empty())
    {
        fwrite("[death tower item]\n", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_field058.size(); ++i)
            fprintf(file, "%d %d\n", script->m_field058[i].first,
                    script->m_field058[i].second);
        fwrite("[/death tower item]\n", 1, 0x14, file);
    }
    if (!script->m_championElements.empty())
    {
        fwrite("[common champion elemental property]\n", 1, 0x25, file);
        for (unsigned int i = 0; i < script->m_championElements.size(); ++i)
            fprintf(file, "`%s`\n",
                    getElementName((ENUM_ELEMENT)script->m_championElements[i]));
        fwrite("[/common champion elemental property]\n\n", 1, 0x27, file);
    }
    if (!script->m_field028[2].empty())   // ORIG 同一 +0x40 段重复导出（忠实）
    {
        fwrite("[super champion drop item]\n", 1, 0x1b, file);
        for (unsigned int i = 0; i < script->m_field028[2].size(); ++i)
            fprintf(file, "%d %d\n", script->m_field028[2][i].first,
                    script->m_field028[2][i].second);
        fwrite("[/super champion drop item]\n\n", 1, 0x1d, file);
    }
    // ---- [keep distance] ----
    if (script->m_keepDistance != defaultScript.m_keepDistance)
        fprintf(file, "[keep distance] %d\n", script->m_keepDistance);
    // ---- [is on the object] 两处 ----
    if (script->m_flag374)
        fprintf(file, "[is on the object] 1 %d\n", script->m_field378);
    if (script->m_isOnTheObject)
        fwrite("[is on the object] 1\n", 1, 0x15, file);
    // ---- [attack kind] +0x6e8 vector<STAttackMethod> ----
    if (!script->m_attackMethods.empty())
    {
        fwrite("[attack kind]\n", 1, 0xe, file);
        for (unsigned int i = 0; i < script->m_attackMethods.size(); ++i)
        {
            // ORIG 逐字段重做下标（0x2c,0x30,0x24,0x28,0x1c,0x20,0x14,0x18,
            // 0x00(三目),0x10,0x0c,0x08 求值序）；%f 三值经 flds→fstpl 双精度。
            fprintf(file,
                    "%f\t%f\t%f\t%d %d %d %d %d %d %d %d %d",
                    (double)script->m_attackMethods[i].m_field08,
                    (double)script->m_attackMethods[i].m_field0c,
                    (double)script->m_attackMethods[i].m_field10,
                    script->m_attackMethods[i].m_field00 ? 1 : 0,
                    script->m_attackMethods[i].m_field18,
                    script->m_attackMethods[i].m_field14,
                    script->m_attackMethods[i].m_field20,
                    script->m_attackMethods[i].m_field1c,
                    script->m_attackMethods[i].m_field28,
                    script->m_attackMethods[i].m_field24,
                    script->m_attackMethods[i].m_field30,
                    script->m_attackMethods[i].m_field2c);
            fwrite("\t\n", 1, 2, file);
        }
        fwrite("[/attack kind]\n\n", 1, 0x10, file);
    }
    // ---- motion 系列（!= "" 比较）----
    if (script->m_waitingMotion != "")
        fprintf(file, "[waiting motion] `%s`\n",
                script->m_waitingMotion.c_str());
    if (script->m_moveMotion != "")
        fprintf(file, "[move motion] `%s`\n", script->m_moveMotion.c_str());
    if (script->m_sitMotion != "")
        fprintf(file, "[sit motion] `%s`\n", script->m_sitMotion.c_str());
    if (script->m_damageMotion[0] != "")
        fprintf(file, "[damage motion 1] `%s`\n",
                script->m_damageMotion[0].c_str());
    if (script->m_damageMotion[1] != "")
        fprintf(file, "[damage motion 2] `%s`\n",
                script->m_damageMotion[1].c_str());
    if (script->m_downMotion != "")
        fprintf(file, "[down motion] `%s`\n", script->m_downMotion.c_str());
    if (script->m_overturnMotion != "")
        fprintf(file, "[overturn motion] `%s`\n",
                script->m_overturnMotion.c_str());
    if (script->m_jumpMotion != "")
        fprintf(file, "[jump motion] `%s`\n", script->m_jumpMotion.c_str());
    if (script->m_jumpAttackMotion != "")
        fprintf(file, "[jumpattack motion] `%s`\n",
                script->m_jumpAttackMotion.c_str());
    // ---- [attack motion] +0x5e4 ----
    if (!script->m_attackMotions.empty())
    {
        fwrite("[attack motion]\n", 1, 0x10, file);
        for (std::vector<std::string>::iterator it =
                 script->m_attackMotions.begin();
             it != script->m_attackMotions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack motion]\n", 1, 0x11, file);
    }
    // ---- [etc motion] +0x5f0 ----
    if (!script->m_etcMotions.empty())
    {
        fwrite("[etc motion]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it =
                 script->m_etcMotions.begin();
             it != script->m_etcMotions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc motion]\n", 1, 0xe, file);
    }
    fputc('\n', file);
    // ---- action 系列 ----
    if (script->m_waitingAction != "")
        fprintf(file, "[waiting action] `%s`\n",
                script->m_waitingAction.c_str());
    if (script->m_moveAction != "")
        fprintf(file, "[move action] `%s`\n", script->m_moveAction.c_str());
    if (!script->m_movebackAction.empty())
        fprintf(file, "[moveback action] `%s`\n",
                script->m_movebackAction.c_str());
    if (!script->m_dashAction.empty())
        fprintf(file, "[dash action] `%s`\n", script->m_dashAction.c_str());
    if (!script->m_dashattackAction.empty())
        fprintf(file, "[dashattack action] `%s`\n",
                script->m_dashattackAction.c_str());
    if (script->m_sitAction != "")
        fprintf(file, "[sit action] `%s`\n", script->m_sitAction.c_str());
    if (script->m_damageAction[0] != "")
        fprintf(file, "[damage action 1] `%s`\n",
                script->m_damageAction[0].c_str());
    if (script->m_damageAction[1] != "")
        fprintf(file, "[damage action 2] `%s`\n",
                script->m_damageAction[1].c_str());
    if (script->m_downAction != "")
        fprintf(file, "[down action] `%s`\n", script->m_downAction.c_str());
    if (script->m_overturnAction != "")
        fprintf(file, "[overturn action] `%s`\n",
                script->m_overturnAction.c_str());
    if (script->m_jumpAction != "")
        fprintf(file, "[jump action] `%s`\n", script->m_jumpAction.c_str());
    if (script->m_jumpAttackAction != "")
        fprintf(file, "[jumpattack action] `%s`\n",
                script->m_jumpAttackAction.c_str());
    if (script->m_basicAction != "")
        fprintf(file, "[basic action] `%s`\n", script->m_basicAction.c_str());
    // ---- [attack action] +0x62c（extra map 同 key 向量配对输出）----
    if (!script->m_attackActions.empty())
    {
        std::vector<std::string> extraActions;
        std::map<std::string, std::vector<std::string> >::iterator it =
            extra.find("[attack action]");
        if (it != extra.end())
            extraActions = it->second;
        fwrite("[attack action]\n", 1, 0x10, file);
        for (unsigned int i = 0; i < script->m_attackActions.size(); ++i)
        {
            std::string s;
            s.clear();
            if (i < extraActions.size())
                s = extraActions[i];
            fprintf(file, "`%s`\t%s\n", script->m_attackActions[i].c_str(),
                    s.c_str());
        }
        fwrite("[/attack action]\n", 1, 0x11, file);
    }
    // ---- [etc action] +0x638 ----
    if (!script->m_etcActions.empty())
    {
        fwrite("[etc action]\n", 1, 0xd, file);
        for (std::vector<std::string>::iterator it =
                 script->m_etcActions.begin();
             it != script->m_etcActions.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc action]\n", 1, 0xe, file);
    }
    if (script->m_lastAction != "")
        fprintf(file, "[last action] `%s`\n", script->m_lastAction.c_str());
    if (!script->m_procAction.empty())
        fprintf(file, "[proc action] `%s`\n", script->m_procAction.c_str());
    if (script->m_jumpAttackInfo != "")
        fprintf(file, "[jumpattack info] `%s`\n",
                script->m_jumpAttackInfo.c_str());
    // ---- [attack info] +0x6b8 / [etc attack info] +0x6c4 ----
    if (!script->m_attackInfos.empty())
    {
        fwrite("[attack info]\n", 1, 0xe, file);
        for (std::vector<std::string>::iterator it =
                 script->m_attackInfos.begin();
             it != script->m_attackInfos.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/attack info]\n", 1, 0xf, file);
    }
    if (!script->m_etcAttackInfos.empty())
    {
        fwrite("[etc attack info]\n", 1, 0x12, file);
        for (std::vector<std::string>::iterator it =
                 script->m_etcAttackInfos.begin();
             it != script->m_etcAttackInfos.end(); it++)
            fprintf(file, "`%s`\n", (*it).c_str());
        fwrite("[/etc attack info]\n", 1, 0x13, file);
    }
    fputc('\n', file);
    // ---- [int data] +0x6d4（extra map "[int data]" 配对）----
    if (!script->m_intData.empty())
    {
        fwrite("[int data]\n", 1, 0xb, file);
        std::string s;
        for (unsigned int i = 0; i < script->m_intData.size(); ++i)
        {
            // ORIG 每轮：clear → 临时 key find → 命中则拷贝整个 vector 再取元素
            s.clear();
            std::map<std::string, std::vector<std::string> >::iterator it =
                extra.find("[int data]");
            if (it != extra.end())
            {
                std::vector<std::string> tmp(it->second);
                if (i < tmp.size())
                    s = tmp[i];
            }
            fprintf(file, "%d\t%s\n", script->m_intData[i], s.c_str());
        }
        fwrite("[/int data]\n", 1, 0xc, file);
    }
    // ---- [speech on situation] +0x70c[111]（0..0x64 用 [on attack]:+序号；0x65..0x6e switch）----
    // ORIG speechTag(-0x19c) 为**函数作用域**：ctor 在此处、dtor 在函数尾
    // （08a29e4c），故不得用 {} 包裹（否则块尾 dtor + 独立 landing pad）。
    bool anySpeech = false;
    std::string speechTag;
    int speechIndex = 0;
    for (int si = 0; si <= 0x6e; ++si)
    {
        if (!script->m_field70c[si].empty())
        {
            fwrite("[speech on situation]\n", 1, 0x16, file);
            anySpeech = true;
            break;
        }
    }
    for (int si = 0; si <= 0x6e; ++si)
    {
        if (script->m_field70c[si].size() != 0)
        {
            if (0 <= si && si <= 100)       // ORIG 08a25313：js/jg → 后段 switch 块
            {
                speechTag = "`[on attack]:`";
                speechIndex = si;
                for (unsigned int j = 0; j < script->m_field70c[si].size(); ++j)
                {
                    fprintf(file, "%s %s %d\n", speechTag.c_str(),
                            toTString(g_scriptStringManager_.getTagByName(
                                7,
                                (const wchar_t*)script->m_field70c[si][j].c_str(),
                                0, false)),
                            speechIndex);
                }
            }
            else
            {
                switch (si - 0x65)
                {
                case 0: speechTag = "`[on damage]`"; break;
                case 1: speechTag = "`[on die]`"; break;
                case 2: speechTag = "`[on revenge]`"; break;
                case 3: speechTag = "`[on etc]`"; break;
                case 4: speechTag = "`[on appear]`"; break;
                case 5: speechTag = "`[on disappear]`"; break;
                case 6: speechTag = "`[on go to help]`"; break;
                case 7: speechTag = "`[on stand]`"; break;
                // case 8（si==0x6d）：ORIG 表项直指 merge（保留上一轮 tag）
                case 9: speechTag = "`[on village]`"; break;
                }
                speechIndex = 0;            // ORIG 08a25533：switch merge 后
                for (unsigned int j = 0; j < script->m_field70c[si].size(); ++j)
                {
                    fprintf(file, "%s %s %d\n", speechTag.c_str(),
                            toTString(g_scriptStringManager_.getTagByName(
                                7,
                                (const wchar_t*)script->m_field70c[si][j].c_str(),
                                0, false)),
                            speechIndex);
                }
            }
        }
    }
    if (anySpeech)
        fwrite("[/speech on situation]\n", 1, 0x17, file);
    // ---- [fix move direction] ----
    if (script->m_flagc50)
        fwrite("[fix move direction] 1\n", 1, 0x17, file);
    // ---- [throw attack] +0x9c ----
    for (unsigned int i = 0; i < script->m_throwAttacks.size(); ++i)
    {
        fwrite("[throw attack]\n", 1, 0xf, file);
        fwrite("`[attack index]`", 1, 0x10, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field44);
        fwrite("`[passive object number]`", 1, 0x19, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field50);
        fwrite("`[passive object filename]`", 1, 0x1b, file);
        for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
            fprintf(file, "`%s`\n",
                    script->m_throwAttacks[i].m_field00[j].c_str());
        fwrite("`[object type]`", 1, 0xf, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field4c);
        fwrite("`[passive object index]`", 1, 0x18, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field0c);
        fwrite("`[power]`", 1, 9, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field10);
        fwrite("`[throw frame]`", 1, 0xf, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field14);
        if (!script->m_throwAttacks[i].m_field18.empty())
        {
            fwrite("`[passive object start x cood]` ", 1, 0x20, file);
            for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
                fprintf(file, "%d ", script->m_throwAttacks[i].m_field18[j]);
            fputc('\n', file);
        }
        if (!script->m_throwAttacks[i].m_field24.empty())
        {
            fwrite("`[passive object start y cood]` ", 1, 0x20, file);
            for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
                fprintf(file, "%d ", script->m_throwAttacks[i].m_field24[j]);
            fputc('\n', file);
        }
        if (!script->m_throwAttacks[i].m_field30.empty())
        {
            fwrite("`[passive object start z cood]` ", 1, 0x20, file);
            for (int j = 0; j < script->m_throwAttacks[i].m_field50; ++j)
                fprintf(file, "%d ", script->m_throwAttacks[i].m_field30[j]);
            fputc('\n', file);
        }
        fwrite("`[attack number]`", 1, 0x11, file);
        fprintf(file, " %d\n", script->m_throwAttacks[i].m_field3c);
        fwrite("`[throw sound]`", 1, 0xf, file);
        fprintf(file, "`%s`\n", script->m_throwAttacks[i].m_field40.c_str());
        fwrite("[/throw attack]\n", 1, 0x10, file);
    }
    // ---- [equipment] +0x390 ----
    if (!script->m_equipment.m_items.empty())
    {
        fwrite("[equipment]\n", 1, 0xc, file);
        fprintf(file, "%d\n", (int)script->m_equipment.m_items.size());
        for (unsigned int i = 0; i < script->m_equipment.m_items.size(); ++i)
            fprintf(file, "`%s`\n", script->m_equipment.m_items[i].c_str());
        fwrite("[/equipment]\n", 1, 0xd, file);
    }
    // ---- [ready action] +0x39c ----
    for (unsigned int i = 0; i < script->m_attackReadies.size(); ++i)
    {
        fwrite("[ready action]\n", 1, 0xf, file);
        fprintf(file, "`[motion index]` %d\n",
                script->m_attackReadies[i].m_field0c);
        fwrite("`[effect ani file & layer index]`\n", 1, 0x22, file);
        fprintf(file, "%d\n", (int)script->m_attackReadies[i].m_field10.size());
        for (unsigned int j = 0; j < script->m_attackReadies[i].m_field10.size(); ++j)
        {
            fprintf(file, "`%s` ",
                    script->m_attackReadies[i].m_field10[j].c_str());
            fprintf(file, "%d\n", script->m_attackReadies[i].m_field00[j]);
        }
        fwrite("[/ready action]\n", 1, 0x10, file);
    }
    // ---- [counter attack] +0xa8 ----
    for (unsigned int i = 0; i < script->m_counterAttacks.size(); ++i)
    {
        fwrite("[counter attack]\n", 1, 0x11, file);
        // ORIG 08a26107：结构体值拷贝进局部（10 条 mov/mov 对）
        const STCounterAttack ca = script->m_counterAttacks[i];
        fprintf(file, "%d\n", (int)script->m_counterAttacks.size());
        fprintf(file, "`[probability]` %d\n", ca.m_field00);
        fprintf(file, "`[attack index]` %d\n", ca.m_field04);
        fwrite("`[damage type]`", 1, 0xf, file);
        // ORIG 08a261c8 switch 线性决策（cmp1 je; cmp2 je; test jne; 0-body fall）
        switch (ca.m_field08)
        {
        case 0:
            fwrite("`[approach attack]`\n", 1, 0x14, file);
            break;
        case 1:
            fwrite("`[range attack]`\n", 1, 0x11, file);
            break;
        case 2:
            fwrite("`[all attack]`\n", 1, 0xf, file);
            break;
        }
        if (ca.m_field10 != 0)
            fprintf(file, "`[need hp]` %d\n", ca.m_field10);
        fwrite("[/counter attack]\n", 1, 0x12, file);
    }
    // ---- [jump attack] +0xb4 ----
    for (unsigned int i = 0; i < script->m_jumpAttacks.size(); ++i)
    {
        // ORIG 08a262ef：值拷贝（11 槽 -0x28c..-0x264）
        const STJumpAttack ja = script->m_jumpAttacks[i];
        fwrite("[jump attack]\n", 1, 0xe, file);
        fprintf(file, "`[attack index]` %d\n", ja.m_field00);
        fprintf(file, "`[start frame index]` %d\n", ja.m_field1c);
        fprintf(file, "`[end frame index]` %d\n", ja.m_field20);
        fprintf(file, "`[jump height]` %d\n", ja.m_field04);
        fprintf(file, "`[jump speed]` %d\n", ja.m_field08);
        fprintf(file, "`[on collision custom animation index]` %d\n",
                ja.m_field0c);
        fprintf(file, "`[ememy distance rate]` %f\n",
                (double)ja.m_field10);
        fprintf(file, "`[limited x cood]` %d\n", ja.m_field24);
        fprintf(file, "`[limited y cood]` %d\n", ja.m_field28);
        fprintf(file, "`[jump type]` %d\n", ja.m_field18);
        fprintf(file, "`[jump direction]` %d\n", ja.m_field14);
        fwrite("[/jump attack]\n", 1, 0xf, file);
    }
    // ---- [dash attack] +0xcc ----
    for (unsigned int i = 0; i < script->m_dashes.size(); ++i)
    {
        fwrite("[dash attack]\n", 1, 0xe, file);
        // ORIG 08a2657a：值拷贝（10 槽）
        const STDash d = script->m_dashes[i];
        fprintf(file, "`[function index]` %d\n", d.m_field00);
        fprintf(file, "`[attack index]` %d\n", d.m_field04);
        fprintf(file, "`[shakeable]` %d %d %d\n",
                d.m_field08, d.m_field0c, d.m_field10);
        fprintf(file, "`[first speed]` %d\n", d.m_field14);
        fprintf(file, "`[last speed]` %d\n", d.m_field18);
        fprintf(file, "`[dash time]` %d\n", d.m_field1c);
        fprintf(file, "`[dash ready time]` %d\n", d.m_field20);
        fprintf(file, "`[animation first index]` %d\n", d.m_field24);
        fwrite("[/dash attack]\n", 1, 0xf, file);
    }
    // ---- [hold attack] +0xd8 ----
    for (unsigned int i = 0; i < script->m_holds.size(); ++i)
    {
        fwrite("[hold attack]\n", 1, 0xe, file);
        // ORIG 08a26785：值拷贝（9 槽）
        const STHold h = script->m_holds[i];
        fprintf(file, "`[function index]` %d", h.m_field00);
        if (h.m_field00 == 1)
            fprintf(file, " %d %d", h.m_field08, h.m_field04);   // ORIG 08a267e6：先 +0x8 后 +0x4
        fputc('\n', file);
        fprintf(file, "`[attack index]` %d\n", h.m_field0c);
        fprintf(file, "`[offset]` %d %d\n", h.m_field10, h.m_field14);
        fprintf(file, "`[animation index]` %d\n", h.m_field18);
        fprintf(file, "`[min hold]` %d\n", h.m_field1c);
        fprintf(file, "`[max hold]` %d\n", h.m_field20);
        fwrite("[/hold attack]\n", 1, 0xf, file);
    }
    // ---- [warp attack] +0xe4（逐字段与默认对象比较）----
    for (unsigned int i = 0; i < script->m_warps.size(); ++i)
    {
        fwrite("[warp attack]\n", 1, 0xe, file);
        STWarp defaultWarp;                      // ORIG 循环体内 C1 → 后 ix
        const STWarp w = script->m_warps[i];     // ORIG 08a26996：值拷贝（9 槽）
        if (w.m_field00 != defaultWarp.m_field00)
            fprintf(file, "`[function index]` %d\n", w.m_field00);
        if (w.m_field04 != defaultWarp.m_field04)
            fprintf(file, "`[attack index]` %d\n", w.m_field04);
        if (w.m_field08 != defaultWarp.m_field08)
            fprintf(file, "`[warp style]` %d\n", w.m_field08);
        // ORIG 08a26xxx 实证字段映射：target offset=+0x18、after attack=+0x0c、
        // after appear time=+0x10、cool time=+0x14（按此输出序）
        if (w.m_field18 != defaultWarp.m_field18)
            fprintf(file, "`[target offset]` %d\n", w.m_field18);
        if (w.m_field0c != defaultWarp.m_field0c)
            fprintf(file, "`[after attack]` %d\n", w.m_field0c);
        if (w.m_field10 != defaultWarp.m_field10)
            fprintf(file, "`[after appear time]` %d\n", w.m_field10);
        if (w.m_field14 != defaultWarp.m_field14)
            fprintf(file, "`[cool time]` %d\n", w.m_field14);
        if (w.m_field1c != defaultWarp.m_field1c)
            fprintf(file, "`[passive object index]` %d\n", w.m_field1c);
        if (w.m_field20 != defaultWarp.m_field20)
        {
            fwrite("`[category of target]`", 1, 0x16, file);
            // ORIG 08a26ba0 switch(-2,null; -1,all; default,string)
            switch (w.m_field20)
            {
            case -2:
                fwrite(" `null`\n", 1, 8, file);
                break;
            case -1:
                fwrite(" `all`\n", 1, 7, file);
                break;
            default:
            {
                std::string cat(getObjectCategoryString(
                    (ENUM_CATEGORY)w.m_field20));
                fprintf(file, " `%s`\n", cat.c_str());
            }
            break;
            }
        }
        fwrite("[/warp attack]\n", 1, 0xf, file);
    }
    // ---- [event super on scr] +0xfc ----
    if (!script->m_eventSupers.empty())
    {
        for (unsigned int i = 0; i < script->m_eventSupers.size(); ++i)
        {
            fwrite("[event super on scr]\n", 1, 0x15, file);
            const STEventSuper es = script->m_eventSupers[i];  // ORIG 08a26d2e：值拷贝
            // ORIG 08a26d88：switch 二分决策树（根=1，je HP / jg{2,3} / else 0）
            switch (es.m_field00)
            {
            case 0:
                fprintf(file, "`[on time]` %d %d %d\n", es.m_field08,
                        es.m_field0c, es.m_field10);
                break;
            case 1:
                fprintf(file, "`[on hp]` %d %d %d\n", es.m_field08,
                        es.m_field0c, es.m_field10);
                break;
            case 2:
                fprintf(file, "`[always]` %d %d\n", es.m_field08,
                        es.m_field10);
                break;
            case 3:
                fprintf(file, "`[counter]` %d %d %d\n", es.m_field08,
                        es.m_field0c, es.m_field10);
                break;
            }
            // ORIG 08a26eb3 实证：%d×4 = f14、f18、f1c、f20（非 f24）
            // ORIG 08a26e97 switch 线性决策（cmp1 je; cmp2 je; test 0 fall）
            switch (es.m_field04)
            {
            case 0:
                fprintf(file, "`[breakable]` %d %d %d %d\n", es.m_field14,
                        es.m_field18, es.m_field1c, es.m_field20);
                break;
            case 1:
                fprintf(file, "`[half]` %d %d %d %d\n", es.m_field14,
                        es.m_field18, es.m_field1c, es.m_field20);
                break;
            case 2:
                fprintf(file, "`[unbreakable]` %d %d %d %d\n", es.m_field14,
                        es.m_field18, es.m_field1c, es.m_field20);
                break;
            }
            // ORIG 此处重做下标读取 +0x24（不复用拷贝缓存）
            if (script->m_eventSupers[i].m_field24)
                fwrite("`[with blast]`\n", 1, 0xf, file);
            fwrite("[/event super on scr]\n", 1, 0x16, file);
        }
    }
    // ---- [atk super on scr] +0x108 ----
    if (!script->m_atkSupers.empty())
    {
        fwrite("[atk super on scr]\n", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_atkSupers.size(); ++i)
        {
            const STAtkSuper as = script->m_atkSupers[i];  // ORIG 08a27073：值拷贝（6 槽）
            fprintf(file, "%d", as.m_field04);
            // ORIG 08a270c8 switch 二分（cmp 0xe je; cmp 0xf je; cmp 0xd jne）
            switch (as.m_field00)
            {
            case 0xd:
                fwrite(" `[breakable]`", 1, 0xe, file);
                break;
            case 0xe:
                fwrite(" `[half]`", 1, 9, file);
                break;
            case 0xf:
                fwrite(" `[unbreakable]`", 1, 0x10, file);
                break;
            }
            fprintf(file, " %d %d %d %d\n", as.m_field08, as.m_field0c,
                    as.m_field10, as.m_field14);
        }
        fwrite("[/atk super on scr]\n", 1, 0x14, file);
    }
    // ---- [cooltime] +0x114（front()+0x4 vector<int>）----
    if (!script->m_coolTimes.empty())
    {
        fwrite("[cooltime]\n", 1, 0xb, file);
        // ORIG 条件与体内各自重新求值 front()（P38）
        for (unsigned int i = 0; i < script->m_coolTimes.front().m_field04.size(); ++i)
            fprintf(file, "%d\n", script->m_coolTimes.front().m_field04[i]);
        fwrite("[/cooltime]\n", 1, 0xc, file);
    }
    // ---- [spawn prob] ----
    if (script->m_fieldc4c != defaultScript.m_fieldc4c)
        fprintf(file, "[spawn prob] %d\n", script->m_fieldc4c);
    // ---- [drop item offset] ----
    if (script->m_field36c != defaultScript.m_field36c ||
        script->m_field370 != defaultScript.m_field370)
        fprintf(file, "[drop item offset] %d %d\n",
                script->m_field36c, script->m_field370);
    // ---- [is grabable]（==0 才输出）----
    if (!script->m_flag368)
        fwrite("[is grabable] 0\n", 1, 0x10, file);
    // ---- [think term]/[destination change term]（m_aiInfo）----
    STAIInfoScript* aiInfo = &script->m_aiInfo;   // ORIG -0x128 命名指针槽
    if (aiInfo->m_thinkTerm != defaultScript.m_aiInfo.m_thinkTerm)
        fprintf(file, "[think term] %d\n", aiInfo->m_thinkTerm);
    if (aiInfo->m_destinationChangeTerm !=
        defaultScript.m_aiInfo.m_destinationChangeTerm)
        fprintf(file, "[destination change term] %d\n",
                aiInfo->m_destinationChangeTerm);
    // ---- [ai pattern]：5 难度 ----
    if (isExistAiPattern(aiInfo, -1))
    {
        fwrite("[ai pattern]\n", 1, 0xd, file);
        int patternIdx = 0;                        // ORIG -0x80 命名槽
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[easy]\n", 1, 7, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/easy]\n", 1, 8, file);
        }
        patternIdx = 1;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[medium]\n", 1, 9, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/medium]\n", 1, 0xa, file);
        }
        patternIdx = 2;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[hard]\n", 1, 7, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/hard]\n", 1, 8, file);
        }
        patternIdx = 3;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[ultimate]\n", 1, 0xb, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/ultimate]\n", 1, 0xc, file);
        }
        patternIdx = 4;
        if (isExistAiPattern(aiInfo, patternIdx))
        {
            fwrite("[hero]\n", 1, 7, file);
            exportAIPatternPath(file, aiInfo, patternIdx);
            fwrite("[/hero]\n", 1, 8, file);
        }
        fwrite("[/ai pattern]\n\n", 1, 0xf, file);
    }
    // ---- [targeting bonus]（22 项；i==20/21 用状态/攻击标签表）----
    {
        bool anyBonus = false;
        for (int i = 0; i <= 0x15; ++i)
        {
            // ORIG 08a27756：置位后 `jmp 08a2776e`（循环出口）= 原码有 break
            if (!script->m_aiInfo.m_targetingBonusStr[i].empty())
            {
                anyBonus = true;
                break;
            }
        }
        if (anyBonus)
        {
            fwrite("[targeting bonus]\n", 1, 0x12, file);
            for (int i = 0; i <= 0x15; ++i)
            {
                // ORIG 08a277b0：此两处走 **this 基址**（script->m_aiInfo.x），非 aiInfo 指针
                int bonus = script->m_aiInfo.m_targetingBonus[i];   // ORIG -0x74
                fprintf(file, "%d ", bonus);
                for (unsigned int j = 0;
                     j < script->m_aiInfo.m_targetingBonusStr[i].size(); ++j)
                {
                    int v = script->m_aiInfo.m_targetingBonusStr[i][j];   // ORIG -0x6c
                    // ORIG 08a277e6：switch 双 case（单 load i + 双 je 前跳）
                    switch (i)
                    {
                    case 0x14:
                        fprintf(file, "`%s` ", ACTIVEOBJECT_STATE_STRING[v]);
                        break;
                    case 0x15:
                        fprintf(file, "`%s` ", ACTIVESTATUS_ATTACK_TAG[v]);
                        break;
                    }
                }
                if ((unsigned int)(i - 0x14) < 2)   // ORIG 08a2789b：sub/cmp/ja
                    fputc('0', file);   // ORIG 08a278b0：字面量 0x30（'0'）
                fputc('\n', file);
            }
            fwrite("[/targeting bonus]\n", 1, 0x13, file);
        }
    }
    // ---- [keep distance with target] ----
    if (aiInfo->m_keepDistance !=
        defaultScript.m_aiInfo.m_keepDistance)
        fprintf(file, "[keep distance with target] %d\n",
                aiInfo->m_keepDistance);
    // ---- [keep range distance with target]（4 组 vector，i==0 带序号）----
    for (unsigned int i = 0; i < aiInfo->m_keepRange0.size(); ++i)
    {
        // ORIG 08a27953：主路径 i!=0（带 %d] 序号）；i==0 走 je 无序号块。
        // 两分支各 4 次 operator[]（共 8 静态调用点，P38）；求值序 0x1d4→0x1b0。
        if (i != 0)
            fprintf(file, "[keep range distance with target %d] %d %d %d %d\n",
                    i,
                    aiInfo->m_keepRange0[i],
                    aiInfo->m_keepRange1[i],
                    aiInfo->m_keepRange2[i],
                    aiInfo->m_keepRange3[i]);
        else
            fprintf(file, "[keep range distance with target] %d %d %d %d\n",
                    aiInfo->m_keepRange0[i],
                    aiInfo->m_keepRange1[i],
                    aiInfo->m_keepRange2[i],
                    aiInfo->m_keepRange3[i]);
    }
    // ---- [warlike]（ORIG 用 m_aiInfo.m_intelligence 二次导出）----
    if (aiInfo->m_intelligence !=
        defaultScript.m_aiInfo.m_intelligence)
        fprintf(file, "[warlike] %d\n", aiInfo->m_intelligence);
    // ---- [intelligence]/[vision]/[back vision] ----
    // ORIG 08a27afe：条件仅比较 +0x1f0/+0x1f4（不含 intelligence）
    if (aiInfo->m_field1f0 != defaultScript.m_aiInfo.m_field1f0 ||
        aiInfo->m_field1f4 != defaultScript.m_aiInfo.m_field1f4)
        fprintf(file, "[intelligence] %d %d %d\n",
                aiInfo->m_intelligence,
                aiInfo->m_field1f0, aiInfo->m_field1f4);
    if (aiInfo->m_vision != defaultScript.m_aiInfo.m_vision)
        fprintf(file, "[vision] %d\n", aiInfo->m_vision);
    if (aiInfo->m_backVision != defaultScript.m_aiInfo.m_backVision)
        fprintf(file, "[back vision] %d\n", aiInfo->m_backVision);
    // ---- [reflection object] +0xc80/+0xc8c ----
    if (!script->m_fieldc8c.empty())
    {
        fwrite("[reflection object]\n", 1, 0x14, file);
        for (unsigned int i = 0; i < script->m_fieldc8c.size(); ++i)
            // ORIG 08a27c66：先 c8c[i] 后 c80[i]（先求值 c80→ebx）
            fprintf(file, "%d %d\n", script->m_fieldc8c[i],
                    script->m_fieldc80[i]);
        fwrite("[/reflection object]\n", 1, 0x15, file);
    }
    // ---- [set damage check] / [is direction to move] ----
    if (script->m_field380 != defaultScript.m_field380)
        fprintf(file, "[set damage check] %d\n", script->m_field380);
    if (script->m_field37c != defaultScript.m_field37c)
        fwrite("[is direction to move] 1\n", 1, 0x19, file);
    // ---- float 三率 ----
    if (script->m_fieldc54 != defaultScript.m_fieldc54)
        fprintf(file, "[absolute damage resistance rate] %f\n",
                (double)script->m_fieldc54);
    if (script->m_field540 != defaultScript.m_field540 ||
        script->m_field544 != defaultScript.m_field544)
        fprintf(file, "[hp regen rate] %f %f\n",
                (double)script->m_field540, (double)script->m_field544);
    if (script->m_fieldc58 != defaultScript.m_fieldc58)
        fprintf(file, "[absolute defense ignore rate] %f\n",
                (double)script->m_fieldc58);
    // ---- [lie frame]（5 项任一不同）----
    {
        bool anyLie = false;
        for (int i = 0; i <= 4; ++i)
        {
            // ORIG 08a27e27：无 break，全 5 轮跑完（仅置位）
            if (script->m_fieldc5c[i] != defaultScript.m_fieldc5c[i])
                anyLie = true;
        }
        if (anyLie)
        {
            fwrite("[lie frame] ", 1, 0xc, file);
            for (int i = 0; i <= 4; ++i)
                fprintf(file, "%d ", script->m_fieldc5c[i]);
            fputc('\n', file);
        }
    }
    // ---- [forced knockback]/[No Combo Rate Monster] ----
    if (script->m_field1cac != defaultScript.m_field1cac)
        fprintf(file, "[forced knockback] %d\n", script->m_field1cac);
    if (script->m_field1ca8 != defaultScript.m_field1ca8)
        fwrite("[No Combo Rate Monster]\n", 1, 0x18, file);
    // ---- [monster title]/[destroy count]/[destroy count damage] ----
    if (!script->m_monsterTitle.empty())
        fprintf(file, "[monster title] `%s`\n", script->m_monsterTitle.c_str());
    if (script->m_field6f4 != defaultScript.m_field6f4)
        fprintf(file, "[destroy count] %d\n", script->m_field6f4);
    if (script->m_field6f8 != defaultScript.m_field6f8)
        fprintf(file, "[destroy count damage] %d\n", script->m_field6f8);
    // ---- [overhead gauge type]/[overhead gauge pos]/[flying mark] ----
    if (script->m_fieldc74 != defaultScript.m_fieldc74)
        fprintf(file, "[overhead gauge type] %d\n", script->m_fieldc74);
    if (script->m_fieldc78 != defaultScript.m_fieldc78 &&
        script->m_fieldc7c != defaultScript.m_fieldc7c)   // ORIG 08a2803b：&&
        fprintf(file, "[overhead gauge pos] %d %d\n",
                script->m_fieldc78, script->m_fieldc7c);
    if (script->m_flagc70 != defaultScript.m_flagc70)
        fwrite("[flying mark] 1\n", 1, 0x10, file);
    // ---- [burning death path] ----
    if (!script->m_field098.empty())
        fprintf(file, "[burning death path] `%s`\n",
                script->m_field098.c_str());
    // ---- [initial target] +0x340 deque ----
    if (!script->m_initialTargets.empty())
    {
        fwrite("[initial target]\n", 1, 0x11, file);
        // ORIG：非 const iterator；两字段各解引用一次（P38）
        for (std::deque<STInitialTarget>::iterator it =
                 script->m_initialTargets.begin();
             it != script->m_initialTargets.end(); ++it)
            fprintf(file, "%s %d\n", (*it).m_field04.c_str(), (*it).m_field00);
        fwrite("[/initial target]\n", 1, 0x12, file);
    }
    // ---- [level of difficulty] +0xf0（5 项）----
    if (!script->m_difficulties.empty())
    {
        fwrite("[level of difficulty]\n", 1, 0x16, file);
        fprintf(file, "[easy] %d %d\n",
                script->m_difficulties[0].m_field00,
                script->m_difficulties[0].m_field04);
        fprintf(file, "[medium] %d %d\n",
                script->m_difficulties[1].m_field00,
                script->m_difficulties[1].m_field04);
        fprintf(file, "[hard] %d %d\n",
                script->m_difficulties[2].m_field00,
                script->m_difficulties[2].m_field04);
        fprintf(file, "[ultimate] %d %d\n",
                script->m_difficulties[3].m_field00,
                script->m_difficulties[3].m_field04);
        fprintf(file, "[hero] %d %d\n",
                script->m_difficulties[4].m_field00,
                script->m_difficulties[4].m_field04);
        fwrite("[/level of difficulty]\n", 1, 0x17, file);
    }
    // ---- 四系 attack（m_paramTable）与 [rigidity]（任一非零）----
    if (script->m_paramTable[0][0] || script->m_paramTable[0][1])
        fprintf(file, "[fire attack] %d %d\n",
                script->m_paramTable[0][0], script->m_paramTable[0][1]);
    if (script->m_paramTable[1][0] || script->m_paramTable[1][1])
        fprintf(file, "[water attack] %d %d\n",
                script->m_paramTable[1][0], script->m_paramTable[1][1]);
    if (script->m_paramTable[2][0] || script->m_paramTable[2][1])
        fprintf(file, "[dark attack] %d %d\n",
                script->m_paramTable[2][0], script->m_paramTable[2][1]);
    if (script->m_paramTable[3][0] || script->m_paramTable[3][1])
        fprintf(file, "[light attack] %d %d\n",
                script->m_paramTable[3][0], script->m_paramTable[3][1]);
    if (script->m_field528[0] || script->m_field528[1])
        fprintf(file, "[rigidity] %d %d\n",
                script->m_field528[0], script->m_field528[1]);
    // ---- [pattern] +0x688（vector<vector<int>> front() 循环）----
    if (!script->m_patterns.empty())
    {
        fwrite("[pattern]\n", 1, 0xa, file);
        // ORIG 条件与体内各自重新求值 front()（P38）
        for (unsigned int i = 0; i < script->m_patterns.front().size(); ++i)
            fprintf(file, "%d\n", script->m_patterns.front()[i]);
        fwrite("[/pattern]\n", 1, 0xb, file);
    }
    // ---- [fix direction] +0xcd4/+0xcd8 ----
    if (script->m_flagcd4 != defaultScript.m_flagcd4)
    {
        std::string dir = "";
        if (script->m_fieldcd8 != defaultScript.m_fieldcd8)
        {
            // ORIG 08a286e2 switch：单 load 入 eax + test je / cmp 1 je 前跳
            switch (script->m_fieldcd8)
            {
            case 0:
                dir = "[left]";
                break;
            case 1:
                dir = "[right]";
                break;
            }
        }
        if (dir == "")
            fwrite("[fix direction] 1\n", 1, 0x12, file);
        else
            fprintf(file, "[fix direction] `%s`\n", dir.c_str());
    }
    // ---- [limit slowdown atkspeed]/[limit slowdown movespeed] ----
    if (script->m_field1c94 != defaultScript.m_field1c94)
        fprintf(file, "[limit slowdown atkspeed] %d\n", script->m_field1c94);
    if (script->m_field1c98 != defaultScript.m_field1c98)
        fprintf(file, "[limit slowdown movespeed] %d\n", script->m_field1c98);
    // ---- [non targeted]/[target appendage] ----
    if (script->m_field56c)
        fwrite("[non targeted] 1\n", 1, 0x11, file);
    if (!script->m_field570.empty())
        fprintf(file, "[target appendage] `%s`\n",
                script->m_field570.c_str());
    // ---- [damage time balance]/[stun time balance] ----
    if (!script->m_field554.empty())
    {
        fwrite("[damage time balance]", 1, 0x15, file);
        for (unsigned int i = 0; i < script->m_field554.size(); ++i)
            fprintf(file, "%d ", script->m_field554[i]);
    }
    if (!script->m_field560.empty())
    {
        fwrite("[stun time balance]", 1, 0x13, file);
        for (unsigned int i = 0; i < script->m_field560.size(); ++i)
            fprintf(file, "%d ", script->m_field560[i]);
    }
    // ---- [add gravity in aerial] ----
    if (script->m_field548)
        fprintf(file, "[add gravity in aerial] 1 %.1f %.1f\n",
                (double)script->m_field54c, (double)script->m_field550);
    // ---- [summon skill status up type] ----
    if (script->m_field3c0 != defaultScript.m_field3c0)
        fprintf(file, "[summon skill status up type] %d\n",
                script->m_field3c0);
    // ---- [attack down prob]（任一 attackMethod +0x4 float 非零）----
    {
        bool anyProb = false;
        for (unsigned int i = 0; i < script->m_attackMethods.size(); ++i)
        {
            // ORIG 08a28a6b：无 break，逐轮置位（flds+cmp0 经 or 物化）
            if (script->m_attackMethods[i].m_field04 != 0.0f)
                anyProb = true;
        }
        if (anyProb)
        {
            fwrite("[attack down prob]\n", 1, 0x13, file);
            for (unsigned int i = 0; i < script->m_attackMethods.size(); ++i)
                fprintf(file, "%f\n",
                        (double)script->m_attackMethods[i].m_field04);
            fwrite("[/attack down prob]\n", 1, 0x14, file);
        }
    }
    // ---- [strong]/[weak]（+0x1ca0 非零；>0 strong，<0 weak 取反）----
    if (script->m_field1ca0)
    {
        if (script->m_field1ca0 > 0)
            fprintf(file, "[strong] %d %.2f\n", script->m_field1ca0,
                    (double)script->m_field1ca4);
        else
            fprintf(file, "[weak] %d %.2f\n", -script->m_field1ca0,
                    (double)script->m_field1ca4);
    }
    // ---- [explain] ----
    if (!script->m_explain.empty())
        fprintf(file, "[explain] `%s`\n", script->m_explain.c_str());
    // ---- [catch item] +0x70（+0x8 按 float→double→%d 低位，ORIG 忠实）----
    if (!script->m_catchItems.empty())
    {
        fwrite("[catch item]\n", 1, 0xd, file);
        for (unsigned int i = 0; i < script->m_catchItems.size(); ++i)
        {
            // ORIG 逐字段重做下标（f8→f4→f0 求值序）；f8 flds→fstpl 双精度
            // 槽（-0x2f60），传参时 fldl/fstpl 二次物化 ⇒ 实参为 double 值。
            fprintf(file, "%d %d %d\n", script->m_catchItems[i].m_field0,
                    script->m_catchItems[i].m_field4,
                    (double)script->m_catchItems[i].m_field8);
        }
        fwrite("[/catch item]\n", 1, 0xe, file);
    }
    // ---- [fixed height]/[can beat index] ----
    if (script->m_field1cb0 != defaultScript.m_field1cb0)
        fprintf(file, "[fixed height] %d\n", script->m_field1cb0);
    if (!script->m_field1cb4.empty())
    {
        fwrite("[can beat index]\n", 1, 0x11, file);
        // ORIG：非 const iterator + 前置 ++（ppEv 实证）
        for (std::vector<int>::iterator it =
                 script->m_field1cb4.begin();
             it != script->m_field1cb4.end(); ++it)
            fprintf(file, "%d\n", *it);
        fwrite("[/can beat index]\n", 1, 0x12, file);
    }
    // ---- [control passive object] 判定（ORIG 08a28e64：bool 组在 mounting 之前算，
    //      函数级无块作用域，槽 -0x120..-0x115）----
    bool cpoIndex   = script->m_field658 != defaultScript.m_field658;
    bool cpoX       = script->m_field65c != defaultScript.m_field65c;
    bool cpoY       = script->m_field660 != defaultScript.m_field660;
    bool cpoW       = script->m_field664 != defaultScript.m_field664;
    bool cpoH       = script->m_field668 != defaultScript.m_field668;
    bool cpoVelX    = script->m_field66c != defaultScript.m_field66c;
    bool cpoVelY    = script->m_field670 != defaultScript.m_field670;
    bool cpoMaster  = script->m_field678 != defaultScript.m_field678;
    bool cpoOthers  = script->m_field67c != defaultScript.m_field67c;
    bool cpoInherit = script->m_field680 != defaultScript.m_field680;
    // ORIG 08a28f7a：transp 以短路 || 物化（mov $1/$0）先于 fix 计算（槽序 -0x116/-0x115）
    bool cpoTransp  = cpoMaster || cpoOthers || cpoInherit;
    bool cpoFix     = script->m_field684 != defaultScript.m_field684;
    // ---- [mounting action]/[mounted action]/[unmounting action]（size!=0）----
    if (script->m_mountingAction.size() != 0)
        fprintf(file, "[mounting action] `%s`\n",
                script->m_mountingAction.c_str());
    if (script->m_mountedAction.size() != 0)
        fprintf(file, "[mounted action] `%s`\n",
                script->m_mountedAction.c_str());
    if (script->m_unmountingAction.size() != 0)
        fprintf(file, "[unmounting action] `%s`\n",
                script->m_unmountingAction.c_str());
    {
        // ORIG 08a290c0：主 OR 链 …,vely(-0x11a) → fix(-0x115) → transp(-0x116)
        if (cpoIndex || cpoX || cpoY || cpoW || cpoH || cpoVelX || cpoVelY ||
            cpoFix || cpoTransp)
        {
            fwrite("[control passive object]\n", 1, 0x19, file);
            if (cpoIndex)
                fprintf(file, "\t[index] %d\n", script->m_field658);
            if (cpoX)
                fprintf(file, "\t[x pos] %d\n", script->m_field65c);
            if (cpoY)
                fprintf(file, "\t[y pos] %d\n", script->m_field660);
            if (cpoW)
                fprintf(file, "\t[width] %d\n", script->m_field664);
            if (cpoH)
                fprintf(file, "\t[height] %d\n", script->m_field668);
            if (cpoVelX)
                fprintf(file, "\t[velocity x] %lf\n",
                        (double)script->m_field66c);
            if (cpoVelY)
                fprintf(file, "\t[velocity y] %lf\n",
                        (double)script->m_field670);
            if (cpoTransp)
            {
                fwrite("\t[transparency]\n", 1, 0x10, file);
                if (cpoMaster)
                    fprintf(file, "\t\t[master] %d\n", script->m_field678);
                if (cpoOthers)
                    fprintf(file, "\t\t[others] %d\n", script->m_field67c);
                if (cpoInherit)
                    fwrite("\t\t[option] `inherit`\n", 1, 0x15, file);
                fwrite("\t[/transparency]\n", 1, 0x11, file);
            }
            if (cpoFix)
            {
                std::string dir;
                // ORIG 08a29334 switch：单 load + test je / cmp1 je 前跳
                switch (script->m_field684)
                {
                case 0:
                    dir = "[left]";
                    break;
                case 1:
                    dir = "[right]";
                    break;
                }
                fprintf(file, "\t[fix direction] `%s`\n", dir.c_str());
            }
            if (script->m_field674)
                fwrite("\t[option] `convert y to z`\n", 1, 0x1b, file);
            fwrite("[/control passive object]\n\n", 1, 0x1b, file);
        }
    }
    // ---- [ridableskill definition] +0xcdc（8 技能槽）----
    for (unsigned int i = 0; i < script->m_ridableSkills.size(); ++i)
    {
        fwrite("[ridableskill definition]\n", 1, 0x1a, file);
        fprintf(file, "\t[pattern] %d\n", script->m_ridableSkills[i].m_field00);
        fwrite("\t[image path] ", 1, 0xe, file);
        std::string img("``");
        if (!script->m_ridableSkills[i].m_name.empty())
            img = script->m_ridableSkills[i].m_name.c_str();
        fprintf(file, "`%s`\n", img.c_str());
        fwrite("\t[skill]\n", 1, 9, file);
        for (int j = 0; j <= 7; ++j)
        {
            // ORIG 每轮一次 operator[] 拷指针，随后字段经该指针访问
            STRidableSkill* rs = &script->m_ridableSkills[i];
            fprintf(file, "\t\t%d", rs->m_field08[j]);
            fprintf(file, " %d", rs->m_fielda8[j]);
            std::string typeName = "normal";
            switch (rs->m_field88[j])
            {
            case 0: typeName = "`normal`"; break;
            case 1: typeName = "`defaultcancel`"; break;
            case 2: typeName = "`movable`"; break;
            case 3: typeName = "`charge`"; break;
            case 4: typeName = "`evasion`"; break;
            case 5: typeName = "`super`"; break;
            default: break;
            }
            fprintf(file, " %s", typeName.c_str());
            // ORIG 08a29735：switch 用表达式（load#1）；此处 register type 重新
            // load（load#2 入 eax），随后 cmp $5 与 shl 复用同一寄存器值。
            register unsigned int type = (unsigned int)rs->m_field88[j];
            if (type < 6)
            {
                // ORIG 08a2974b-08a29766：bits 常驻 ebx（register 局部，不落栈槽）
                register unsigned int bit = 1u << type;
                if ((bit & 0x37u) != 0)
                {
                    int cnt = (int)rs->m_levels[j].size();
                    fprintf(file, " %d", cnt);
                    for (int k = 0; k < cnt; ++k)
                        fprintf(file, " %d", rs->m_levels[j][k]);
                }
                else if ((bit & 8u) != 0)
                    fprintf(file, " %d %d", rs->m_fieldc8[j],
                            rs->m_fielde8[j]);
            }
            fputc('\n', file);
        }
        fwrite("[/ridableskill definition]\n\n", 1, 0x1c, file);
    }
    // ---- [ridableskill explanation] +0xce8 ----
    for (unsigned int i = 0; i < script->m_ridableSkillExplains.size(); ++i)
    {
        fwrite("[ridableskill explanation]\n", 1, 0x1b, file);
        fprintf(file, "\t[pattern] %d\n",
                script->m_ridableSkillExplains[i].m_field10);
        // ORIG 每字段：key=0 在 if 外初始化（08a298xx），命中后 key=c_str()，
        // getTagByName 第2参重做 c_str()、第3参用 key 槽（P38+P15）
        // ORIG 四段共用**同一** key 槽（-0x28 复用）：单声明 + 每段前手动置 0
        const wchar_t* key = 0;
        if (!script->m_ridableSkillExplains[i].m_field00.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field00.c_str();
            fprintf(file, "\t[title] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field00.c_str(),
                        key, false)));
        }
        if (!script->m_ridableSkillExplains[i].m_field04.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field04.c_str();
            fprintf(file, "\t[subtitle] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field04.c_str(),
                        key, false)));
        }
        if (!script->m_ridableSkillExplains[i].m_field08.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field08.c_str();
            fprintf(file, "\t[category] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field08.c_str(),
                        key, false)));
        }
        if (!script->m_ridableSkillExplains[i].m_field0c.empty())
        {
            key = (const wchar_t*)script->m_ridableSkillExplains[i].m_field0c.c_str();
            fprintf(file, "\t[detail] %s\n",
                    toTString(g_scriptStringManager_.getTagByName(
                        7,
                        (const wchar_t*)script->m_ridableSkillExplains[i].m_field0c.c_str(),
                        key, false)));
        }
        fwrite("[/ridableskill explanation]\n\n", 1, 0x1d, file);
    }
    // ---- [physical attack besed on diff]（0xcf4 != -1.0f；200 行 ×5）----
    if (script->m_difficultyScales[0][0] != -1.0f)
    {
        fwrite("[physical attack besed on diff]\n", 1, 0x20, file);
        // ORIG 08a29d21/08a29df2：while (i < 200 && 0.0 <= scales[i][1])，
        // cond1 在 ++i 后（loop 尾 setle 物化），cond2 在 body 头（fucompp+sete）
        int diffRow = 0;
        while (diffRow < 200)
        {
            // ORIG 08a29d34 `add $0xcf8`（= &m_difficultyScales[diffRow][1]）：
            // 原码终止判据读第 2 列，打印循环仍从 col=0 起（P171 照抄）；
            // 比较式写 `x < 0`（探针 g2）才出 ORIG 的 fucompp/test $0x45,%ah/sete
            if (script->m_difficultyScales[diffRow][1] < 0.0f)
                break;
            fprintf(file, "%d", diffRow + 1);
            for (int col = 0; col <= 4; ++col)
            {
                fputc('\t', file);
                // ORIG 08a29d8e：rate 命名 float 槽（mov %eax,-0x1c; flds -0x1c）
                float rate = script->m_difficultyScales[diffRow][col];
                fprintf(file, "%.2f",
                        (double)rate);
            }
            fputc('\n', file);
            ++diffRow;
        }
        fwrite("[/physical attack besed on diff]\n", 1, 0x21, file);
    }
    fclose(file);
    convertBinaryToText(saveFileName);
    return true;
