int getMonsterTableIndex(const STMonsterScript* script, int dungeonIdx,
                         int roleType, bool flagA, bool flagB)
{
    int tableIdx = 0;
    if (script->m_abilityTable == 1)          // +0x3bc == 1 → idx 3
        tableIdx = 3;
    if (flagA)
        tableIdx = 3;
    if (roleType == 3)                        // 第 3 参（[ebp+0x10]）== 3 → idx 2
        tableIdx = 2;
    if (isNamedMonster(script->m_index))      // 命名怪 → idx 1
        tableIdx = 1;
    // ORIG +0x228..0x240：b≤1999 时查 d；b∈(1999,2002] 置 4（短路或形）
    if ((dungeonIdx > 1999 && dungeonIdx <= 2002) || flagB)
        tableIdx = 4;
    return tableIdx;
