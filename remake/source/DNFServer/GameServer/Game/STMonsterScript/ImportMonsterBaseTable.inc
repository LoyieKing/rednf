bool ImportMonsterBaseTable(STMonsterScript* script, const char* path)
{
    if (!loadRDARScriptFile(g_szMonsterScriptBaseDirectory, path))
        return false;                          // +0x1f

    int level = 0;                             // 初始化执行序实证：level→ok→param C1→row
    bool ok = false;
    MonsterBaseParameter param;
    int row = 0;                               // 目标行号
    while (true)
    {
        level = ScanInt(&ok);                  // +0x53 首列 level；!ok → 正常结束
        if (!ok)
            break;                             // +0x2d4 跳出 → 循环外 mov eax,1
        param.m_value[0] = level;              // +0xac
        // +0x7b..0x87 写入序列按 ORIG：[3][4][5][6] → [1][2] → [7]..[12]
        param.m_value[3] = ScanInt(static_cast<bool*>(0));         // -0xe0
        param.m_value[4] = ScanInt(static_cast<bool*>(0));         // -0xdc
        param.m_value[5] = ScanInt(static_cast<bool*>(0));         // -0xd8
        param.m_value[6] = ScanInt(static_cast<bool*>(0));         // -0xd4
        param.m_value[1] = ScanInt(static_cast<bool*>(0));         // -0xe8
        param.m_value[2] = ScanInt(static_cast<bool*>(0));         // -0xe4
        param.m_value[7] = ScanInt(static_cast<bool*>(0));         // -0xd0
        param.m_value[8] = ScanInt(static_cast<bool*>(0));         // -0xcc
        param.m_value[9] = ScanInt(static_cast<bool*>(0));         // -0xc8
        param.m_value[10] = ScanInt(static_cast<bool*>(0));        // -0xc4
        param.m_value[11] = ScanInt(static_cast<bool*>(0));        // -0xc0
        param.m_value[12] = ScanInt(static_cast<bool*>(0));        // -0xbc
        for (int i = 0; i <= 3; ++i)           // +0x160 disp=+4、idx=i+0xc（m_value[13+i]）
            param.m_grid[3][i] = ScanInt(static_cast<bool*>(0));
        for (int i = 0; i <= 3; ++i)           // +0x191 disp=+4、idx=i+0x10（m_value[17+i]）
            param.m_grid[4][i] = ScanInt(static_cast<bool*>(0));
        param.m_value[21] = ScanInt(static_cast<bool*>(0));        // -0x98（param+0x54）
        param.m_value[22] = ScanInt(static_cast<bool*>(0));        // -0x94
        param.m_value[23] = ScanInt(static_cast<bool*>(0));        // -0x90

        if (row == script->m_baseParameters.size())            // ORIG seta 无符号族（P209 载寄存器）
            script->m_baseParameters.push_back(param);            // +0x1f4 追加
        else
        {
            if (row > script->m_baseParameters.size())            // +0x22b 行号跳跃（无符号）
                script->m_baseParameters.resize(row + 1, MonsterBaseParameter());
            script->m_baseParameters[row] = param;                // +0x29e 共享覆写 rep movsl
        }
        ++row;                                 // +0x2cb
    }
    return true;                               // +0x2d4
