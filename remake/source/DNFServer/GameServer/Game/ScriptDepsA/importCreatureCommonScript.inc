// ORIG importCreatureCommonScript T 0x08a383f6（报告 ## 汇编 + ## 反编译 C 全文
// 转写）。伤害表同步进 damageTable；module/assault 段除数为 _DAT_08e23930=100.0f。
bool importCreatureCommonScript(CreatureCommonScript* script, const char* path,
                                std::vector<int>* damageTable)
{
    // [b28d-solo-ScriptDepsA 判词] N82/N89 封顶（§6.2：415/405，gap=10，
    // p167 69/69 CLEAR）：本波补 [module damage percent] 的具名临时 string
    // （allocator+str.c_str()+SsC1EPKcRKSaIcE 三件套，ORIG 0x08a3866x）。
    // 残差=巨件帧/槽平移 + hash_map/vector 模板实例内联差。
    if (!loadRDARScriptFile(g_creatureScriptBaseDirectory, path))
        return false;

    script->clear();
    damageTable->clear();

    std::string token;
    std::string str;
    int value = 0;
    bool ok = true;

    // [P292/P283] ORIG 各循环头均 `xor $0x1,%eax; test %al,%al; jne/je`（bool
    // 取反形）⇒ 原码统一为 for(;;)+`if (!cond) break;`，非 while(cond)。
    for (;;)
    {
        if (!ScanType(token, true))
            break;
        if (token == "[attack constant]")
        {
            script->m_attackConstant = ScanFloat(static_cast<bool*>(0));
        }
        else if (token == "[response delay]")
        {
            script->m_responseDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[second per fill point spend]")
        {
            script->m_secondPerFillPointSpend = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[second per fill point charge]")
        {
            script->m_secondPerFillPointCharge = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[MAX fill point]")
        {
            script->m_maxFillPoint = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[fill point to eat feed]")
        {
            script->m_fillPointToEatFeed = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[evolution fade in delay]")
        {
            script->m_evolutionFadeInDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[evolution fade out delay]")
        {
            script->m_evolutionFadeOutDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[evolution delay]")
        {
            script->m_evolutionDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[dying delay]")
        {
            script->m_dyingDelay = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[ambient in room list]")
        {
            script->m_ambientRoomList[0] = ScanInt(static_cast<bool*>(0));
            script->m_ambientRoomList[1] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[ambient in main game]")
        {
            script->m_ambientMainGame[0] = ScanInt(static_cast<bool*>(0));
            script->m_ambientMainGame[1] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[ambient in pvp]")
        {
            script->m_ambientPvp[0] = ScanInt(static_cast<bool*>(0));
            script->m_ambientPvp[1] = ScanInt(static_cast<bool*>(0));
        }
        else if (token == "[damage]")
        {
            script->m_damage.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_damage.push_back(value);
                damageTable->push_back(value);
            }
        }
        else if (token == "[mana]")
        {
            script->m_mana.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                script->m_mana.push_back(value);
            }
        }
        else if (token == "[module damage percent]")
        {
            g_creatureModuleDamageRateMap.clear();
            for (;;)
            {
                // ORIG 0x08a387f6/0x08a38814 两处独立 `xor $0x1` 守卫 ⇒ 两条独立
                // break，非 `ScanStr && …` 短路合并（P133）。
                if (!ScanStr(&str))
                    break;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                // [b28d-solo-ScriptDepsA] ORIG 0x08a3866x 段：allocator+str.c_str()
                // +string(const char*,const allocator&) 三件套 ⇒ 具名临时 string
                // 由 c_str() 复制构造（P130 家族）。
                std::string moduleName(str.c_str());
                const ENUM_MODULE_TYPE type =
                    static_cast<ENUM_MODULE_TYPE>(getModuleTypeFromTag(moduleName));
                // [P252/P0] ORIG 0x08a38890 段 `fstps -0x10(%ebp); mov -0x10(%ebp),%eax;
                // mov %eax,0x4(%esp)` ⇒ 原码有具名 float 局部（槽往返），非内联实参。
                const float rate = static_cast<float>(value) / 100.0f;
                addCreatureModuleRate(type, rate);
            }
        }
        else if (token == "[assault creature damage penalty]")
        {
            g_creatureIndexDamageRateMap.clear();
            for (;;)
            {
                value = ScanInt(&ok);
                if (!ok)
                    break;
                const int idx = value;
                value = ScanInt(&ok);
                if (!ok)
                    break;
                // [P252/P0] ORIG 0x08a38944 段 `fstps -0xc(%ebp)` 先算存槽，
                // operator[] 之后 `mov -0xc(%ebp),%edx; mov %edx,(%eax)` 读回 ⇒
                // 原码具名 float 局部，且先于 map 查找求值。
                const float rate = static_cast<float>(value) / 100.0f;
                g_creatureIndexDamageRateMap[idx] = rate;
            }
        }
        else
        {
            LogManager::logFormat(
                0, "../RDARScriptObject.cpp", __FUNCTION__, 0x2701,
                "script loading failed. | file <%s> tag <%s>",
                path, token.data());
            return false;
        }
    }
    return true;
}