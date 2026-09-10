int importNoviceCommandScript(const char* path, stNoviceCommand& out)
{
    if (!loadRDARScriptFile("", path))
        return 0x11cc;
    std::string token;
    for (int i = 0; i <= 0xa; ++i)   // ORIG cmpl $0xa/setle（P2）
        out.m_slots[i].clear();
    std::vector<int> row;
    int val;                          // ORIG 单槽 -0x60：各块共用
    for (;;)
    {
        if (!ScanType(token, true))
            break;                    // ORIG 出口岛在函数尾（共享 leave）
        if (token == "[gunner]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_GUNNER].push_back(row);
            }
        }
        else if (token == "[fighter]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_FIGHTER].push_back(row);
            }
        }
        else if (token == "[swordman]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_SWORDMAN].push_back(row);
            }
        }
        else if (token == "[at fighter]")
        {
            for (int j = 0; j <= 0x7; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_ATFIGHTER].push_back(row);
            }
        }
        else if (token == "[thief]")
        {
            for (int j = 0; j <= 0x5; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_THIEF].push_back(row);
            }
        }
        else if (token == "[mage]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_MAGE].push_back(row);
            }
        }
        else if (token == "[at gunner]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_ATGUNNER].push_back(row);
            }
        }
        else if (token == "[priest]")
        {
            for (int j = 0; j <= 0x9; ++j)
            {
                row.clear();
                for (int k = 0; k <= 0x9; ++k)
                {
                    val = ScanInt((bool*)0);
                    row.push_back(val);
                }
                out.m_slots[stNoviceCommand::SLOT_PRIEST].push_back(row);
            }
        }
    }
    return 1;
