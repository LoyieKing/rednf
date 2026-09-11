// ORIG importDisjointScript T 0x088e4d88（报告全文转写）。依赖已由既有 TU 提供：
// DisjointScript::clear(CtorProducers_2.cpp)/getCharacterJob(SyncScriptDeps.cpp)/
// createLotteryInfo(CStackableItem.cpp)/DisjointSkillScript 生命周期(CtorProducers_2.cpp)。
bool importDisjointScript(DisjointScript* script, const char* path)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：542/529，gap=13，
    // p167 65/65 CLEAR）：本波修 [additional result] 的 push_back 右值内联实参
    // （ORIG 0x088e5198 EOi）+ [avatar disjoint info] 终止守卫改 `==`+break
    // （P133）。残差=巨件帧/槽平移 + map/vector 模板实例内联差。
    if (!loadRDARScriptFile("", path))
        return false;

    script->clear();

    std::string token;
    std::string str;
    int value = 0;
    bool ok = false;

    // [P292/P283] ORIG 循环头 `xor $0x1,%eax; test %al,%al; jne/je` ⇒ 原码
    // 统一 for(;;)+`if (!cond) break;`。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[cube index]")
        {
            ScanStr(&str);
            const int elem = getElement(str.c_str());
            // [P183] ORIG 0x088e4e3d/0x088e4e73 两处各自重算
            // `3*elem*4 + script`（add/add/shl/add 链）⇒ 禁引用缓存，逐点重取。
            script->m_cubeIndexVecs[elem].clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_cubeIndexVecs[elem].push_back(value);
            }
        }
        else if (token == "[legacy info]")
        {
            const int idx = ScanInt(static_cast<bool*>(0));
            std::vector<int> counts;
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                counts.push_back(value);
            }
            // [P171/P137] ORIG 0x088e4ef6 仅 `sub $0x1,%eax; cmp $0x3,%eax; jg`
            // 单一有符号上界（无下界守卫，idx=0 照抄越界行为）⇒ 写 idx-1 <= 3。
            if (idx - 1 <= 3)
                createLotteryInfo(&script->m_legacyInfo[idx - 1], &counts);
        }
        else if (token == "[skill info]")
        {
            script->m_skills.clear();
            script->m_skillSelect = ScanInt(static_cast<bool*>(0));
            DisjointSkillScript entry;
            for (;;)
            {
                if (!ScanStr(&str))
                    break;
                entry.m_field00 = getCharacterJob(str.c_str());
                entry.m_field04 = ScanInt(static_cast<bool*>(0));
                entry.m_field08 = ScanInt(static_cast<bool*>(0));
                ScanStr(&str);
                entry.m_field0c = getElement(str.c_str());
                script->m_skills.push_back(entry);
            }
        }
        else if (token == "[skill prob]")
        {
            script->m_fieldCC = ScanFloat(static_cast<bool*>(0));
            script->m_skillProbD0 = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[cube creation const]")
        {
            script->m_creationConst = ScanInt(static_cast<bool*>(0));
            for (int i = 0; i < 6; ++i)
                script->m_creationRate[i] = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[additional result]")
        {
            for (int g = 0; g < 6; ++g)
            {
                // m_additional 为 DisjointScript 匿名嵌套结构（P146 保持 ._381 mangled），
                // 不可具名引用，直接成员访问。
                script->m_additional[g].m_count = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_additional[g].m_items.clear();
                script->m_additional[g].m_items.reserve(
                    script->m_additional[g].m_count);
                for (int j = 0; j < script->m_additional[g].m_count; ++j)
                {
                    // [b28d-solo-ScriptDepsA] ORIG 0x088e5198 为 push_backEOi
                    //（右值）：源码把 ScanInt 调用内联进实参（4.1 为右值建临时槽
                    // -0x48 并传址）。
                    script->m_additional[g].m_items.push_back(
                        ScanInt(static_cast<bool*>(0)));
                }
            }
        }
        else if (token == "[additional result const]")
        {
            for (int g = 0; g < 6; ++g)
            {
                script->m_additionalWeight[g] = ScanFloat(static_cast<bool*>(0));
                // [P47/P222] ORIG 0x088e5232 为 `jne <非零>; jp <非零>`（== 语义，
                // 零分支直落）；写 `!=` 时 4.4 反产 `jp; je`（非零直落）⇒ 翻正极性。
                if (script->m_additionalWeight[g] == 0.0f)
                {
                    script->m_additionalRate[g] = 0.0f;
                    script->m_additionalConst[g].m_item = 0;
                    script->m_additionalConst[g].m_count = 0;
                }
                else
                {
                    script->m_additionalRate[g] = ScanFloat(static_cast<bool*>(0));
                    script->m_additionalConst[g].m_item = ScanInt(static_cast<bool*>(0));
                    script->m_additionalConst[g].m_count = ScanInt(static_cast<bool*>(0));
                }
            }
        }
        else if (token == "[avatar disjoint info]")
        {
            // [b28d-solo-ScriptDepsA] ORIG 0x088e50xx 用 eq（`==`+break）而非
            // 短路 `!=`（p167：MISSING eq / EXTRA ne）。
            for (;;)
            {
                if (!ScanType(token, true))
                    break;
                if (token == "[/avatar disjoint info]")
                    break;
                const int job = getCharacterJob(token.c_str());
                if (job == 0xb)
                    return false;
                for (int grade = 1; grade <= 3; ++grade)
                {
                    for (int slot = 0; slot < 5; ++slot)
                    {
                        // [P0/P171] ORIG 0x088e5397 段：ScanInt 结果入 %edi 直存
                        //（无具名局部）；索引为 [job][grade][slot]（grade 1..3 直接
                        // 使用，基址 +0xd4 = 0x088e53cc `0x4(%ecx,%eax,4)` 实证）。
                        script->m_avatarDisjoint[job][grade][slot] = ScanInt(&ok);
                        if (!ok)
                            return false;
                    }
                }
                ScanInt(&ok);
            }
        }
        else if (token == "[avatar disjoint for parts info]")
        {
            for (int grade = 1; grade <= 3; ++grade)
            {
                for (int parts = 0; parts < 10; ++parts)
                {
                    // [P171] ORIG 0x088e5d05 段（clear 同址式 0x08375d05）：
                    // 索引 = 10*grade + parts + 0x110，grade 1..3 直接用 ⇒ 基址 +0x444。
                    script->m_avatarPartsDisjoint[grade][parts] = ScanInt(&ok);
                    if (!ok)
                        return false;
                }
            }
            ScanInt(&ok);
        }
    }
    return true;
}