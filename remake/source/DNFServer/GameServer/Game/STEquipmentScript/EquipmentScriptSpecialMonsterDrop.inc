bool EquipmentScriptSpecialMonsterDrop(STEquipmentScript* script,
                                       std::string str, const char* path)
{
    // [b28d-solo-STEquipmentScript] ORIG 0x08991856（与 StackableScriptSpecialMonsterDrop
    // 0x89ae906 同构，目标为 STItemScript::m_specialDrop +0x105）：
    // ①函数头只构造一个 string 局部（-0x10），各分支复用（P28/P212 全头部声明）；
    // ②禁 `STSpecialMonsterDrop& drop` 引用缓存（P183）——每个使用点直写
    //   `script->m_specialDrop.xxx`，才复现 ORIG `add $0x105,%eax` + `mov %dl,0x105(%eax)`。
    (void)path;
    std::string token;
    if (str == "[special monster drop]")
    {
        bool value = false;
        bool ok = ScanInt(&value) != 0;
        if (value == false)
        {
            return false;
        }
        script->m_specialDrop.m_enable = ok;   // P136/P32：bool 字段直存（ORIG movzbl→mov %dl）
    }
    else if (str == "[difficulty for extra drop]")
    {
        while (ScanStr(&token))
        {
            if (token == "[easy]")
            {
                script->m_specialDrop.m_difficulty[0] = 1;
            }
            else if (token == "[medium]")
            {
                script->m_specialDrop.m_difficulty[1] = 1;
            }
            else if (token == "[hard]")
            {
                script->m_specialDrop.m_difficulty[2] = 1;
            }
            else if (token == "[ultimate]")
            {
                script->m_specialDrop.m_difficulty[3] = 1;
            }
            else if (token == "[hero]")
            {
                script->m_specialDrop.m_difficulty[4] = 1;
            }
            else if (token == "[all]")
            {
                memset(script->m_specialDrop.m_difficulty, 1,
                       sizeof(script->m_specialDrop.m_difficulty));
            }
        }
    }
    else if (str == "[dungeon type for extra drop]")
    {
        while (ScanStr(&token))
        {
            if (token == "[normal]")
            {
                script->m_specialDrop.m_dungeonType[0] = 1;
            }
            else if (token == "[ancient]")
            {
                script->m_specialDrop.m_dungeonType[1] = 1;
            }
            else if (token == "[dimension]")
            {
                script->m_specialDrop.m_dungeonType[2] = 1;
            }
            else if (token == "[all]")
            {
                memset(script->m_specialDrop.m_dungeonType, 1,
                       sizeof(script->m_specialDrop.m_dungeonType));
            }
        }
    }
    return true;
}