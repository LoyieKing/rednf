bool ImportCharacterScript(STCharacterScript* script, const char* path)
{
    // [b28d-solo-ScriptDepsA·大件波 BigSkillChar] AE 口径 ndiff 2728→**101**：
    //  ① [attack action]/[action list] 收尾具名 `bool endTag` 改纯短路 if——ORIG
    //     4.1 将 endTag 驻 %eax（`mov $1/$0; test %al`），4.4 具名 bool 必落 -0x29
    //     栈槽并把 cur* 指针组压深 4、全 deep 区 +4 平移（≈4300 行 diff 主源）；
    //     真值表等价（A&&B→break；C&&D→break；否则 return false）。
    //  ② [body image path] 的 `s_bodyImagePath[static_cast<ENUM>(m_job)]` rvalue
    //     经 reinterpret 左值引用直绑 &m_job（字段偏移 0）——ORIG `mov %eax,0x4`
    //     直传实证；消 -0x4c 临时槽（第二平移源）。map 键型保持 ENUM_CHARACTERJOB
    //     （ORIG W 实例 _ZNSt3mapI17ENUM_CHARACTERJOBSs...4find/ix 实证）。
    //  ③ [category] 块 `curEtc->m_category.clear()` 移至 t/isType 声明前（ORIG
    //     clear 先于 string ctor 序，消 hunk 错位）。
    // 语义核对：imm/字段访问常量组差集仅剩 `mov (%eax),%eax; mov %eax,%edx` 型
    // 4.1 寄存器复制（×4）与一处 imm:0 物化 ⇒ token 链/字段写/循环界无缺失。
    // 剩余 101 构成：ORIG 函数头 41 条 `jmp <T>` else-if 回跳岛（4.4 jump-
    // threading 消除，N121 封顶）+ 上述寄存器复制 + `test/je`↔`xor/test/jne`
    // 守卫极性（P292/N122）+ EH dtor epilogue 合并差（p167 332/332 CLEAR 已证）。
    if (!loadRDARScriptFile(g_szCharacterScriptBaseDirectory, path))
        return false;

    script->Clear();
    script->m_scriptName = path;
    getDirectoryPath(script->m_scriptPath, "%s%s",
                     g_szCharacterScriptBaseDirectory, path);

    // ---- ORIG 函数级槽（0x08a0ce80..0x08a0ced4）：token/str/值暂存/指针游标
    // 全部函数作用域共享；growtype N / awakening 分支为 5 指针直赋
    // （0x08a0d155.. 逐例 `mov 0x8(%ebp),%eax; add $K; mov %eax,-0x48/-0x44/
    // -0x40/-0x3c/-0x38`），不合并、不算下标。 ----
    std::string token;       // ORIG -0xcc
    std::string str;         // ORIG -0xd0（job 名 / blood 串复用）
    int value = 0;           // ORIG -0xd4（ScanInt(int*) 值暂存：width/jump/
                             // skill 第二列/hit int 共用）
    float fvalue = 0.0f;     // ORIG -0xd8（ScanFloat/ScanTypeOrFloat 共用暂存）
    bool ok = false;         // ORIG -0xd9（ScanInt(int*)/ScanTypeOr* 的 got）

    STRDCharacterStatus* curStatus = script->m_status;             // -0x48
    std::vector<std::pair<int, int> >* curSkills = &script->m_skillList[0]; // -0x44
    CharacterEtcStats* curEtc = &script->m_etcStats[0];                    // -0x40
    // ORIG 0x08a0ced7/ce de：curSecond/curAwaken 初值 0；仅 [initial value]/
    // [growtype N] 落位（初值分支不重置 curAwaken）。[awakening 1/2/name] 先
    // 于二者出现属畸形输入（ORIG 空指针解引用），忠实保留。
    STCharacterScript::STSecondGrowTypeData* curSecond = 0;                // -0x3c
    std::string* curAwaken = 0;                                            // -0x38

    for (;;)
    {
        // [bigfix] ORIG 0x08a0cf6e..0x08a0cf78：`call ScanType; xor $0x1,%eax;
        // test %al,%al; jne <exit>`（P164：外层 for(;;)+否定 break 复现 4.1
        // 头部条件块的 xor 物化；while(ScanType) 4.4 恒产 `test;je` 无 xor）。
        if (!ScanType(token, true))
            break;

        if (token == "[job]")
        {
            if (!ScanStr(&str))
                return false;
            // [bigfix-enum] ORIG 0x08a0cfcd `mov 0x8(%ebp),%edx; mov %eax,(%edx)`
            // LHS-first ⇒ 目标字段与 callee 返回同型（enum，P211/P254）。
            script->m_job = static_cast<ENUM_CHARACTERJOB>(getCharacterJob(str.c_str()));
        }
        else if (token == "[growtype name]")
        {
            // ORIG 0x08a0d020 `cmpl $5; setle` ⇒ i <= 5（P2）
            for (int i = 0; i <= 5; ++i)
                if (!ScanStr(&script->m_growtypeName[i]))
                    return false;
        }
        else if (token == "[width]")
        {
            // ORIG ScanInt(int*) + 值槽 -0xd4 中转（非直接 &field）
            if (ScanInt(&value) == false)
                return false;
            script->m_width = value;
            if (ScanInt(&value) == false)
                return false;
            script->m_height = value;
        }
        else if (token == "[jump speed]")
        {
            if (ScanInt(&value) == false)
                return false;
            script->m_jumpSpeed = value;
        }
        else if (token == "[initial value]")
        {
            curStatus = script->m_status;
            curSkills = &script->m_skillList[0];
            curEtc = &script->m_etcStats[0];
            curSecond = &script->m_secondGrow[0];
        }
        else if (token == "[growtype 1]")
        {
            curStatus = &script->m_status[1];
            curSkills = &script->m_skillList[1];
            curEtc = &script->m_etcStats[1];
            curSecond = &script->m_secondGrow[1];
            curAwaken = &script->m_awakeningName[0][0];
        }
        else if (token == "[growtype 2]")
        {
            curStatus = &script->m_status[2];
            curSkills = &script->m_skillList[2];
            curEtc = &script->m_etcStats[2];
            curSecond = &script->m_secondGrow[2];
            curAwaken = &script->m_awakeningName[1][0];
        }
        else if (token == "[growtype 3]")
        {
            curStatus = &script->m_status[3];
            curSkills = &script->m_skillList[3];
            curEtc = &script->m_etcStats[3];
            curSecond = &script->m_secondGrow[3];
            curAwaken = &script->m_awakeningName[2][0];
        }
        else if (token == "[growtype 4]")
        {
            curStatus = &script->m_status[4];
            curSkills = &script->m_skillList[4];
            curEtc = &script->m_etcStats[4];
            curSecond = &script->m_secondGrow[4];
            curAwaken = &script->m_awakeningName[3][0];
        }
        else if (token == "[growtype 5]")
        {
            curStatus = &script->m_status[5];
            curSkills = &script->m_skillList[5];
            curEtc = &script->m_etcStats[5];
            curSecond = &script->m_secondGrow[5];
            curAwaken = &script->m_awakeningName[4][0];
        }
        else if (token == "[growtype 6]")
        {
            curStatus = &script->m_status[6];
            curSkills = &script->m_skillList[6];
            curEtc = &script->m_etcStats[6];
            curSecond = &script->m_secondGrow[6];
            curAwaken = &script->m_awakeningName[5][0];
        }
        else if (token == "[awakening 1]")
        {
            curStatus = curSecond->m_statusA;           // +0x00
            curSkills = &curSecond->m_skills[0];         // +0xb0
            curEtc = &curSecond->m_etc[0];               // +0xc8
        }
        else if (token == "[awakening 2]")
        {
            curStatus = &curSecond->m_statusA[1];           // +0x57
            curSkills = &curSecond->m_skills[1];         // +0xbc
            curEtc = &curSecond->m_etc[1];               // +0xe4
        }
        else if (token == "[awakening name]")
        {
            if (!ScanStr(curAwaken))
                return false;
            if (!ScanStr(curAwaken + 1))
                return false;
        }
        else if (token == "[awakening skill]")
        {
            std::string t;                // ORIG -0xe0
            bool isType = false;          // ORIG -0xe1
            int v;                        // ORIG -0xe8（ScanTypeOrInt 返回值）
            curSkills->clear();
            for (;;)
            {
                v = ScanTypeOrInt(isType, t, ok, true);
                if (ok != true)
                    return false;
                if (isType)
                {
                    if (t == "[/awakening skill]")
                        break;
                    return false;
                }
                if (ScanInt(&value) == false)
                    return false;
                // [b28d-solo-ScriptDepsA] ORIG pair converting ctor `C1IRiS2_`
                //（int&,int&）⇒ 直构（P192）。
                curSkills->push_back(std::pair<int, int>(v, value));
            }
        }
        else if (token == "[HP MAX]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_hpMax = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[MP MAX]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_mpMax = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[physical attack]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_physAttack =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[physical defense]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_physDefense =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[magical attack]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_magAttack =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[magical defense]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_magDefense =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[fire resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[0] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[water resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[1] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[dark resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[2] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[light resistance]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_elemResist[3] =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[inventory limit]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_inventoryLimit = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[HP regen speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_hpRegen =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[MP regen speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_mpRegen =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[move speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_moveSpeed = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[attack speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_attackSpeed =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[cast speed]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_castSpeed =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[hit recovery]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_hitRecovery =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[jump power]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_jumpPower =
                static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
        }
        else if (token == "[weight]")
        {
            if (!ScanFloat(&fvalue)) return false;
            curStatus->m_weight = GetRoundInt(10.0f * fvalue);
        }
        else if (token == "[skill]")
        {
            std::string t;                // ORIG -0xf0
            bool isType = false;          // ORIG -0xf1
            int v;                        // ORIG -0xf8
            curSkills->clear();
            for (;;)
            {
                v = ScanTypeOrInt(isType, t, ok, true);
                if (ok != true)
                    return false;
                if (isType)
                {
                    if (t == "[/skill]")
                        break;
                    return false;
                }
                if (ScanInt(&value) == false)
                    return false;
                curSkills->push_back(std::pair<int, int>(v, value));
            }
        }
        else if (token == "[category]")
        {
            curEtc->m_category.clear();
            // ORIG 0x08a0dd??：clear（`mov -0x40,%eax; call`）先于 t/isType 的
            // string ctor + movb $0（-0xf8/-0xf9）——ORIG 把 clear 写在声明前。
            std::string t;                // ORIG -0xf8
            bool isType = false;          // ORIG -0xf9（折入 t 槽）
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    // [b28d-solo-ScriptDepsA] P133：ORIG 0x08a0de4f 用否定式
                    // 守卫（`_ZStne`）；p167 MISSING ne / EXTRA eq。
                    if (t != "[/category]")
                        return false;
                    break;
                }
                const int cat = getObjectCategory(t);   // 传值 → 拷贝临时
                if (cat != 0x2c)
                    // ORIG 0x08a0dde0：push_backEOi（右值临时 -0xb4）。
                    curEtc->m_category.push_back(static_cast<int>(cat));
            }
        }
        else if (token == "[module damage rate]")
        {
            // ORIG 0x08a0df61 `cmpl $3; setle` ⇒ i <= 3（P2）
            for (int i = 0; i <= 3; ++i)
            {
                if (!ScanFloat(&fvalue)) return false;
                curEtc->m_moduleDamageRate[i] = fvalue;
            }
        }
        else if (token == "[waiting motion]")   { if (!ScanStr(&script->m_motionWaiting)) return false; }
        else if (token == "[move motion]")      { if (!ScanStr(&script->m_motionMove)) return false; }
        else if (token == "[sit motion]")       { if (!ScanStr(&script->m_motionSit)) return false; }
        else if (token == "[damage motion 1]")  { if (!ScanStr(&script->m_motionDamage[0])) return false; }
        else if (token == "[damage motion 2]")  { if (!ScanStr(&script->m_motionDamage[1])) return false; }
        else if (token == "[down motion]")      { if (!ScanStr(&script->m_motionDown)) return false; }
        else if (token == "[overturn motion]")  { if (!ScanStr(&script->m_motionOverturn)) return false; }
        else if (token == "[jump motion]")      { if (!ScanStr(&script->m_motionJump)) return false; }
        else if (token == "[jumpattack motion]"){ if (!ScanStr(&script->m_motionJumpattack)) return false; }
        else if (token == "[rest motion]")      { if (!ScanStr(&script->m_motionRest)) return false; }
        else if (token == "[throw motion 1-1]") { if (!ScanStr(&script->m_throwMotion1[0])) return false; }
        else if (token == "[throw motion 1-2]") { if (!ScanStr(&script->m_throwMotion2[0])) return false; }
        else if (token == "[throw motion 2-1]") { if (!ScanStr(&script->m_throwMotion1[1])) return false; }
        else if (token == "[throw motion 2-2]") { if (!ScanStr(&script->m_throwMotion2[1])) return false; }
        else if (token == "[throw motion 3-1]") { if (!ScanStr(&script->m_throwMotion1[2])) return false; }
        else if (token == "[throw motion 3-2]") { if (!ScanStr(&script->m_throwMotion2[2])) return false; }
        // ORIG 0x08a0e399/0x08a0e3de：4-1/4-2 在 3-2 之后、dash 之前。
        else if (token == "[throw motion 4-1]") { if (!ScanStr(&script->m_throwMotion1[3])) return false; }
        else if (token == "[throw motion 4-2]") { if (!ScanStr(&script->m_throwMotion2[3])) return false; }
        else if (token == "[dash motion]")      { if (!ScanStr(&script->m_motionDash)) return false; }
        else if (token == "[dashattack motion]"){ if (!ScanStr(&script->m_motionDashattack)) return false; }
        else if (token == "[getitem motion]")   { if (!ScanStr(&script->m_motionGetitem)) return false; }
        else if (token == "[buff motion]")      { if (!ScanStr(&script->m_motionBuff)) return false; }
        else if (token == "[back motion]")      { if (!ScanStr(&script->m_motionBack)) return false; }
        else if (token == "[simple rest motion]")  { if (!ScanStr(&script->m_motionSimpleRest)) return false; }
        else if (token == "[simple move motion]")  { if (!ScanStr(&script->m_motionSimpleMove)) return false; }
        // ORIG 链序：attack motion(0x08a0e598) → ghost motion(0x08a0e69d) →
        // etc motion(0x08a0e7a2)；各块 t/isType 独立槽。
        else if (token == "[attack motion]")
        {
            std::string t;                // ORIG -0x100
            bool isType = false;          // ORIG -0x101
            script->m_attackMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/attack motion]")
                        break;
                    return false;
                }
                script->m_attackMotion.push_back(t);
            }
        }
        else if (token == "[ghost motion]")
        {
            std::string t;                // ORIG -0x108
            bool isType = false;          // ORIG -0x109
            script->m_ghostMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/ghost motion]")
                        break;
                    return false;
                }
                script->m_ghostMotion.push_back(t);
            }
        }
        else if (token == "[etc motion]")
        {
            std::string t;                // ORIG -0x110
            bool isType = false;          // ORIG -0x111
            script->m_etcMotion.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/etc motion]")
                        break;
                    return false;
                }
                script->m_etcMotion.push_back(t);
            }
        }
        else if (token == "[waiting action]")     { if (!ScanStr(&script->m_actionSet.m_waiting)) return false; }
        else if (token == "[move action]")        { if (!ScanStr(&script->m_actionSet.m_move)) return false; }
        else if (token == "[dash action]")        { if (!ScanStr(&script->m_actionSet.m_dash)) return false; }
        else if (token == "[sit action]")         { if (!ScanStr(&script->m_actionSet.m_sit)) return false; }
        else if (token == "[damage action 1]")    { if (!ScanStr(&script->m_actionSet.m_damage[0])) return false; }
        else if (token == "[damage action 2]")    { if (!ScanStr(&script->m_actionSet.m_damage[1])) return false; }
        else if (token == "[down action]")        { if (!ScanStr(&script->m_actionSet.m_down)) return false; }
        else if (token == "[overturn action]")    { if (!ScanStr(&script->m_actionSet.m_overturn)) return false; }
        else if (token == "[jump action]")        { if (!ScanStr(&script->m_actionSet.m_jump)) return false; }
        else if (token == "[jumpattack action]")  { if (!ScanStr(&script->m_actionSet.m_jumpattack)) return false; }
        else if (token == "[attack action]" || token == "[action list]")
        {
            std::string t;                // ORIG -0x11c
            bool isType = false;          // ORIG -0x11d
            script->m_actionSet.m_attackList.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    // ORIG 0x08a0ec0d..：先 (token==[attack action] &&
                    // t==[/attack action])，否则再 (token==[action list] &&
                    // t==[/action list])，皆不中 → 失败。ORIG 具名 endTag 由
                    // 4.1 驻 %eax（mov $1/$0 物化，无栈槽）；4.4 具名 bool 必落
                    // -0x29 并把指针组压深 4 字节（§N82 主源，实测）。等价改写：
                    // 条件直接进 if（短路跳转形，无物化槽），命中 break / 失败
                    // return false，与 “a||b → break” 真值表一致。
                    if (token == "[attack action]" &&
                        t == "[/attack action]")
                        break;
                    if (token == "[action list]" &&
                        t == "[/action list]")
                        break;
                    return false;
                }
                script->m_actionSet.m_attackList.push_back(t);
            }
        }
        else if (token == "[etc action]")
        {
            std::string t;                // ORIG -0x124
            bool isType = false;          // ORIG -0x125
            script->m_actionSet.m_etc.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/etc action]")
                        break;
                    return false;
                }
                script->m_actionSet.m_etc.push_back(t);
            }
        }
        else if (token == "[last action]")      { if (!ScanStr(&script->m_lastAction)) return false; }
        else if (token == "[proc action]")      { if (!ScanStr(&script->m_procAction)) return false; }
        else if (token == "[jumpattack info]")  { if (!ScanStr(&script->m_jumpattackInfo)) return false; }
        else if (token == "[dashattack info]")  { if (!ScanStr(&script->m_dashattackInfo)) return false; }
        else if (token == "[attack info]")
        {
            std::string t;                // ORIG -0x12c
            bool isType = false;          // ORIG -0x12d
            script->m_attackInfo.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/attack info]")
                        break;
                    return false;
                }
                script->m_attackInfo.push_back(t);
            }
        }
        else if (token == "[etc attack info]")
        {
            std::string t;                // ORIG -0x130
            bool isType = false;          // ORIG -0x131
            script->m_etcAttackInfo.clear();
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/etc attack info]")
                        break;
                    return false;
                }
                script->m_etcAttackInfo.push_back(t);
            }
        }
        else if (token == "[weapon wav]")
        {
            std::string t;                // ORIG -0x138
            bool isType = false;          // ORIG -0x139
            WeaponInfo info;              // ORIG -0x190（t 后构造、先析构）
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    // ORIG 0x08a0f169 `jne 08a0f194`：push_back(info) 块在
                    // !isType 块之后；负向早返形（P222）实测我方 INSERT +7
                    //（break 重排），回退维持正向形（块序差 N 封顶）。
                    if (t == "[/weapon wav]")
                    {
                        script->m_weaponWav.push_back(info);
                        break;
                    }
                    return false;
                }
                info.m_wavs.push_back(t);
            }
        }
        else if (token == "[weapon hit info]")
        {
            std::string t;                // ORIG -0x140
            bool isType = false;          // ORIG -0x141
            unsigned int idx = 0;
            for (;;)
            {
                if (!ScanTypeOrStr(isType, t, true))
                    return false;
                if (isType)
                {
                    if (t == "[/weapon hit info]")
                        break;
                    return false;
                }
                if (script->m_weaponWav.size() <= idx)
                {
                    // [b28d-solo-ScriptDepsA] ORIG 0x08a3eae6 段：value-init 临时
                    // 就地构造进 resize 按值形参（无 C1ERKS 参数副本，D1 少一枚）。
                    script->m_weaponWav.resize(idx + 1, WeaponInfo());
                }
                WeaponInfo* w = &script->m_weaponWav[idx];
                if (t == "[blow]") w->m_hitType = 0;
                else if (t == "[cut]") w->m_hitType = 1;
                else if (t == "[burn]") w->m_hitType = 2;
                else if (t == "[etc]") w->m_hitType = 3;
                else return false;
                if (!ScanStr(&str)) return false;
                if (str == "[no blood]") w->m_blood = 0;
                else if (str == "[blood]") w->m_blood = 1;
                else return false;
                if (ScanInt(&value) == false) return false;
                w->m_field14 = value;
                if (!ScanFloat(&fvalue)) return false;
                w->m_rate18 = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_rate1c = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_rate20 = fvalue;
                ++idx;
            }
        }
        else if (token == "[weapon skill info]")
        {
            std::string t;                // ORIG -0x148
            bool isType = false;          // ORIG -0x149
            unsigned int idx = 0;
            while (true)
            {
                fvalue = ScanTypeOrFloat(isType, t, ok, true);
                if (ok != true) return false;
                if (isType)
                {
                    if (t == "[/weapon skill info]") break;
                    return false;
                }
                if (script->m_weaponWav.size() <= idx)
                {
                    script->m_weaponWav.resize(idx + 1, WeaponInfo());
                }
                WeaponInfo* w = &script->m_weaponWav[idx];
                w->m_skillRates[0] = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_skillRates[1] = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_skillRates[2] = fvalue;
                if (!ScanFloat(&fvalue)) return false;
                w->m_skillRates[3] = fvalue;
                ++idx;
            }
        }
        else if (token == "[body image path]")
        {
            std::string t;                // ORIG -0x150
            // ORIG `xor $0x1,%eax; test; je` = 否定守卫（!ScanStr→失败）。
            // m_job 为 ENUM_CHARACTERJOB（P254 实证 0x08a0f951 中转），直接作
            // map 键，无需 reinterpret 视图。
            if (!ScanStr(&t))
                return false;
            s_bodyImagePath[script->m_job] = t;
        }
        else if (token == "[upgrade weapon attack power rate]")
        {
            // [bigfix] ORIG 0x08a0f946 `mov -0x18(%ebp),%eax; cmp $0x5,%eax; ja`
            // 两段寄存器形 ⇒ 局部声明 int + 显式无符号比较（P310）。
            int col = 0;                  // ORIG -0x18
            float rate = 0.0f;            // ORIG -0x14（ScanTypeOrFloat 返回）
            float val = 0.0f;             // ORIG -0x154
            std::string t;                // ORIG -0x158
            bool isType = false;          // ORIG -0x159
            while (true)
            {
                rate = ScanTypeOrFloat(isType, t, ok, true);
                if (ok != true) return false;
                if (isType)
                {
                    if (t == "[/upgrade weapon attack power rate]") break;
                    return false;
                }
                if (!ScanFloat(&val)) return false;
                if ((unsigned int)col <= 5)  // ORIG 0x08a0f949 `cmp $5; ja`（无符号）
                {
                    upgradeWeaponAttackPowerRate_[script->m_job][col][0] = rate;
                    upgradeWeaponAttackPowerRate_[script->m_job][col][1] = val;
                }
                col = col + 1;
            }
        }
        else if (token == "[weapon durability decrease rate]")
        {
            // ORIG 0x08a0fa41 `cmpl $5; setle` ⇒ i <= 5
            for (int i = 0; i <= 5; ++i)
            {
                if (!ScanFloat(&fvalue)) return false;
                g_weaponDurabilityDecreaseRate_[script->m_job][i] = fvalue;
            }
        }
        else
        {
            // ORIG 0x08a0fa51..0x08a0fac9：i = 0..0x10（`cmpl $0x10; setle`）
            // 线性比 17 项 ACTIVESTATUS_TAG，命中即 ScanFloat+×10 写
            // curStatus->m_activeResist[i]（+0x18+2i），**无 break** 续扫。
            for (int i = 0; i <= 0x10; ++i)
            {
                if (token == ACTIVESTATUS_TAG[i])
                {
                    if (!ScanFloat(&fvalue)) return false;
                    curStatus->m_activeResist[i] =
                        static_cast<unsigned short>(GetRoundInt(10.0f * fvalue));
                }
            }
        }
    }
    return true;
