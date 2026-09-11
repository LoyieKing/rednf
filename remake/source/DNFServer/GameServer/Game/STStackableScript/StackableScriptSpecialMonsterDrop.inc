// [b28d-solo-STStackableScript] AE（172=172）：local_d 由 char 改 bool（P136/P32：
// bool 源→bool 字段直存，char 源恒多 `cmpb $0; setne`），`local_15 != true`
// 改 `!local_15`（ORIG 089ae947 `movzbl; xor $1; test; je` = N122 取反形）。
bool StackableScriptSpecialMonsterDrop(STStackableScript* script,
                                       std::string str, const char* path)
{
    // ORIG 0x89ae906（Ghidra C 逐句转写）：
    // 局部：bool local_15、std::string local_14、char local_d。
    // "[special monster drop]"：local_15=false → ScanInt(&local_15) 返回 int iVar3 →
    //   local_d = (iVar3 != 0)；若 local_15 != true 返回 false；否则
    //   m_specialDrop.m_enable = local_d。
    // "[difficulty for extra drop]" / "[dungeon type for extra drop]"：循环 ScanStr，
    //   命中标签置对应 flag（+0x106..+0x10a 难度 / +0x10b..+0x10d 类型），
    //   "[all]" memset 5 / 3 字节。其余输入返回 true。
    (void)path;
    std::string local_14;
    bool local_15;
    bool local_d;
    if (str == "[special monster drop]")
    {
        local_15 = false;
        local_d = ScanInt(&local_15) != 0;
        if (!local_15)
        {
            return false;
        }
        script->m_specialDrop.m_enable = local_d;
    }
    else if (str == "[difficulty for extra drop]")
    {
        while (ScanStr(&local_14))
        {
            if (local_14 == "[easy]")
            {
                script->m_specialDrop.m_difficulty[0] = 1;
            }
            else if (local_14 == "[medium]")
            {
                script->m_specialDrop.m_difficulty[1] = 1;
            }
            else if (local_14 == "[hard]")
            {
                script->m_specialDrop.m_difficulty[2] = 1;
            }
            else if (local_14 == "[ultimate]")
            {
                script->m_specialDrop.m_difficulty[3] = 1;
            }
            else if (local_14 == "[hero]")
            {
                script->m_specialDrop.m_difficulty[4] = 1;
            }
            else if (local_14 == "[all]")
            {
                memset(script->m_specialDrop.m_difficulty, 1, 5);
            }
        }
    }
    else if (str == "[dungeon type for extra drop]")
    {
        while (ScanStr(&local_14))
        {
            if (local_14 == "[normal]")
            {
                script->m_specialDrop.m_dungeonType[0] = 1;
            }
            else if (local_14 == "[ancient]")
            {
                script->m_specialDrop.m_dungeonType[1] = 1;
            }
            else if (local_14 == "[dimension]")
            {
                script->m_specialDrop.m_dungeonType[2] = 1;
            }
            else if (local_14 == "[all]")
            {
                memset(script->m_specialDrop.m_dungeonType, 1, 3);
            }
        }
    }
    return true;
}