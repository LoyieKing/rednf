bool importAICharacterCommonScript(STAICharacterCommonScript* script,
                                   const char* path, bool byLevel)
{
    if (!loadRDARScriptFile(g_aiCharacterScriptBaseDirectory, path))
        return false;

    script->clear();
    getDirectoryPath(script->m_info.m_dir, "%s%s",
                     g_aiCharacterScriptBaseDirectory, path);

    std::string tag;
    std::string tok;
    bool ok = false;

    while (true)
    {
        if (!ScanType(tag, true))
            return true;                       // LAB_088cc01f: ebx=1

        if (tag == "[minimum info]")
        {
            // ORIG 0x088c9dd0..0x088c9eaf：ScanStr→m_name=tok + 14 个逐条
            // ScanInt（第 5 个写 m_field4），无循环合并（P183 物化口径）。
            ScanStr(&tok);
            script->m_minimumInfo.m_name = tok;
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            script->m_minimumInfo.m_field4 = ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanStr(&tok);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
            ScanInt((bool*)0);
        }
        else if (tag == "[NPC]")
        {
            script->m_npcId = ScanInt((bool*)0);          // +0x1008
        }
        else if (tag == "[additional character status]")
        {
            STRDCharacterStatus& st = script->m_status;
            st.m_hpMax       = ScanInt((bool*)0);         // +0x00
            st.m_mpMax       = ScanInt((bool*)0);         // +0x04
            st.m_physAttack  = (unsigned short)ScanInt((bool*)0);  // +0x08
            st.m_physDefense = (unsigned short)ScanInt((bool*)0);  // +0x0a
            st.m_magAttack   = (unsigned short)ScanInt((bool*)0);  // +0x0c
            st.m_magDefense  = (unsigned short)ScanInt((bool*)0);  // +0x0e
            st.m_inventoryLimit = ScanInt((bool*)0);      // +0x3a
            st.m_hpRegen     = (unsigned short)ScanInt((bool*)0);  // +0x3e
            st.m_mpRegen     = (unsigned short)ScanInt((bool*)0);  // +0x40
            st.m_moveSpeed   = ScanInt((bool*)0);         // +0x42
            st.m_attackSpeed = (unsigned short)ScanInt((bool*)0);  // +0x46
            st.m_castSpeed   = (unsigned short)ScanInt((bool*)0);  // +0x48
            st.m_hitRecovery = (unsigned short)ScanInt((bool*)0);  // +0x4a
            st.m_jumpPower   = (unsigned short)ScanInt((bool*)0);  // +0x4c
            st.m_weight      = ScanInt((bool*)0);         // +0x4e
            st.m_field52     = (unsigned char)ScanInt((bool*)0);   // +0x52
            for (int j = 0; j <= 3; ++j)                  // elemResist @+0x10
                st.m_elemResist[j] = (unsigned short)ScanInt((bool*)0);
            for (int n = 0; ; ++n)                        // activeResist @+0x18
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                st.m_activeResist[n] = (short)v;      // ORIG 0x088ca0bd 无界写，照抄（P171）
            }
        }
        else if (tag == "[equipment]")
        {
            while (true)
            {
                STAICharacterEquipmentData e;
                e.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                e.m_b = ScanInt((bool*)0);
                e.m_c = ScanInt((bool*)0);
                script->m_equipment.push_back(e);
            }
        }
        else if (tag == "[skill]" || tag == "[quick skill]")
        {
            while (true)
            {
                STAICharacterSkillData s;
                s.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                s.m_b = ScanInt((bool*)0);
                if (tag == "[quick skill]")
                    script->m_skills[0].push_back(s);      // +0x70
                else
                    script->m_skills[1].push_back(s);      // +0x7c
            }
        }
        else if (tag == "[item]" || tag == "[quick item]")
        {
            while (true)
            {
                STAICharacterItemData it;
                it.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                it.m_b = ScanInt((bool*)0);
                if (tag == "[quick item]")
                    script->m_items[0].push_back(it);      // +0x94
                else
                    script->m_items[1].push_back(it);      // +0xa0
            }
        }
        else if (tag == "[skill correction]")
        {
            while (true)
            {
                const int first = ScanInt(&ok);
                if (!ok)
                    break;
                STAICharacterSkillCorrection c;
                c.m_skill = first;
                c.m_rate = ScanFloat((bool*)0);
                script->m_corrections.push_back(c);
            }
        }
        else if (tag == "[key stream]")
        {
            bool again = true;
            while (again)
            {
                again = false;
                std::string keyFile;
                bool keyOk = ScanStr(&keyFile);
                if (!keyOk)
                    break;
                if (isKeyFile(keyFile.c_str()))
                {
                    tok = keyFile;
                    keyFile = "";                      // ORIG 0x088ca432 operator=(const char*)
                }
                else
                {
                    keyOk = ScanStr(&tok);
                }
                if (*keyFile.c_str() == '\0')          // ORIG 0x088ca456 c_str+testb（非 empty()）
                    keyFile = getFileName(tok);
                script->m_keyFiles.push_back(keyFile);
                if (keyOk)
                {
                    makeKeyFullPath(script->m_info.m_dir, tok);
                    script->m_keyStreamFiles.push_back(tok);
                    again = true;
                }
            }
        }
        else if (tag == "[key cooltime]")
        {
            while (ScanStr(&tok))
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_keyCooltimes[tok] = v;
            }
        }
        else if (tag == "[rarity]")
        {
            if (!ScanStr(&tok))
                return false;
            if (tok == "common")         script->m_rarity = 0;
            else if (tok == "uncommon")  script->m_rarity = 1;
            else if (tok == "rare")      script->m_rarity = 2;
            else if (tok == "unique")    script->m_rarity = 3;
            else if (tok == "epic")      script->m_rarity = 4;
            else if (tok == "chronicle") script->m_rarity = 5;
            script->m_rarity = getItemRarity(tok.c_str());
            if (script->m_rarity == 6)
                return false;
        }
        else if (tag == "[appearance point]")
        {
            script->m_appearancePoint = ScanInt((bool*)0);
        }
        else if (tag == "[death tower item]")
        {
            script->m_deathTowerItems.clear();
            while (true)
            {
                int first = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG 0x088ca81c/0x088ca837：pair<int&,int> 模板 ctor（第二参为
                // 右值实参）→ push_back(T&&)。
                script->m_deathTowerItems.push_back(
                    std::pair<int, int>(first, ScanInt((bool*)0)));
            }
        }
        else if (tag == "[leave body]")
        {
            script->m_leaveBody = (ScanInt((bool*)0) != 0);
        }
        else if (tag == "[waiting action]")   { ScanStr(&script->m_actionSet.m_waiting); }
        else if (tag == "[move action]")      { ScanStr(&script->m_actionSet.m_move); }
        else if (tag == "[dash action]")      { ScanStr(&script->m_actionSet.m_dash); }
        else if (tag == "[sit action]")       { ScanStr(&script->m_actionSet.m_sit); }
        else if (tag == "[damage action 1]")  { ScanStr(&script->m_actionSet.m_damage[0]); }
        else if (tag == "[damage action 2]")  { ScanStr(&script->m_actionSet.m_damage[1]); }
        else if (tag == "[down action]")      { ScanStr(&script->m_actionSet.m_down); }
        else if (tag == "[overturn action]")  { ScanStr(&script->m_actionSet.m_overturn); }
        else if (tag == "[jump action]")      { ScanStr(&script->m_actionSet.m_jump); }
        else if (tag == "[jumpattack action]"){ ScanStr(&script->m_actionSet.m_jumpattack); }
        else if (tag == "[attack action]" || tag == "[action list]")
        {
            script->m_actionSet.m_attackList.clear();
            std::string line;
            while (ScanStr(&line))
                script->m_actionSet.m_attackList.push_back(line);
        }
        else if (tag == "[etc action]" || tag == "[custom action]")
        {
            script->m_actionSet.m_etc.clear();
            std::string line;
            while (ScanStr(&line))
                script->m_actionSet.m_etc.push_back(line);
        }
        else if (tag == "[last action]")  { ScanStr(&script->m_lastAction); }
        else if (tag == "[proc action]")  { ScanStr(&script->m_procAction); }
        else if (tag == "[add gravity in aerial]")
        {
            script->m_addGravityInAerial = (ScanInt((bool*)0) != 0);
            script->m_gravityAerialA = ScanFloat((bool*)0);
            script->m_gravityAerialB = ScanFloat((bool*)0);
        }
        else if (tag == "[speech on situation]")
        {
            std::string line;
            for (int i = 0; i <= 0x6e; ++i)               // ORIG cmpl $0x6e; setle（P51）
                script->m_speeches[i].clear();           // 24*i + 0x300
            script->m_onAttackSpeech.clear();            // +0xd68
            script->m_onKeySpeech.clear();               // +0xd74
            while (true)
            {
                bool isType = false;
                if (!ScanTypeOrStr(isType, tok, true))
                    break;                               // 段结束（esi=1）
                if (tok == "[/speech on situation]")     // ORIG 不查 isType 标志
                    break;
                ScanStr(&line);
                const int prob = ScanInt((bool*)0);
                // [C05 F2/P310 一次性旋钮实测·已回退] `unsigned slot` 确使尾段谓词
                // 对齐 ORIG 的 `cmpl $0x6e,-0x44; ja`（内存直比；widthscan 的
                // cmp_l orig=8 our=7 差消失），但 `(std::size_t)slot+1` 三处使用点
                // 因无符号语义各多 1 条 ⇒ our_insns 2108→2111、gap −3→−6（净负）
                // ⇒ 按 P321 S1 恢复直证形态并封 N（本旋钮不可用，勿再试）。
                int slot = -1;                            // ORIG 0x088cadb2 先置 -1
                if (tok == "[on attack]")
                {
                    // ORIG 0x088cad83..0x088caeed：独立块（与 [on key] 不共享代码）。
                    // ORIG 0x088cad8d 只 lea 取址、全函数无 fv 初始化写 ⇒ 原码无初值
                    // （P0/P171：不写即不写，`= 0.0f` 反生 `mov $0,%eax;mov %eax,槽`）。
                    float fv;
                    // P251：返回值无栈槽往返 + 多路相等比较 ⇒ 原码是 switch
                    // （ORIG 0x088cad9b `test %eax,%eax;je` + `cmp $0x1,%eax;je` + `jmp`）。
                    switch (ScanFloatStr(fv, tok))
                    {
                    case 0:
                        slot = (int)fv;
                        break;
                    case 1:
                        for (std::size_t i = 0; i < script->m_keyFiles.size(); ++i)
                        {
                            if (script->m_keyFiles[i] == tok)
                            {
                                slot = (int)i;
                                break;
                            }
                        }
                        break;
                    }
                    if (script->m_onAttackSpeech.size() < (std::size_t)slot + 1)
                        script->m_onAttackSpeech.resize((std::size_t)slot + 1, APCSpeech());
                    script->m_onAttackSpeech[(std::size_t)slot].add(line, prob);
                }
                else if (tok == "[on key]")
                {
                    // 同 [on attack]：无初值 + switch（ORIG 0x088caf1a/0x088caf28）。
                    float fv;
                    switch (ScanFloatStr(fv, tok))
                    {
                    case 0:
                        slot = (int)fv;
                        break;
                    case 1:
                        for (std::size_t i = 0; i < script->m_keyFiles.size(); ++i)
                        {
                            if (script->m_keyFiles[i] == tok)
                            {
                                slot = (int)i;
                                break;
                            }
                        }
                        break;
                    }
                    if (script->m_onKeySpeech.size() < (std::size_t)slot + 1)
                        script->m_onKeySpeech.resize((std::size_t)slot + 1, APCSpeech());
                    script->m_onKeySpeech[(std::size_t)slot].add(line, prob);
                }
                else
                {
                    if (tok == "[on damage]")      slot = 0x65;
                    else if (tok == "[on die]")    slot = 0x66;
                    else if (tok == "[on revenge]") slot = 0x67;
                    else if (tok == "[on etc]")    slot = 0x68;
                    else if (tok == "[on appear]") slot = 0x69;
                    else if (tok == "[on disappear]") slot = 0x6a;
                    else if (tok == "[on go to help]") slot = 0x6b;
                    else if (tok == "[on stand]")  slot = 0x6c;
                    else
                        return false;                // ORIG: esi=0 → 返回失败
                    if ((unsigned)slot <= 0x6e)      // ORIG cmpl $0x6e,-0x44; ja（无符号）
                        script->m_speeches[slot].add(line, prob);
                }
            }
        }
        else if (tag == "[common champion elemental property]")
        {
            script->m_championElements.clear();
            while (ScanStr(&tok))
            {
                for (int e = 0; e < 5; ++e)
                {
                    if (tok == getElementName((ENUM_ELEMENT)e))
                    {
                        script->m_championElements.push_back(e);
                        break;
                    }
                }
            }
        }
        else if (tag == "[party member]")
        {
            script->m_partyMembers.clear();
            while (true)
            {
                int a = ScanInt(&ok);
                if (!ok)
                    break;
                int b = ScanInt(&ok);
                if (!ok)
                    break;
                // ORIG 0x088cb399：make_pair<int&,int&> sret + push_back 嵌套实参（P178）。
                script->m_partyMembers.push_back(std::make_pair(a, b));
            }
        }
        else if (tag == "[far attack reaction rate]")
        {
            const float f = ScanFloat(&ok);
            // ORIG flds [_DAT_08e0a1e0]=100.0f; fmulp; fldcw(截断); fistp
            script->m_farAttackReactionRate = (int)(f * 100.0f);
        }
        else if (tag == "[pvp win point]")
        {
            script->m_pvpWinPointLow = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_pvpWinPointHigh = ScanInt(&ok);
            if (!ok)
                return false;
            if (script->m_pvpWinPointHigh < script->m_pvpWinPointLow)
                return false;
        }
        else if (tag == "[character status rate]")
        {
            // ORIG 0x088cb4ed..0x088cb703：head 10 条 + tail 10 条逐条单写
            // （fstps 0xdac.. / 0xe28..），随后 for≤3 写 mid28、while 无界写 mid38。
            STRDCharacterStatusRate& r = script->m_statusRate;
            r.m_head[0] = ScanFloat((bool*)0);
            r.m_head[1] = ScanFloat((bool*)0);
            r.m_head[2] = ScanFloat((bool*)0);
            r.m_head[3] = ScanFloat((bool*)0);
            r.m_head[4] = ScanFloat((bool*)0);
            r.m_head[5] = ScanFloat((bool*)0);
            r.m_head[6] = ScanFloat((bool*)0);
            r.m_head[7] = ScanFloat((bool*)0);
            r.m_head[8] = ScanFloat((bool*)0);
            r.m_head[9] = ScanFloat((bool*)0);
            r.m_tail[0] = ScanFloat((bool*)0);
            r.m_tail[1] = ScanFloat((bool*)0);
            r.m_tail[2] = ScanFloat((bool*)0);
            r.m_tail[3] = ScanFloat((bool*)0);
            r.m_tail[4] = ScanFloat((bool*)0);
            r.m_tail[5] = ScanFloat((bool*)0);
            r.m_tail[6] = ScanFloat((bool*)0);
            r.m_tail[7] = ScanFloat((bool*)0);
            r.m_tail[8] = ScanFloat((bool*)0);
            r.m_tail[9] = ScanFloat((bool*)0);
            for (int t = 0; t <= 3; ++t)
                r.m_mid28[t] = ScanFloat((bool*)0);      // (t+0x370)*4+0x14 → Rate+0x28+4t
            for (int n = 0; ; ++n)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                // ORIG 0x088cb6cc 无界连续写 (n+0x374)*4+0x14 → Rate+0x38+4n；照抄（P171）。
                r.m_mid38[n] = f;
            }
        }
        else if (tag == "[custom skill level]")
        {
            while (true)
            {
                STAICharacterCustomSkillLevel c;
                c.m_a = ScanInt(&ok);
                if (!ok)
                    break;
                c.m_b = ScanInt((bool*)0);
                c.m_c = ScanInt((bool*)0);
                c.m_d = ScanInt((bool*)0);
                script->m_customSkills.push_back(c);
            }
        }
        else if (tag == "[attack damage rate]")
        {
            script->m_attackDamageRate = ScanFloat(&ok);
        }
        else if (tag == "[add equipment status from level]")
        {
            script->m_addEquipStatusFromLevel = ScanInt(&ok);
        }
        else if (tag == "[armor subtype]")
        {
            script->m_armorSubtype = ScanInt(&ok);
        }
        else if (tag == "[default equipment]")
        {
            script->m_defaultEquipment = ScanInt(&ok);
        }
        else if (tag == "[doll special dungeon]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_dollSpecialDungeons.push_back(v);
            }
        }
        else if (tag == "[character status rate special]")
        {
            // ORIG 0x088cb8da..（链位在 doll 之后）：special 独立块，e78.. / ef4..
            // 各 10 条逐条单写（fstps），for≤3 mid28、while 无界 mid38。
            STRDCharacterStatusRate& r = script->m_specialStatusRate;
            r.m_head[0] = ScanFloat((bool*)0);
            r.m_head[1] = ScanFloat((bool*)0);
            r.m_head[2] = ScanFloat((bool*)0);
            r.m_head[3] = ScanFloat((bool*)0);
            r.m_head[4] = ScanFloat((bool*)0);
            r.m_head[5] = ScanFloat((bool*)0);
            r.m_head[6] = ScanFloat((bool*)0);
            r.m_head[7] = ScanFloat((bool*)0);
            r.m_head[8] = ScanFloat((bool*)0);
            r.m_head[9] = ScanFloat((bool*)0);
            r.m_tail[0] = ScanFloat((bool*)0);
            r.m_tail[1] = ScanFloat((bool*)0);
            r.m_tail[2] = ScanFloat((bool*)0);
            r.m_tail[3] = ScanFloat((bool*)0);
            r.m_tail[4] = ScanFloat((bool*)0);
            r.m_tail[5] = ScanFloat((bool*)0);
            r.m_tail[6] = ScanFloat((bool*)0);
            r.m_tail[7] = ScanFloat((bool*)0);
            r.m_tail[8] = ScanFloat((bool*)0);
            r.m_tail[9] = ScanFloat((bool*)0);
            for (int t = 0; t <= 3; ++t)
                r.m_mid28[t] = ScanFloat((bool*)0);      // (t+0x3a4)*4+0x10 → Rate+0x28+4t
            for (int n = 0; ; ++n)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                r.m_mid38[n] = f;                        // ORIG 无界写，照抄（P171）
            }
        }
        else if (tag == "[attack damage rate special]")
        {
            script->m_attackDamageRateSpecial = ScanFloat(&ok);
        }
        else if (tag == "[attack stuck rate]")
        {
            // ORIG fstps 0x5f —— 绝对 +0x5f = STRDCharacterStatus(+0xc)+0x53 = m_field53
            *reinterpret_cast<float*>(&script->m_status.m_field53) = ScanFloat(&ok);
        }
        else if (tag == "[limit passiveobject]")
        {
            while (true)
            {
                const int v = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_limitPassiveObjects.push_back(v);
            }
        }
        else if (tag == "[overhead gauge type]")
        {
            script->m_overheadGaugeType = ScanInt((bool*)0);
        }
        else if (tag == "[overhead gauge pos]")
        {
            script->m_overheadGaugePosX = ScanInt((bool*)0);
            script->m_overheadGaugePosY = ScanInt((bool*)0);
        }
        else if (tag == "[getup invincible]")
        {
            script->m_getupInvincible = ScanInt((bool*)0);
        }
        else if (tag == "[drop avatar item]")
        {
            for (int i = 0; i <= 3; ++i)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_rates08[i] = f;
            }
            for (int k = 0; k <= 4; ++k)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_rates18[k] = f;
            }
            script->m_avatarDrop.m_field0 = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_avatarDrop.m_field4 = ScanInt(&ok);
            if (!ok)
                return false;
            while (true)
            {
                stEnamyDropAvatarElement_t el;
                el.m_itemId = ScanInt(&ok);
                if (!ok)
                    break;
                el.m_total = ScanInt(&ok);
                if (!ok)
                    return false;
                el.m_flag = (unsigned char)ScanInt(&ok);
                if (!ok)
                    return false;
                script->m_avatarDrop.m_total += el.m_total;
                el.m_total = script->m_avatarDrop.m_total;   // 回填累计值
                script->m_avatarDrop.m_elements.push_back(el);
            }
        }
        else if (tag == "[skill data up]")
        {
            importSkillDataUp(script->m_skillDataUps);
        }
        else if (tag == "[summon monster status rate]")
        {
            while (true)
            {
                const float f = ScanFloat(&ok);
                if (!ok)
                    break;
                script->m_summonStatusRates.push_back(f);
            }
        }
        else if (tag == "[no cooltime]")
        {
            script->m_noCooltime = 1;
        }
        else if (tag == "[check accountitem]")
        {
            script->m_checkAccountItem = (ScanInt((bool*)0) == 1);
        }
        else if (tag == "[hell monster]")
        {
            // ORIG 0x088cbfc3/0x088cbfc6：结果先落具名 int 槽（-0x10）再
            // `cmpl $0x0,-0x10(%ebp); setne %dl` ⇒ 原码有具名局部（P0 槽往返律）；
            // 直接 `(ScanInt(...) != 0)` 会被 4.4 折成 `test %eax,%eax`（少 mov+cmp）。
            const int v = ScanInt((bool*)0);
            script->m_isHellMonster = (v != 0);
        }
        else if (tag == "[animation job]")
        {
            const int v = ScanInt(&ok);
            if (!ok)
                return false;
            script->m_animationJob = v;
        }
        else
        {
            if (!importAIInfoScript(&script->m_info, &tag, false))
                return false;
        }

        if (byLevel)
            break;                       // ORIG do{}while(!param_3)
    }
    return true;
}